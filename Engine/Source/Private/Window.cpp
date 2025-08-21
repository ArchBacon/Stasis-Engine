﻿#include "Window.hpp"
#include "Blackbox.hpp"
#include <glad/glad.h>

blackbox::Window::Window(
    const uint32_t width,
    const uint32_t height,
    const std::string& name,
    const std::string& icon
) {
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    
    constexpr SDL_WindowFlags windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
    
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

    if (SDL_GL_CreateContext(window) == nullptr)
    {
        LogEngine->Error("Failed to create openGL context. {}", SDL_GetError());
    }

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
    {
        LogEngine->Error("Failed to initialize GLAD");
        return;
    }

    LogEngine->Info("OpenGL Version: {}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));
    LogEngine->Info("OpenGL Renderer: {}", reinterpret_cast<const char*>(glGetString(GL_RENDERER)));

    glViewport(0, 0, Width(), Height());

    EnableVSync(true);
}

blackbox::Window::~Window()
{
    SDL_DestroyWindow(window);
}

float blackbox::Window::AspectRatio() const
{
    return Width<float>() / Height<float>();
}

void blackbox::Window::SwapBuffers() const
{
    SDL_GL_SwapWindow(window);
}

void blackbox::Window::EnableVSync(const bool enabled) const
{
    SDL_GL_SetSwapInterval(enabled);
}

void blackbox::Window::OnWindowResized(const uint32_t width, const uint32_t height) const
{
    glViewport(0, 0, static_cast<int32_t>(width), static_cast<int32_t>(height));
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
