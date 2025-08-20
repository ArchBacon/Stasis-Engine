#pragma once

#include "Core/ECS.hpp"
#include "Core/Types.hpp"

namespace blackbox
{
    class Window;
}

namespace blackbox::editor
{
    struct EditorType {};
    
    class Editor
    {
        Window& window;
        Entity camera {};
        float cameraMoveSpeed {2.5f};
        float cameraRotationSpeed {2.5f};
        bool cameraFirstClick {true};

        // TODO: Global config
        float3 cameraUp {0.0f, 1.0f, 0.0f};
        
    public:
        Editor(Window& window);
        ~Editor();

        Editor(const Editor& other) = delete;
        Editor &operator=(const Editor&) = delete;
        Editor(Editor&& other) = delete;
        Editor& operator=(Editor&& other) = delete;

        void Tick(float deltaTime);
    };
}
