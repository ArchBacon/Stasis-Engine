#pragma once

namespace blackbox::graphics
{
    class GlRenderer
    {
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
