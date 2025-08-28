// Copyright (c) 2025, Christian Delicaat. All rights reserved.

#pragma once

#include <string>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#include "Events.hpp"
#include "Types.hpp"

namespace blackbox
{
    class EventBus;
    
    class Window
    {
        SDL_Window* raw {nullptr};
        EventBus& eventbus;
        
    public:
        Window(EventBus& eventbus, uint32_t width, uint32_t height, const std::string& name, const std::string& icon = {});
        ~Window();

        Window(const Window& other) = delete;
        Window &operator=(const Window&) = delete;
        Window(Window&& other) = delete;
        Window& operator=(Window&& other) = delete;

        template <Numeric T = int>
        [[nodiscard]] T Width() const;
        template <Numeric T = int>
        [[nodiscard]] T Height() const;
        [[nodiscard]] float AspectRatio() const;

        void SwapBuffers() const;
        void EnableVSync(bool enabled = true) const;
        
        void OnWindowResized(WindowResizedEvent event);
        
    private:
        [[nodiscard]] std::string GetBuildModeSuffix() const;
    };
    
    template <Numeric T>
    T Window::Width() const
    {
        int32_t width {};
        SDL_GetWindowSize(raw, &width, nullptr);

        return static_cast<T>(width);
    }

    template <Numeric T>
    T Window::Height() const
    {
        int32_t height {};
        SDL_GetWindowSize(raw, nullptr, &height);

        return static_cast<T>(height);
    }
}
