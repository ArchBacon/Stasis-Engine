#include "VulkanRenderer.hpp"
#include <SDL3/SDL_vulkan.h>

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
}

void Stasis::VulkanRenderer::Shutdown()
{
    DestroySwapchain();

    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyDevice(device, nullptr);

    vkb::destroy_debug_utils_messenger(instance, debugMessenger);
    vkDestroyInstance(instance, nullptr);
    SDL_DestroyWindow(window);
}

void Stasis::VulkanRenderer::Draw()
{
    frameNumber++;
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
}

void Stasis::VulkanRenderer::InitSwapchain()
{
    CreateSwapchain(windowExtent.width, windowExtent.height);
}

void Stasis::VulkanRenderer::InitCommands()
{
}

void Stasis::VulkanRenderer::InitSyncStructures()
{
}

void Stasis::VulkanRenderer::CreateSwapchain(uint32_t width, uint32_t height)
{
    vkb::SwapchainBuilder swapchainBuilder {selectedGPU, device, surface};
    swapchainImageFormat = VK_FORMAT_B8G8R8A8_UNORM;

    vkb::Swapchain vkbSwapchain = swapchainBuilder
        .set_desired_format(VkSurfaceFormatKHR{.format = swapchainImageFormat, .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR })
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
