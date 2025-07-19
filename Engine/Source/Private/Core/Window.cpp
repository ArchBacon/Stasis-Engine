#include "Core/Window.hpp"

blackbox::Window::Window(
    const uint32_t width,
    const uint32_t height,
    const std::string& name,
    const std::string& icon
) {
    SDL_Init(SDL_INIT_VIDEO);
    
    constexpr SDL_WindowFlags windowFlags = SDL_WINDOW_VULKAN;
    
    window = SDL_CreateWindow(
        (name + GetBuildModeSuffix()).c_str(),
        static_cast<int32_t>(width),
        static_cast<int32_t>(height),
        windowFlags
    );

    if (!icon.empty())
    {
        SDL_Surface* iconSurface = SDL_LoadBMP(icon.c_str());
        SDL_SetWindowIcon(window, iconSurface);
        SDL_DestroySurface(iconSurface);
    }
}

blackbox::Window::~Window()
{
    SDL_DestroyWindow(window);
}

uint32_t blackbox::Window::GetWidth() const
{
    int32_t width {};
    SDL_GetWindowSize(window, &width, nullptr);

    return width;
}

uint32_t blackbox::Window::GetHeight() const
{
    int32_t height {};
    SDL_GetWindowSize(window, nullptr, &height);

    return height;
}

blackbox::uint2 blackbox::Window::GetSize() const
{
    int2 size {};
    SDL_GetWindowSize(window, &size.x, &size.y);

    return size;
}

std::string blackbox::Window::GetBuildModeSuffix() const
{
#ifdef DEBUG
    return " [DEBUG]";
#elif defined DEVELOPMENT
    return " [DEVELOPMENT]";
#else
    return {};
#endif
}
