#include "VulkanRenderer.hpp"
#include <SDL3/SDL_vulkan.h>
#include "vk_images.h"
#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>
#include "vk_pipelines.h"

#ifdef NDEBUG
constexpr bool USE_VALIDATION_LAYERS = false;
#else
constexpr bool USE_VALIDATION_LAYERS = true;
#endif

Stasis::VulkanRenderer::VulkanRenderer()
{
    VulkanRenderer::Initialize();
}

Stasis::VulkanRenderer::~VulkanRenderer()
{
    VulkanRenderer::Shutdown();
}

void Stasis::VulkanRenderer::Initialize()
{
    SDL_Init(SDL_INIT_VIDEO);

    constexpr SDL_WindowFlags windowFlags = SDL_WINDOW_VULKAN;
    window = SDL_CreateWindow(
        "Stasis Engine",
        static_cast<int32_t>(windowExtent.width),
        static_cast<int32_t>(windowExtent.height),
        windowFlags
    );

    InitVulkan();
    InitSwapchain();
    InitCommands();
    InitSyncStructures();
    InitDescriptors();
    InitPipelines();
}

void Stasis::VulkanRenderer::Shutdown()
{
    // Make sure the GPU has stopped doing its thing
    vkDeviceWaitIdle(device);

    for (auto& frame : frames)
    {
        vkDestroyCommandPool(device, frame.commandPool, nullptr);

        // Destroy sync objects
        vkDestroyFence(device, frame.renderFence, nullptr);
        vkDestroySemaphore(device, frame.swapchainSemaphore, nullptr);
        
        frame.deletionQueue.Flush();
    }

    for (const auto& renderSemaphore : renderSemaphores)
    {
        vkDestroySemaphore(device, renderSemaphore, nullptr);
    }

    deletionQueue.Flush();
    
    DestroySwapchain();

    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyDevice(device, nullptr);

    vkb::destroy_debug_utils_messenger(instance, debugMessenger);
    vkDestroyInstance(instance, nullptr);
    SDL_DestroyWindow(window);
}

void Stasis::VulkanRenderer::Draw()
{
    constexpr uint32_t singleSecond = 1000000000;
    
    // Wait until the GPU has finished rendering the last frame. Timeout of one second
    VK_CHECK(vkWaitForFences(device, 1, &GetCurrentFrame().renderFence, true, singleSecond));
    GetCurrentFrame().deletionQueue.Flush();    
    VK_CHECK(vkResetFences(device, 1, &GetCurrentFrame().renderFence));

    // Request image from the swapchain
    uint32_t swapchainImageIndex {};
    VK_CHECK(vkAcquireNextImageKHR(device, swapchain, singleSecond, GetCurrentFrame().swapchainSemaphore, nullptr, &swapchainImageIndex));

    // Reset and restart the command buffer
    const VkCommandBuffer commandBuffer = GetCurrentFrame().commandBuffer;

    // Now that we are sure that the commands finished executing,
    // we can safely reset the command buffer to begin recording again.
    VK_CHECK(vkResetCommandBuffer(commandBuffer, 0));

    // Begin the command buffer recording. We will use this command buffer exactly once, so we want to let Vulkan know that.
    const VkCommandBufferBeginInfo commandBufferBeginInfo = vkinit::CommandBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    drawExtent.width = drawImage.imageExtent.width;
    drawExtent.height = drawImage.imageExtent.height;

    VK_CHECK(vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo));	

    // transition our main draw image into general layout so we can write into it
    // we will overwrite it all so we dont care about what was the older layout
    vkutil::TransitionImage(commandBuffer, drawImage.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

    DrawBackground(commandBuffer);

    //transition the draw image and the swapchain image into their correct transfer layouts
    vkutil::TransitionImage(commandBuffer, drawImage.image, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
    vkutil::TransitionImage(commandBuffer, swapchainImages[swapchainImageIndex], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    // execute a copy from the draw image into the swapchain
    vkutil::CopyImageToImage(commandBuffer, drawImage.image, swapchainImages[swapchainImageIndex], drawExtent, swapchainExtent);

    // set swapchain image layout to Present so we can show it on the screen
    vkutil::TransitionImage(commandBuffer, swapchainImages[swapchainImageIndex], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

    //finalize the command buffer (we can no longer add commands, but it can now be executed)
    VK_CHECK(vkEndCommandBuffer(commandBuffer));

    // Prepare the submission to the queue.
    // We want to wait on the swapchainSemaphore, as that semaphore is signaled when the swapchain is ready.
    // We will signal the renderSemaphore, to signal that rendering has finished.
    VkCommandBufferSubmitInfo commandBufferSubmitInfo = vkinit::CommandBufferSubmitInfo(commandBuffer);
    VkSemaphoreSubmitInfo waitInfo = vkinit::SemaphoreSubmitInfo(VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR, GetCurrentFrame().swapchainSemaphore);
    VkSemaphoreSubmitInfo signalInfo = vkinit::SemaphoreSubmitInfo(VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT, renderSemaphores[swapchainImageIndex]);
    VkSubmitInfo2 submit = vkinit::SubmitInfo(&commandBufferSubmitInfo, &signalInfo, &waitInfo);

    // Submit the command buffer to the queue and execute it.
    // renderFence will now block until the graphic commands finish executing.
    VK_CHECK(vkQueueSubmit2(graphicsQueue, 1, &submit, GetCurrentFrame().renderFence));

    // Prepare present
    // This will put the image we just rendered to into the visible window.
    // We want to wait on the renderSemaphore for that, as it's necessary that
    // the drawing commands have finished before the image is displayed to the user.
    const VkPresentInfoKHR presentInfo
    {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &renderSemaphores[swapchainImageIndex],
        .swapchainCount = 1,
        .pSwapchains = &swapchain,
        .pImageIndices = &swapchainImageIndex,
    };

    VK_CHECK(vkQueuePresentKHR(graphicsQueue, &presentInfo));

    // Increase the number of frames drawn.
    frameNumber++;
}

void Stasis::VulkanRenderer::DrawBackground(VkCommandBuffer commandBuffer)
{
    // Bind the gradient drawing compute pipeline
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, gradientPipeline);

    // Bind the descriptor set container the draw image for the compute pipeline
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, gradientPipelineLayout, 0, 1, &drawImageDescriptors, 0, nullptr);

    // Execute the compute pipeline dispatch. We are using 16x16 workgroup size so we need to divide by it
    vkCmdDispatch(commandBuffer, static_cast<uint32_t>(std::ceil(drawExtent.width / 16.0)), static_cast<uint32_t>(std::ceil(drawExtent.height / 16.0)), 1);
}

void Stasis::VulkanRenderer::InitVulkan()
{
    vkb::InstanceBuilder builder {};

    // Make the vulkan instance with basic debug features
    auto resultInstance = builder
        .set_app_name("Stasis Engine")
        .request_validation_layers(USE_VALIDATION_LAYERS)
        .use_default_debug_messenger()
        .require_api_version(1, 3, 0)
        .build();

    vkb::Instance vkbInstance = resultInstance.value();
    instance = vkbInstance.instance;
    debugMessenger = resultInstance.value().debug_messenger;

    SDL_Vulkan_CreateSurface(window, instance, nullptr, &surface);

    // Vulkan 1.3 features
    VkPhysicalDeviceVulkan13Features vk13Features
    {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
        .synchronization2 = true,
        .dynamicRendering = true,
    };

    // Vulkan 1.2 features
    VkPhysicalDeviceVulkan12Features vk12Features
    {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES,
        .descriptorIndexing = true,
        .bufferDeviceAddress = true,
    };

    // Use vkbootstrap to select a GPU
    // We want a GPU that can write to SDL surface and supports vulkan 1.3 with the correct features
    vkb::PhysicalDeviceSelector selector {vkbInstance};
    vkb::PhysicalDevice physicalDevice = selector
        .set_minimum_version(1, 3)
        .set_required_features_13(vk13Features)
        .set_required_features_12(vk12Features)
        .set_surface(surface)
        .select()
        .value();

    // Create the final Vulkan device
    vkb::DeviceBuilder deviceBuilder {physicalDevice};
    vkb::Device vkbDevice = deviceBuilder.build().value();

    // Get the vkDevice handle used in the rest of the Vulkan application
    device = vkbDevice.device;
    selectedGPU = physicalDevice.physical_device;

    // Use vkbootstrap to get a graphics queue
    graphicsQueue = vkbDevice.get_queue(vkb::QueueType::graphics).value();
    graphicsQueueFamily = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();

    // Initialize the memory allocator
    VmaAllocatorCreateInfo allocatorInfo = {};
    allocatorInfo.physicalDevice = selectedGPU;
    allocatorInfo.device = device;
    allocatorInfo.instance = instance;
    allocatorInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
    vmaCreateAllocator(&allocatorInfo, &allocator);

    deletionQueue.Add([&]() {
        vmaDestroyAllocator(allocator);
    });
}

void Stasis::VulkanRenderer::InitSwapchain()
{
    CreateSwapchain(windowExtent.width, windowExtent.height);

    //draw image size will match the window
    VkExtent3D drawImageExtent = {
        windowExtent.width,
        windowExtent.height,
        1
    };

    //hardcoding the draw format to 32-bit float
    drawImage.imageFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
    drawImage.imageExtent = drawImageExtent;

    VkImageUsageFlags drawImageUsages{};
    drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    drawImageUsages |= VK_IMAGE_USAGE_STORAGE_BIT;
    drawImageUsages |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    VkImageCreateInfo drawImageInfo = vkinit::ImageCreateInfo(drawImage.imageFormat, drawImageUsages, drawImageExtent);

    //for the draw image, we want to allocate it from gpu local memory
    VmaAllocationCreateInfo drawImageAllocInfo = {};
    drawImageAllocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    drawImageAllocInfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    //allocate and create the image
    vmaCreateImage(allocator, &drawImageInfo, &drawImageAllocInfo, &drawImage.image, &drawImage.allocation, nullptr);

    //build a image-view for the draw image to use for rendering
    VkImageViewCreateInfo drawViewInfo = vkinit::ImageviewCreateInfo(drawImage.imageFormat, drawImage.image, VK_IMAGE_ASPECT_COLOR_BIT);

    VK_CHECK(vkCreateImageView(device, &drawViewInfo, nullptr, &drawImage.imageView));

    //add to deletion queues
    deletionQueue.Add([=]() {
        vkDestroyImageView(device, drawImage.imageView, nullptr);
        vmaDestroyImage(allocator, drawImage.image, drawImage.allocation);
    });
}

void Stasis::VulkanRenderer::InitCommands()
{
    // Create a command pool for commands submitted to the graphics queue.
    // We also want the pool to allow for resetting of individual command buffers.
    const VkCommandPoolCreateInfo commandPoolInfo = vkinit::CommandPoolCreateInfo(graphicsQueueFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

    for (auto& frame : frames)
    {
        VK_CHECK(vkCreateCommandPool(device, &commandPoolInfo, nullptr, &frame.commandPool));

        // Allocate the default command buffer that we will use for rendering
        VkCommandBufferAllocateInfo commandBufferAllocInfo = vkinit::CommandBufferAllocateInfo(frame.commandPool, 1);

        VK_CHECK(vkAllocateCommandBuffers(device, &commandBufferAllocInfo, &frame.commandBuffer));
    }

    VK_CHECK(vkCreateCommandPool(device, &commandPoolInfo, nullptr, &immediateCommandPool));

    // Allocate the command buffer for immediate submits
    VkCommandBufferAllocateInfo commandAllocInfo = vkinit::CommandBufferAllocateInfo(immediateCommandPool, 1);
    VK_CHECK(vkAllocateCommandBuffers(device, &commandAllocInfo, &immediateCommandBuffer));

    deletionQueue.Add([=]()
    {
        vkDestroyCommandPool(device, immediateCommandPool, nullptr);
    });
}

void Stasis::VulkanRenderer::InitSyncStructures()
{
    // Create sync structures
    // One fence to control when the GPU has finished rendering the frame,
    // and two semaphores to sync rendering with the swapchain.
    // We want the fence to start "signaled", so we can wait on it the first time
    VkFenceCreateInfo fenceCreateInfo = vkinit::FenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);
    VkSemaphoreCreateInfo semaphoreCreateInfo = vkinit::SemaphoreCreateInfo();

    for (auto& frame : frames)
    {
        VK_CHECK(vkCreateFence(device, &fenceCreateInfo, nullptr, &frame.renderFence));
        VK_CHECK(vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &frame.swapchainSemaphore));
    }

    renderSemaphores.clear();
    renderSemaphores.resize(swapchainImages.size());
    for (auto& renderSemaphore : renderSemaphores) 
    {
        VK_CHECK(vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &renderSemaphore));
    }

    VK_CHECK(vkCreateFence(device, &fenceCreateInfo, nullptr, &immediateFence));
    deletionQueue.Add([=]()
    {
        vkDestroyFence(device, immediateFence, nullptr);
    });
}

void Stasis::VulkanRenderer::InitDescriptors()
{
    // Create a descriptor pool that will hold 10 sets with 1 image each
    std::vector<DescriptorAllocator::PoolSizeRatio> sizes = {{VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1}};
    globalDescriptorAllocator.InitPool(device, 10, sizes);

    // Make the descriptor set layout for our compute draw
    {
        DescriptorLayoutBuilder builder {};
        builder.AddBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
        drawImageDescriptorLayout = builder.Build(device, VK_SHADER_STAGE_COMPUTE_BIT);
    }

    // Allocate a descriptor set for our draw image
    drawImageDescriptors = globalDescriptorAllocator.Allocate(device, drawImageDescriptorLayout);

    VkDescriptorImageInfo imageInfo
    {
        .imageView = drawImage.imageView,
        .imageLayout = VK_IMAGE_LAYOUT_GENERAL,
    };

    VkWriteDescriptorSet drawImageWrite
    {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = drawImageDescriptors,
        .dstBinding = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
        .pImageInfo = &imageInfo,
    };

    vkUpdateDescriptorSets(device, 1, &drawImageWrite, 0, nullptr);

    // Make sure both the descriptor allocators and the new layout get cleaned up properly
    deletionQueue.Add([&]()
    {
        globalDescriptorAllocator.DestroyPool(device);
        vkDestroyDescriptorSetLayout(device, drawImageDescriptorLayout, nullptr);
    });
}

void Stasis::VulkanRenderer::InitPipelines()
{
    InitBackgroundPipelines();
}

void Stasis::VulkanRenderer::InitBackgroundPipelines()
{
    const VkPipelineLayoutCreateInfo computeLayout
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext = nullptr,
        .setLayoutCount = 1,
        .pSetLayouts = &drawImageDescriptorLayout,
        .pPushConstantRanges = nullptr,
    };

    VK_CHECK(vkCreatePipelineLayout(device, &computeLayout, nullptr, &gradientPipelineLayout));

    // Layout code
    VkShaderModule computerDrawShader {};
    if (!vkutil::LoadShaderModule("Shaders/gradient.comp.spv", device, &computerDrawShader))
    {
        LogRenderer->Error("Error when building the compute shader.");
    }

    const VkPipelineShaderStageCreateInfo shaderStageInfo
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_COMPUTE_BIT,
        .module = computerDrawShader,
        .pName = "main",
    };

    const VkComputePipelineCreateInfo computePipelineInfo
    {
        .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
        .stage = shaderStageInfo,
        .layout = gradientPipelineLayout,
    };

    VK_CHECK(vkCreateComputePipelines(device, VK_NULL_HANDLE, 1, &computePipelineInfo, nullptr, &gradientPipeline));

    vkDestroyShaderModule(device, computerDrawShader, nullptr);
    deletionQueue.Add([&]() {
        vkDestroyPipeline(device, gradientPipeline, nullptr);
        vkDestroyPipelineLayout(device, gradientPipelineLayout, nullptr);
    });
}

void Stasis::VulkanRenderer::CreateSwapchain(
    const uint32_t width,
    const uint32_t height
) {
    vkb::SwapchainBuilder swapchainBuilder {selectedGPU, device, surface};
    swapchainImageFormat = VK_FORMAT_B8G8R8A8_UNORM;

    vkb::Swapchain vkbSwapchain = swapchainBuilder
        .set_desired_format(VkSurfaceFormatKHR{.format = swapchainImageFormat, .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR})
        .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR) // Use vsync present mode
        .set_desired_extent(width, height)
        .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
        .build()
        .value();

    swapchainExtent = vkbSwapchain.extent;
    swapchain = vkbSwapchain.swapchain;
    swapchainImages = vkbSwapchain.get_images().value();
    swapchainImageViews = vkbSwapchain.get_image_views().value();
}

void Stasis::VulkanRenderer::DestroySwapchain()
{
    vkDestroySwapchainKHR(device, swapchain, nullptr);

    // Destroy swapchain resources
    for (const auto& swapchainImageView : swapchainImageViews)
    {
        vkDestroyImageView(device, swapchainImageView, nullptr);
    } 
}

void Stasis::VulkanRenderer::ImmediateSubmit(
    std::function<void(VkCommandBuffer)>&& callback
) {
    VK_CHECK(vkResetFences(device, 1, &immediateFence));
    VK_CHECK(vkResetCommandBuffer(immediateCommandBuffer, 0));

    VkCommandBuffer commandBuffer = immediateCommandBuffer;
    VkCommandBufferBeginInfo commandBufferBeginInfo = vkinit::CommandBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    VK_CHECK(vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo));

    callback(commandBuffer);

    VK_CHECK(vkEndCommandBuffer(commandBuffer));

    VkCommandBufferSubmitInfo commandBufferSubmitInfo = vkinit::CommandBufferSubmitInfo(commandBuffer);
    VkSubmitInfo2 submit = vkinit::SubmitInfo(&commandBufferSubmitInfo, nullptr, nullptr);

    // Submit command buffer to the queue and execute it
    // renderFence will now block until the graphic commands finish execution
    VK_CHECK(vkQueueSubmit2(graphicsQueue, 1, &submit, immediateFence));
    VK_CHECK(vkWaitForFences(device, 1, &immediateFence, VK_TRUE, UINT64_MAX));
}

void Stasis::VulkanRenderer::InitImGui()
{
}
