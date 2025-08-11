#pragma once
#include "GlShader.hpp"

namespace blackbox::graphics
{
    class GlRenderer
    {
        GlShader shader {};
        unsigned VBO {}, VAO {}, EBO {};
        
    public:
        GlRenderer();
        ~GlRenderer();

        GlRenderer(const GlRenderer& other) = delete;
        GlRenderer &operator=(const GlRenderer&) = delete;
        GlRenderer(GlRenderer&& other) = delete;
        GlRenderer& operator=(GlRenderer&& other) = delete;

        void Render();
    };
}
