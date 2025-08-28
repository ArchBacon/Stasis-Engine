#include "Window.hpp"
#include "Blackbox.hpp"
#include <glad/glad.h>

#include "EventBus.hpp"

blackbox::Window::Window(
    EventBus& eventbus, 
    const uint32_t width,
    const uint32_t height,
    const std::string& name,
    const std::string& icon
) : eventbus(eventbus) {
    eventbus.Subscribe<WindowResizedEvent>(this, &Window::OnWindowResized);
    
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    
    constexpr SDL_WindowFlags windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
    
    raw = SDL_CreateWindow(
        (name + GetBuildModeSuffix()).c_str(),
        static_cast<int32_t>(width),
        static_cast<int32_t>(height),
        windowFlags
    );

    if (!icon.empty())
    {
        SDL_Surface* iconSurface = SDL_LoadBMP(icon.c_str());
        SDL_SetWindowIcon(raw, iconSurface);
        SDL_DestroySurface(iconSurface);
    }

    if (SDL_GL_CreateContext(raw) == nullptr)
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
    SDL_DestroyWindow(raw);
}

float blackbox::Window::AspectRatio() const
{
    return Width<float>() / Height<float>();
}

void blackbox::Window::SwapBuffers() const
{
    SDL_GL_SwapWindow(raw);
}

void blackbox::Window::EnableVSync(const bool enabled) const
{
    SDL_GL_SetSwapInterval(enabled);
}

void blackbox::Window::OnWindowResized(const WindowResizedEvent event)
{
    LogEngine->Info("Resized window from ({}, {}) -> ({}, {})", Width(), Height(), event.newWindowSize.x, event.newWindowSize.y);
    glViewport(0, 0, static_cast<int32_t>(event.newWindowSize.y), static_cast<int32_t>(event.newWindowSize.y));
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
