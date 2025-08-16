#pragma once

#include "Graphics/Camera.hpp"

namespace blackbox::editor
{
    class EditorCamera final : public graphics::Camera
    {
    public:
        float moveSpeed {2.5f};
        float rotationSpeed {100.f};
        bool firstClick {true};
    };
}
