#include "GlRenderer.hpp"

#include "Core/Engine.hpp"
#include "glad/glad.h"
#include "glm/ext/scalar_constants.hpp"

namespace blackbox::graphics
{
    GlRenderer::GlRenderer() = default;
    GlRenderer::~GlRenderer() = default;

    void GlRenderer::Render()
    {
        float t = ::Engine.FrameNumber() % 144 / 144.0f;  // 0.0 to 1.0 over 144 frames
        float PI = glm::pi<float>();
            
        // Simple RGB cycle
        float r = (sin(t * 2.0f * PI) + 1.0f) * 0.5f;
        float g = (sin(t * 2.0f * PI + 2.0f * PI / 3.0f) + 1.0f) * 0.5f;
        float b = (sin(t * 2.0f * PI + 4.0f * PI / 3.0f) + 1.0f) * 0.5f;
    
        glClearColor(r, g, b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }
}
