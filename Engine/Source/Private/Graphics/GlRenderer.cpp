#include "GlRenderer.hpp"

#include "Core/Engine.hpp"
#include "Core/Window.hpp"
#include "glad/glad.h"

namespace blackbox::graphics
{
    GlRenderer::GlRenderer()
    {
    }

    GlRenderer::~GlRenderer() = default;

    void GlRenderer::Render()
    {
        glClearColor(0.3f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ::Engine.Window().SwapBuffers();
    }
}
