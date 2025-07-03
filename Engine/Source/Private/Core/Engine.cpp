#include "Engine.hpp"

#include <chrono>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include "Rendering/vk_initializers.h"
#include "Stasis.hpp"

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
    Window = SDL_CreateWindow("Stasis Engine", WindowExtent.x, WindowExtent.y, WindowFlags);

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
    FrameNumber++;
}

void Stasis::Engine::Shutdown()
{
    LogEngine->Trace("Shutting Down Engine...");

    if (IsInitialized)
    {
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
}

void Stasis::Engine::InitSwapChain()
{
    CreateSwapChain(WindowExtent.x, WindowExtent.y);
}

void Stasis::Engine::InitCommands()
{
}

void Stasis::Engine::InitSyncStructures()
{
}

void Stasis::Engine::CreateSwapChain(uint32_t Width, uint32_t Height)
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
    for (auto SwapChainImageView : SwapChainImageViews)
    {
        vkDestroyImageView(Device, SwapChainImageView, nullptr);
    } 
}
