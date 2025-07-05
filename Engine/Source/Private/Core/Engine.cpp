#include "Engine.hpp"

#include <chrono>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include "Rendering/vk_initializers.h"
#include "Rendering/vk_images.h"
#include "Stasis.hpp"

#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

Stasis::Engine Engine;

#ifdef NDEBUG
    constexpr bool UseValidationLayers = false;
#else
    constexpr bool UseValidationLayers = true;
#endif


void Stasis::Engine::Initialize()
{
    LogEngine->Trace("Initializing Engine...");

    SDL_Init(SDL_INIT_VIDEO);

    constexpr SDL_WindowFlags WindowFlags = SDL_WINDOW_VULKAN;
    Window = SDL_CreateWindow("Stasis Engine", WindowExtent.width, WindowExtent.height, WindowFlags);

    InitVulkan();
    InitSwapChain();
    InitCommands();
    InitSyncStructures();
    
    IsInitialized = true;
}

void Stasis::Engine::Run()
{
    // Do not run engine if not successfully initialized
    if (!IsInitialized) return;
    
    auto PreviousTime = std::chrono::high_resolution_clock::now();
    SDL_Event Event;
    
    while (IsRunning)
    {
        const auto CurrentTime = std::chrono::high_resolution_clock::now();
        const float Elapsed = static_cast<float>(std::chrono::duration_cast<std::chrono::microseconds>(CurrentTime - PreviousTime).count());
        const float DeltaTime = Elapsed / 1000000.0f; // time in seconds
        const float FrameTime = Elapsed / 1000.0f; // time in milliseconds
        PreviousTime = CurrentTime;

        LogEngine->Trace("FPS: {:.1f} ({}ms)", 1000.0f / FrameTime, FrameTime);
        while (SDL_PollEvent(&Event))
        {
            // Close the window when the user ALT-F4s or closes the window
            if (Event.type == SDL_EVENT_QUIT)
            {
                IsRunning = false;
            }

            // Pause rendering when the window is minimized or loses focus
            if (Event.type == SDL_EVENT_WINDOW_MINIMIZED || Event.type == SDL_EVENT_WINDOW_FOCUS_LOST)
            {
                StopRendering = true;
            }
            if (Event.type == SDL_EVENT_WINDOW_RESTORED || Event.type == SDL_EVENT_WINDOW_FOCUS_GAINED)
            {
                StopRendering = false;
            }
        }
        
        // Do not draw if we are minimized
        if (StopRendering)
        {
            // Throttle the speed to avoid the endless spinning
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        Draw();
    }
}

void Stasis::Engine::Draw()
{
    // Wait until the gpu has finished rendering the last frame. Timeout of 1 second
    VK_CHECK(vkWaitForFences(Device, 1, &GetCurrentFrame().RenderFence, true, 1000000000));

    GetCurrentFrame().DeletionQueue.Flush();
    
    VK_CHECK(vkResetFences(Device, 1, &GetCurrentFrame().RenderFence));

    //request image from the swapchain
    uint32_t SwapchainImageIndex;
    VK_CHECK(vkAcquireNextImageKHR(Device, SwapChain, 1000000000, GetCurrentFrame().SwapchainSemaphore, nullptr, &SwapchainImageIndex));

    VkCommandBuffer CommandBuffer = GetCurrentFrame().CommandBuffer;

    // Now that we are sure that the commands finished executing, we can safely reset the command buffer to begin recording again.
    VK_CHECK(vkResetCommandBuffer(CommandBuffer, 0));
    
    // Begin the command buffer recording. We will use this command buffer exactly once, so we want to let vulkan know that.
    VkCommandBufferBeginInfo CommandBufferBeginInfo = vkinit::CommandBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    // vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

    DrawExtent.width = DrawImage.ImageExtent.width;
    DrawExtent.height = DrawImage.ImageExtent.height;
    
    // Start the command buffer recording.
    VK_CHECK(vkBeginCommandBuffer(CommandBuffer, &CommandBufferBeginInfo));

    // Transition our main draw image into a general layout so we can write into it,
    // We will overwrite it all, so we don't care about what was the older layout.
    vkutil::TransitionImage(CommandBuffer, DrawImage.Image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

    DrawBackground(CommandBuffer);

    // Transition the draw image and the swapchain image into their correct transfer layouts.
    vkutil::TransitionImage(CommandBuffer, DrawImage.Image, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
    vkutil::TransitionImage(CommandBuffer, SwapChainImages[SwapchainImageIndex], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    // Execute a copy from the draw image into the swapchain
    vkutil::CopyImageToImage(CommandBuffer, DrawImage.Image, SwapChainImages[SwapchainImageIndex], DrawExtent, SwapChainExtent);

    // Set the swapchain image layout to Present so we can show it on screen.
    vkutil::TransitionImage(CommandBuffer, SwapChainImages[SwapchainImageIndex], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

    // Finalize the command buffer (we can no longer add commands, but it can now be executed)
    VK_CHECK(vkEndCommandBuffer(CommandBuffer));

    // Prepare the submission to the queue
    // We want to wait on the RenderSemaphore as that semaphore is signaled when the swapchain is ready
    // We will signal the RenderSemaphore to signal that rendering has finished
    VkCommandBufferSubmitInfo CommandInfo = vkinit::CommandBufferSubmitInfo(CommandBuffer);
    VkSemaphoreSubmitInfo WaitInfo = vkinit::SemaphoreSubmitInfo(VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR, GetCurrentFrame().SwapchainSemaphore);
    VkSemaphoreSubmitInfo SignalInfo = vkinit::SemaphoreSubmitInfo(VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT_KHR, GetCurrentFrame().RenderSemaphore);
    VkSubmitInfo2 Submit = vkinit::SubmitInfo(&CommandInfo, &SignalInfo, &WaitInfo);

    // Submit the command buffer to the queue and execute it.
    // RenderFence will now block the graphics commands finish execution
    VK_CHECK(vkQueueSubmit2(GraphicsQueue, 1, &Submit, GetCurrentFrame().RenderFence));
    
    // This will put the image we just rendered to into the visible window.
    // We want to wait on the RenderSemaphore for that as it's necessary that drawing commands
    // have finished before the image is displayed to the user.
    VkPresentInfoKHR PresentInfo {};
    PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    PresentInfo.pNext = nullptr;
    PresentInfo.pSwapchains = &SwapChain;
    PresentInfo.swapchainCount = 1;

    PresentInfo.pWaitSemaphores = &GetCurrentFrame().RenderSemaphore;
    PresentInfo.waitSemaphoreCount = 1;
    
    PresentInfo.pImageIndices = &SwapchainImageIndex;

    VK_CHECK(vkQueuePresentKHR(GraphicsQueue, &PresentInfo));
    
    FrameNumber++;
}

void Stasis::Engine::DrawBackground(
    VkCommandBuffer CommandBuffer
) {
    // Make a clear-color from the frame number. This will flash with a 120-frame period.
    VkClearColorValue ClearValue {};
    float Flash = std::abs(std::sin(FrameNumber / 1440.f));
    ClearValue = {{0.0f, 0.0f, Flash, 1.0f}};
    VkImageSubresourceRange ClearRange = vkinit::ImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT);

    // Clear image
    vkCmdClearColorImage(CommandBuffer, DrawImage.Image, VK_IMAGE_LAYOUT_GENERAL, &ClearValue, 1, &ClearRange);
}

void Stasis::Engine::Shutdown()
{
    LogEngine->Trace("Shutting Down Engine...");

    if (IsInitialized)
    {
        // Make sure the GPU has stopped doing its things
        vkDeviceWaitIdle(Device);

        // Free per-frame structures and deletion queue
        for (auto& Frame : Frames)
        {
            vkDestroyCommandPool(Device, Frame.CommandPool, nullptr);

            // Destroy sync objects
            vkDestroyFence(Device, Frame.RenderFence, nullptr);
            vkDestroySemaphore(Device, Frame.RenderSemaphore, nullptr);
            vkDestroySemaphore(Device, Frame.SwapchainSemaphore, nullptr);

            Frame.DeletionQueue.Flush();
        }

        // Flush the global deletion queue
        MainDeletionQueue.Flush();
        
        DestroySwapChain();
        vkDestroySurfaceKHR(Instance, Surface, nullptr);
        vkDestroyDevice(Device, nullptr);

        vkDestroyInstance(Instance, nullptr);
        vkb::destroy_debug_utils_messenger(Instance, DebugMessenger);
        SDL_DestroyWindow(Window);
    }
}

/** Vulkan */
void Stasis::Engine::InitVulkan()
{
    vkb::InstanceBuilder Builder {};

    // Make the vulkan instance with basic debug features
    auto InstanceResult = Builder.set_app_name("Stasis Engine Application")
        .request_validation_layers(UseValidationLayers)
        .require_api_version(1, 3, 0)
        .build();

    vkb::Instance VkbInstance = InstanceResult.value();

    // Grab the instance
    Instance = VkbInstance.instance;
    DebugMessenger = VkbInstance.debug_messenger;

    SDL_Vulkan_CreateSurface(Window, Instance, nullptr, &Surface);

    // Vulkan 1.3 features
    VkPhysicalDeviceVulkan13Features Vk13Features = { .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES };
    Vk13Features.dynamicRendering = true;
    Vk13Features.synchronization2 = true;
    
    // Vulkan 1.2 features
    VkPhysicalDeviceVulkan12Features Vk12Features = { .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES };
    Vk12Features.bufferDeviceAddress = true;
    Vk12Features.descriptorIndexing = true;

    // Use vkbootstrap to select GPU
    // We want a GPU that can write to the SDL surface and supports Vulkan 1.3 with the correct features
    vkb::PhysicalDeviceSelector Selector {VkbInstance};
    vkb::PhysicalDevice PhysicalDevice = Selector
        .set_minimum_version(1, 3)
        .set_required_features_13(Vk13Features)
        .set_required_features_12(Vk12Features)
        .set_surface(Surface)
        .select()
        .value();

    // Create the final vulkan device
    vkb::DeviceBuilder DeviceBuilder {PhysicalDevice};
    vkb::Device VkbDevice = DeviceBuilder.build().value();

    // Get the VkDevice handle used in the rest of the application
    Device = VkbDevice.device;
    SelectedGPU = PhysicalDevice.physical_device;

    // Use vkbootstrap to get a graphics queue
    GraphicsQueue = VkbDevice.get_queue(vkb::QueueType::graphics).value();
    GraphicsQueueFamily = VkbDevice.get_queue_index(vkb::QueueType::graphics).value();

    // IInitialize the memory allocator
    VmaAllocatorCreateInfo AllocatorInfo {};
    AllocatorInfo.physicalDevice = SelectedGPU;
    AllocatorInfo.device = Device;
    AllocatorInfo.instance = Instance;
    AllocatorInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
    vmaCreateAllocator(&AllocatorInfo, &Allocator);

    MainDeletionQueue.PushFunction([&]()
    {
        vmaDestroyAllocator(Allocator);        
    });
}

void Stasis::Engine::InitSwapChain()
{
    CreateSwapChain(WindowExtent.width, WindowExtent.height);

    // Draw image size will match the window
    const VkExtent3D DrawImageExtent = {
        WindowExtent.width,
        WindowExtent.height,
        1
    };

    // Hardcoding the draw format to 32-bit float
    DrawImage.ImageFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
    DrawImage.ImageExtent = DrawImageExtent;

    VkImageUsageFlags DrawImageUsages {};
    DrawImageUsages |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    DrawImageUsages |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    DrawImageUsages |= VK_IMAGE_USAGE_STORAGE_BIT;
    DrawImageUsages |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    VkImageCreateInfo DrawImageInfo = vkinit::ImageCreateInfo(DrawImage.ImageFormat, DrawImageUsages, DrawImageExtent);

    // For the draw image, we want to allocate it from the gpu local memory
    VmaAllocationCreateInfo DrawImageAllocInfo {};
    DrawImageAllocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    DrawImageAllocInfo.requiredFlags = static_cast<VkMemoryPropertyFlags>(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    // Allocate and create image
    vmaCreateImage(Allocator, &DrawImageInfo, &DrawImageAllocInfo, &DrawImage.Image, &DrawImage.Allocation, nullptr);

    // Build an image-view for the draw image to use for rendering
    VkImageViewCreateInfo DrawImageViewInfo = vkinit::ImageviewCreateInfo(DrawImage.ImageFormat, DrawImage.Image, VK_IMAGE_ASPECT_COLOR_BIT);
    VK_CHECK(vkCreateImageView(Device, &DrawImageViewInfo, nullptr, &DrawImage.ImageView));

    // Add to deletion queues
    MainDeletionQueue.PushFunction([&]()
    {
        vkDestroyImageView(Device, DrawImage.ImageView, nullptr);
        vmaDestroyImage(Allocator, DrawImage.Image, DrawImage.Allocation);
    });
}

void Stasis::Engine::InitCommands()
{
    // Create a command pool for commands submitted to the graphics queue.
    // We also want the pool to allow for resetting of individua; command buffers
    VkCommandPoolCreateInfo CommandPoolInfo = {};
    CommandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    CommandPoolInfo.pNext = nullptr;
    CommandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    CommandPoolInfo.queueFamilyIndex = GraphicsQueueFamily;

    for (auto& Frame : Frames)
    {
        VK_CHECK(vkCreateCommandPool(Device, &CommandPoolInfo, nullptr, &Frame.CommandPool));

        // Allocate the default command buffer that we will use for rendering
        VkCommandBufferAllocateInfo CommandAllocateInfo = vkinit::CommandBufferAllocateInfo(Frame.CommandPool, 1);

        VK_CHECK(vkAllocateCommandBuffers(Device, &CommandAllocateInfo, &Frame.CommandBuffer));
    }
}

void Stasis::Engine::InitSyncStructures()
{
    // Create synchronization structures
    // One fence to control when the GPU has finished rendering the frame
    // and 2 semaphores to synchronize rendering with the swapchain.
    // We want the fence to start signaled so we can wait for it on the first frame
    const VkFenceCreateInfo FenceInfo = vkinit::FenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);
    const VkSemaphoreCreateInfo SemaphoreInfo = vkinit::SemaphoreCreateInfo();

    for (auto& Frame : Frames)
    {
        VK_CHECK(vkCreateFence(Device, &FenceInfo, nullptr, &Frame.RenderFence));

        VK_CHECK(vkCreateSemaphore(Device, &SemaphoreInfo, nullptr, &Frame.SwapchainSemaphore));
        VK_CHECK(vkCreateSemaphore(Device, &SemaphoreInfo, nullptr, &Frame.RenderSemaphore));
    }
}

void Stasis::Engine::CreateSwapChain(const uint32_t Width, const uint32_t Height)
{
    vkb::SwapchainBuilder SwapchainBuilder {SelectedGPU, Device, Surface};
    SwapChainImageFormat = VK_FORMAT_B8G8R8A8_UNORM;
    vkb::Swapchain VkbSwapchain = SwapchainBuilder
        .set_desired_format(VkSurfaceFormatKHR { .format = SwapChainImageFormat, .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR })
        .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
        .set_desired_extent(Width, Height)
        .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
        .build()
        .value();

    SwapChainExtent = VkbSwapchain.extent;
    SwapChain = VkbSwapchain.swapchain;
    SwapChainImages = VkbSwapchain.get_images().value();
    SwapChainImageViews = VkbSwapchain.get_image_views().value();
}

void Stasis::Engine::DestroySwapChain()
{
    vkDestroySwapchainKHR(Device, SwapChain, nullptr);

    // Destroy swapchain resources
    for (const auto SwapChainImageView : SwapChainImageViews)
    {
        vkDestroyImageView(Device, SwapChainImageView, nullptr);
    } 
}
