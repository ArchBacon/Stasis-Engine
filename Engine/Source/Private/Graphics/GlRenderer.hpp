#pragma once

namespace blackbox::graphics
{
    class GlRenderer
    {
        unsigned vertexShader {};
        unsigned fragmentShader {};
        unsigned shaderProgram {};
        unsigned VBO {}, VAO {};
        
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
