#pragma once

#include "GlShader.hpp"

namespace blackbox::graphics
{
    enum class RenderMode
    {
        Default,
        Wireframe,
    };
    
    class GlRenderer
    {
        GlShader* shader;
        unsigned VBO {}, VAO {}, EBO {};
        unsigned texture {}, texture2 {};
        FileIO& fileIO;
        
    public:
        GlRenderer(FileIO& fileIO);
        ~GlRenderer();

        GlRenderer(const GlRenderer& other) = delete;
        GlRenderer &operator=(const GlRenderer&) = delete;
        GlRenderer(GlRenderer&& other) = delete;
        GlRenderer& operator=(GlRenderer&& other) = delete;

        void Render();

        void SetRenderMode(RenderMode mode) const;
    };
}
