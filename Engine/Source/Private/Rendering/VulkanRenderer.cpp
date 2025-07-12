#include "VulkanRenderer.hpp"
#include <SDL3/SDL_vulkan.h>
#include "vk_images.h"
#pragma warning(push, 0)
#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>
#pragma warning(pop)
#include "vk_loader.h"
#include "vk_pipelines.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl3.h"
#include "imgui/imgui_impl_vulkan.h"

#ifdef NDEBUG
constexpr bool USE_VALIDATION_LAYERS = false;
#else
constexpr bool USE_VALIDATION_LAYERS = true;
#endif

blackbox::VulkanRenderer::VulkanRenderer()
{
    VulkanRenderer::Initialize();
}

blackbox::VulkanRenderer::~VulkanRenderer()
{
    VulkanRenderer::Shutdown();
}

void blackbox::VulkanRenderer::Initialize()
{
    SDL_Init(SDL_INIT_VIDEO);

    constexpr SDL_WindowFlags windowFlags = SDL_WINDOW_VULKAN;
    window = SDL_CreateWindow(
        "Blackbox",
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
    InitImGui();
    InitDefaultData();
}

void blackbox::VulkanRenderer::Shutdown()
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

    for (auto& mesh : testMeshes)
    {
        DestroyBuffer(mesh->meshBuffers.indexBuffer);
        DestroyBuffer(mesh->meshBuffers.vertexBuffer);
    } 

    deletionQueue.Flush();
    
    DestroySwapchain();

    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyDevice(device, nullptr);

    vkb::destroy_debug_utils_messenger(instance, debugMessenger);
    vkDestroyInstance(instance, nullptr);
    SDL_DestroyWindow(window);
}

void blackbox::VulkanRenderer::Draw()
{
    // --------------- IMGUI START ---------------
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    if (ImGui::Begin("Background"))
    {
        ComputeEffect& selected = backgroundEffects[currentComputeEffectIndex];

        ImGui::Text("Selected effect: ", selected.name);
        ImGui::SliderInt("Effect index", &currentComputeEffectIndex, 0, static_cast<int>(backgroundEffects.size() - 1));

        ImGui::InputFloat4("data1", reinterpret_cast<float*>(&selected.data.data1));
        ImGui::InputFloat4("data2", reinterpret_cast<float*>(&selected.data.data2));
        ImGui::InputFloat4("data3", reinterpret_cast<float*>(&selected.data.data3));
        ImGui::InputFloat4("data4", reinterpret_cast<float*>(&selected.data.data4));
    }
    ImGui::End();

    ImGui::Render();
    // --------------- IMGUI END ---------------
    
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

    vkutil::TransitionImage(commandBuffer, drawImage.image, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

    DrawGeometry(commandBuffer);
    
    //transition the draw image and the swapchain image into their correct transfer layouts
    vkutil::TransitionImage(commandBuffer, drawImage.image, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
    vkutil::TransitionImage(commandBuffer, swapchainImages[swapchainImageIndex], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    // execute a copy from the draw image into the swapchain
    vkutil::CopyImageToImage(commandBuffer, drawImage.image, swapchainImages[swapchainImageIndex], drawExtent, swapchainExtent);

    // set swapchain image layout to Attachment Optimal so we can draw it
    vkutil::TransitionImage(commandBuffer, swapchainImages[swapchainImageIndex], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

    //draw imgui into the swapchain image
    DrawImGui(commandBuffer,  swapchainImageViews[swapchainImageIndex]);
    
    // set swapchain image layout to Present so we can show it on the screen
    vkutil::TransitionImage(commandBuffer, swapchainImages[swapchainImageIndex], VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
    
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

blackbox::GPUMeshBuffers blackbox::VulkanRenderer::UploadMesh(
    const std::span<uint32_t> indices,
    const std::span<Vertex> vertices
) {
    const size_t vertexBufferSize = vertices.size() * sizeof(Vertex);
    const size_t indexBufferSize = indices.size() * sizeof(uint32_t);

    GPUMeshBuffers newSurface {};
    
    // Create a vertex buffer
    newSurface.vertexBuffer = CreateBuffer(vertexBufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VMA_MEMORY_USAGE_GPU_ONLY);

    // Find the address of the vertex buffer
    VkBufferDeviceAddressInfo deviceAddressInfo
    {
        .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
        .buffer = newSurface.vertexBuffer.buffer,
    };
    newSurface.vertexBufferAddress = vkGetBufferDeviceAddress(device, &deviceAddressInfo);

    // Create index buffer
    newSurface.indexBuffer = CreateBuffer(indexBufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_GPU_ONLY);

    AllocatedBuffer staging = CreateBuffer(vertexBufferSize + indexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);

    void* data = staging.allocation->GetMappedData();

    // Copy vertex buffer
    memcpy(data, vertices.data(), vertexBufferSize);
    // Copy index buffer
    memcpy(static_cast<char*>(data) + vertexBufferSize, indices.data(), indexBufferSize);

    ImmediateSubmit([&](const VkCommandBuffer commandBuffer)
    {
        const VkBufferCopy vertexCopy
        {
            .srcOffset = 0,
            .dstOffset = 0,
            .size = vertexBufferSize,
        };
        vkCmdCopyBuffer(commandBuffer, staging.buffer, newSurface.vertexBuffer.buffer, 1, &vertexCopy);

        const VkBufferCopy indexCopy
        {
            .srcOffset = vertexBufferSize,
            .dstOffset = 0,
            .size = indexBufferSize,
        };
        vkCmdCopyBuffer(commandBuffer, staging.buffer, newSurface.indexBuffer.buffer, 1, &indexCopy);
    });

    DestroyBuffer(staging);

    return newSurface;
}

void blackbox::VulkanRenderer::DrawImGui(
    const VkCommandBuffer commandBuffer,
    const VkImageView targetImageView
) {
    VkRenderingAttachmentInfo colorAttachmentInfo = vkinit::AttachmentInfo(targetImageView, nullptr, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    VkRenderingInfo renderInfo = vkinit::RenderingInfo(swapchainExtent, &colorAttachmentInfo, nullptr);

    vkCmdBeginRendering(commandBuffer, &renderInfo);
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
    vkCmdEndRendering(commandBuffer);
}

void blackbox::VulkanRenderer::DrawGeometry(
    const VkCommandBuffer commandBuffer
) {
    // Begin a render pass connected to our draw image
    VkRenderingAttachmentInfo colorAttachmentInfo = vkinit::AttachmentInfo(drawImage.imageView, nullptr, VK_IMAGE_LAYOUT_GENERAL);
    VkRenderingAttachmentInfo depthAttachment = vkinit::DepthAttachmentInfo(depthImage.imageView, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);

    VkRenderingInfo renderInfo = vkinit::RenderingInfo(drawExtent, &colorAttachmentInfo, &depthAttachment);
    vkCmdBeginRendering(commandBuffer, &renderInfo);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, meshPipeline);

    // Set dynamic viewport and scissor
    const VkViewport viewport
    {
        .x = 0.0f,
        .y = 0.0f,
        .width = static_cast<float>(drawExtent.width),
        .height = static_cast<float>(drawExtent.height),
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    };
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    const VkRect2D scissor
    {
        .offset = {.x = 0, .y = 0},
        .extent = drawExtent,
    };
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    static float rotation = 0.0f;
    rotation += 1.0f;
    
    mat4 view = translate(mat4(1.0f), float3{0.0f, 0.0f, -5.f});
    mat4 projection = perspective(radians(70.0f), static_cast<float>(drawExtent.width) / static_cast<float>(drawExtent.height), 0.1f, 10000.0f);
    // Invert the Y-direction on the projection matrix so that we are more similar to opengl and gltf axis
    projection[1][1] *= -1;

    const GPUDrawPushConstants pushConstants
    {
        .worldMatrix = projection * view,
        .vertexBuffer = testMeshes[2]->meshBuffers.vertexBufferAddress,
    };

    vkCmdPushConstants(commandBuffer, meshPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(GPUDrawPushConstants), &pushConstants);
    vkCmdBindIndexBuffer(commandBuffer, testMeshes[2]->meshBuffers.indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);

    vkCmdDrawIndexed(commandBuffer, testMeshes[2]->surfaces[0].count, 1, testMeshes[2]->surfaces[0].startIndex, 0, 0);
    
    vkCmdEndRendering(commandBuffer);
}

void blackbox::VulkanRenderer::DrawBackground(
    const VkCommandBuffer commandBuffer
) {
    ComputeEffect& effect = backgroundEffects[currentComputeEffectIndex];
    
    // Bind the gradient drawing compute pipeline
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, effect.pipeline);

    // Bind the descriptor set container the draw image for the compute pipeline
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, gradientPipelineLayout, 0, 1, &drawImageDescriptors, 0, nullptr);

    ComputePushConstants pushConstants
    {
        .data1 = float4(1, 0, 0, 1),
        .data2 = float4(0, 0, 1, 1),
    };
    vkCmdPushConstants(commandBuffer, gradientPipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(pushConstants), &effect.data);
    
    // Execute the compute pipeline dispatch. We are using 16x16 workgroup size, so we need to divide by it
    vkCmdDispatch(commandBuffer, static_cast<uint32_t>(std::ceil(drawExtent.width / 16.0)), static_cast<uint32_t>(std::ceil(drawExtent.height / 16.0)), 1);
}

void blackbox::VulkanRenderer::InitVulkan()
{
    vkb::InstanceBuilder builder {};

    // Make the vulkan instance with basic debug features
    auto resultInstance = builder
        .set_app_name("Blackbox")
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
        .add_required_extension(VK_KHR_SHADER_RELAXED_EXTENDED_INSTRUCTION_EXTENSION_NAME)
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

void blackbox::VulkanRenderer::InitSwapchain()
{
    CreateSwapchain(windowExtent.width, windowExtent.height);

    // Draw image size will match the window
    const VkExtent3D drawImageExtent = {
        windowExtent.width,
        windowExtent.height,
        1
    };

    // Hardcoding the draw format to 32-bit float
    drawImage.imageFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
    drawImage.imageExtent = drawImageExtent;

    VkImageUsageFlags drawImageUsages {};
    drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    drawImageUsages |= VK_IMAGE_USAGE_STORAGE_BIT;
    drawImageUsages |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    VkImageCreateInfo drawImageInfo = vkinit::ImageCreateInfo(drawImage.imageFormat, drawImageUsages, drawImageExtent);

    // For the draw image, we want to allocate it from gpu local memory
    VmaAllocationCreateInfo drawImageAllocInfo {};
    drawImageAllocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    drawImageAllocInfo.requiredFlags = static_cast<VkMemoryPropertyFlags>(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    // Allocate and create the image
    vmaCreateImage(allocator, &drawImageInfo, &drawImageAllocInfo, &drawImage.image, &drawImage.allocation, nullptr);

    // Build an image-view for the draw image to use for rendering
    VkImageViewCreateInfo drawViewInfo = vkinit::ImageviewCreateInfo(drawImage.imageFormat, drawImage.image, VK_IMAGE_ASPECT_COLOR_BIT);

    VK_CHECK(vkCreateImageView(device, &drawViewInfo, nullptr, &drawImage.imageView));

    depthImage.imageFormat = VK_FORMAT_D32_SFLOAT;
    depthImage.imageExtent = drawImageExtent;

    VkImageUsageFlags depthImageUsages {};
    depthImageUsages |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

    VkImageCreateInfo depthImageInfo = vkinit::ImageCreateInfo(depthImage.imageFormat, depthImageUsages, drawImageExtent);

    // Allocate and create the image
    vmaCreateImage(allocator, &depthImageInfo, &drawImageAllocInfo, &depthImage.image, &depthImage.allocation, nullptr);

    // Build an image-view for the draw image to use for rendering
    VkImageViewCreateInfo depthViewInfo = vkinit::ImageviewCreateInfo(depthImage.imageFormat, depthImage.image, VK_IMAGE_ASPECT_DEPTH_BIT);

    VK_CHECK(vkCreateImageView(device, &depthViewInfo, nullptr, &depthImage.imageView));
    
    // Add to deletion queues
    deletionQueue.Add([=]() {
        vkDestroyImageView(device, drawImage.imageView, nullptr);
        vmaDestroyImage(allocator, drawImage.image, drawImage.allocation);

        vkDestroyImageView(device, depthImage.imageView, nullptr);
        vmaDestroyImage(allocator, depthImage.image, depthImage.allocation);
    });
}

void blackbox::VulkanRenderer::InitCommands()
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

void blackbox::VulkanRenderer::InitSyncStructures()
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

void blackbox::VulkanRenderer::InitDescriptors()
{
    // Create a descriptor pool that will hold 10 sets with 1 image each
    std::vector<DescriptorAllocator::PoolSizeRatio> sizes = {{.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, .ratio = 1}};
    globalDescriptorAllocator.InitPool(device, 10, sizes);

    // Make the descriptor set a layout for our compute draw
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

void blackbox::VulkanRenderer::InitPipelines()
{
    // Compute pipelines
    InitBackgroundPipelines();

    // Graphics pipelines
    InitMeshPipeline();
}

void blackbox::VulkanRenderer::InitBackgroundPipelines()
{
    constexpr VkPushConstantRange pushConstants
    {
        .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
        .offset = 0,
        .size = sizeof(ComputePushConstants),
    };

    const VkPipelineLayoutCreateInfo computeLayout
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = 1,
        .pSetLayouts = &drawImageDescriptorLayout,
        .pushConstantRangeCount = 1,
        .pPushConstantRanges = &pushConstants,
    };

    VK_CHECK(vkCreatePipelineLayout(device, &computeLayout, nullptr, &gradientPipelineLayout));

    // Layout code
    VkShaderModule gradientShader {};
    if (!vkutil::LoadShaderModule("Shaders/gradient_color.comp", device, &gradientShader))
    {
        LogRenderer->Error("Error when building the compute shader. Shader: {}", "gradient_color.comp");
    }
    
    VkShaderModule skyShader {};
    if (!vkutil::LoadShaderModule("Shaders/sky.comp", device, &skyShader))
    {
        LogRenderer->Error("Error when building the compute shader. Shader: {}", "sky.comp");
    }

    const VkPipelineShaderStageCreateInfo shaderStageInfo
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_COMPUTE_BIT,
        .module = gradientShader,
        .pName = "main",
    };

    VkComputePipelineCreateInfo computePipelineInfo
    {
        .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
        .stage = shaderStageInfo,
        .layout = gradientPipelineLayout,
    };

    ComputeEffect gradient
    {
        .name = "gradient",
        .layout = gradientPipelineLayout,
        .data =
        {
            .data1 = float4(1, 0, 0, 1), 
            .data2 = float4(0, 0, 1, 1),
        },
    };

    VK_CHECK(vkCreateComputePipelines(device, VK_NULL_HANDLE, 1, &computePipelineInfo, nullptr, &gradient.pipeline));

    // Change the shader module only to create the sky shader
    computePipelineInfo.stage.module = skyShader;

    ComputeEffect sky
    {
        .name = "sky",
        .layout = gradientPipelineLayout,
        .data =
        {
            .data1 = float4(0.1, 0.2, 0.4, 0.97), 
        },
    };

    VK_CHECK(vkCreateComputePipelines(device, VK_NULL_HANDLE, 1, &computePipelineInfo, nullptr, &sky.pipeline));

    // Add the two background effects into the array
    backgroundEffects.push_back(gradient);
    backgroundEffects.push_back(sky);

    // Clean up
    vkDestroyShaderModule(device, gradientShader, nullptr);
    vkDestroyShaderModule(device, skyShader, nullptr);
    
    deletionQueue.Add([=]() {
        vkDestroyPipelineLayout(device, gradientPipelineLayout, nullptr);
        vkDestroyPipeline(device, gradient.pipeline, nullptr);
        vkDestroyPipeline(device, sky.pipeline, nullptr);
    });
}

void blackbox::VulkanRenderer::InitMeshPipeline()
{
    VkShaderModule triangleFragShader {};
    if (!vkutil::LoadShaderModule("Shaders/colored_triangle.frag", device, &triangleFragShader))
    {
        LogRenderer->Error("Error when building the mesh fragment shader.");
    }
    LogRenderer->Trace("Triangle fragment shader successfully loaded.");

    VkShaderModule triangleVertShader {};
    if (!vkutil::LoadShaderModule("Shaders/colored_triangle_mesh.vert", device, &triangleVertShader))
    {
        LogRenderer->Error("Error when building the mesh vertex shader.");
    }
    LogRenderer->Trace("Triangle vertex shader successfully loaded.");

    VkPushConstantRange pushConstants
    {
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
        .offset = 0,
        .size = sizeof(GPUDrawPushConstants),
    };
    
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = vkinit::PipelineLayoutCreateInfo();
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstants;
    
    VK_CHECK(vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &meshPipelineLayout));

    PipelineBuilder pipelineBuilder {};
    pipelineBuilder.pipelineLayout = meshPipelineLayout;
    pipelineBuilder.SetShaders(triangleVertShader, triangleFragShader);
    pipelineBuilder.SetInputTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
    pipelineBuilder.SetPolygonMode(VK_POLYGON_MODE_FILL);
    pipelineBuilder.SetCullMode(VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE);
    pipelineBuilder.SetMultisamplingNone();
    pipelineBuilder.EnableBlendingAdditive();
    pipelineBuilder.EnableDepthTest(true, VK_COMPARE_OP_LESS_OR_EQUAL);

    // Connect the image format we will draw into, from draw image
    pipelineBuilder.SetColorAttachmentFormat(drawImage.imageFormat);
    pipelineBuilder.SetDepthFormat(depthImage.imageFormat);

    // Finally, build the pipeline
    meshPipeline = pipelineBuilder.Build(device);

    // Clean structures
    vkDestroyShaderModule(device, triangleFragShader, nullptr);
    vkDestroyShaderModule(device, triangleVertShader, nullptr);
    deletionQueue.Add([&]()
    {
        vkDestroyPipelineLayout(device, meshPipelineLayout, nullptr);
        vkDestroyPipeline(device, meshPipeline, nullptr);
    });
}

void blackbox::VulkanRenderer::InitDefaultData()
{
    testMeshes = LoadGltfMesh(this, "Content/basicmesh.glb").value();
}

void blackbox::VulkanRenderer::CreateSwapchain(
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

void blackbox::VulkanRenderer::DestroySwapchain()
{
    vkDestroySwapchainKHR(device, swapchain, nullptr);

    // Destroy swapchain resources
    for (const auto& swapchainImageView : swapchainImageViews)
    {
        vkDestroyImageView(device, swapchainImageView, nullptr);
    } 
}

void blackbox::VulkanRenderer::ImmediateSubmit(
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

void blackbox::VulkanRenderer::InitImGui()
{
    // 1: Create descriptor pool for ImGui
    // The size of the pool is very oversized, but it's copied from the ImGui demo itself
    VkDescriptorPoolSize poolSizes[] = {
        {VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
        {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
        {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
        {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000},
    };

    const VkDescriptorPoolCreateInfo poolInfo
    {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
        .maxSets = 1000,
        .poolSizeCount = static_cast<uint32_t>(std::size(poolSizes)),
        .pPoolSizes = poolSizes,
    };

    VkDescriptorPool imguiPool {};
    VK_CHECK(vkCreateDescriptorPool(device, &poolInfo, nullptr, &imguiPool));

    // 2: Initialize ImGui library
    // This initialized the core structures of ImGui
    ImGui::CreateContext();

    // This initialized ImGui for SDL
    ImGui_ImplSDL3_InitForVulkan(window);

    // This initialized ImGui for Vulkan
    ImGui_ImplVulkan_InitInfo initInfo
    {
        .Instance = instance,
        .PhysicalDevice = selectedGPU,
        .Device = device,
        .Queue = graphicsQueue,
        .DescriptorPool = imguiPool,
        .MinImageCount = 3,
        .ImageCount = 3,
        .MSAASamples = VK_SAMPLE_COUNT_1_BIT,
        .UseDynamicRendering = true,
        
        // Dynamic rendering parameters for ImGui to use
        .PipelineRenderingCreateInfo {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
            .colorAttachmentCount = 1,
            .pColorAttachmentFormats = &swapchainImageFormat,
        },
    };

    ImGui_ImplVulkan_Init(&initInfo);

    deletionQueue.Add([=]() {
        ImGui_ImplVulkan_Shutdown();
        vkDestroyDescriptorPool(device, imguiPool, nullptr);
    });
}

blackbox::AllocatedBuffer blackbox::VulkanRenderer::CreateBuffer(
    const size_t allocSize,
    const VkBufferUsageFlags usage,
    const VmaMemoryUsage memoryUsage
) {
    // Allocate buffer
    VkBufferCreateInfo bufferInfo
    {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = allocSize,
        .usage = usage,
    };

    VmaAllocationCreateInfo vmaAllocInfo
    {
        .flags = VMA_ALLOCATION_CREATE_MAPPED_BIT,
        .usage = memoryUsage,
    };

    AllocatedBuffer newBuffer {};

    // Allocate the buffer
    VK_CHECK(vmaCreateBuffer(allocator, &bufferInfo, &vmaAllocInfo, &newBuffer.buffer, &newBuffer.allocation, &newBuffer.info));

    return newBuffer;
}

void blackbox::VulkanRenderer::DestroyBuffer(
    const AllocatedBuffer& buffer
) {
    vmaDestroyBuffer(allocator, buffer.buffer, buffer.allocation);
}
