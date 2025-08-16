#pragma once

#include <memory>

namespace blackbox::editor
{
    class EditorCamera;
    
    class Editor
    {
        std::unique_ptr<EditorCamera> camera {nullptr};
        
    public:
        Editor();
        ~Editor();

        Editor(const Editor& other) = delete;
        Editor &operator=(const Editor&) = delete;
        Editor(Editor&& other) = delete;
        Editor& operator=(Editor&& other) = delete;

        [[nodiscard]] EditorCamera& Camera() const { return *camera; }

        void Tick(float deltaTime);
    };
}
