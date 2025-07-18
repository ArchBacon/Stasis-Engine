// Copyright (c) 2025, Christian Delicaat. All rights reserved.

#pragma once

#include <string>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#include "Core/Types.hpp"

namespace blackbox
{
    class Window
    {
        SDL_Window* window { nullptr};
        
    public:
        Window(uint32_t width, uint32_t height, const std::string& name, const std::string& icon = {});
        ~Window();

        Window(const Window& window) = delete;
        Window &operator=(const Window&) = delete;
        Window(Window&& window) = delete;
        Window& operator=(Window&& other) = delete;

        [[nodiscard]] uint32_t GetWidth() const;
        [[nodiscard]] uint32_t GetHeight() const;
        [[nodiscard]] uint2 GetSize() const;
        
    private:
        [[nodiscard]] std::string GetBuildModeSuffix() const;
    };
}
