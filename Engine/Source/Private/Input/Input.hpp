#pragma once
#include <memory>
#include <ranges>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "Blackbox.hpp"
#include "InputAction.hpp"
#include "InputMappingContext.hpp"

namespace blackbox
{
    struct TickEvent;
    struct KeyReleasedEvent;
    struct KeyPressedEvent;
    class EventBus;

    class Input
    {
        EventBus& eventbus;

        std::unordered_map<std::type_index, std::unique_ptr<InputMappingContext>> contexts {};
        std::unordered_map<std::type_index, std::unique_ptr<InputAction>> actions {};
            
    public:
        Input(EventBus& eventbus);
        ~Input() = default;

        Input(const Input& other) = delete;
        Input& operator=(const Input&) = delete;
        Input(Input&& other) = delete;
        Input& operator=(Input&& other) = delete;

        template <InputMappingContextType T>
        void AddContext();
        
        template <InputMappingContextType T>
        void RemoveContext();

        void Clear();

        template <InputActionType T>
        [[nodiscard]] T& GetAction();

    private:
        void OnKeyPressedEvent(KeyPressedEvent event);
        void OnKeyReleasedEvent(KeyReleasedEvent event);
        void OnTickEvent(TickEvent event);
    };

    template <InputMappingContextType T>
    void Input::AddContext()
    {
        const auto type = std::type_index(typeid(T));
        if (contexts.contains(type))
        {
            LogEngine->Warn("Context `{}` already added on input system.", type.name());
            return;
        }

        contexts[type] = std::make_unique<T>();
        auto& context = contexts[type];
        for (auto& mapping : context->mappings)
        {
            mapping.second
        } 
    }

    template <InputMappingContextType T>
    void Input::RemoveContext()
    {
        const auto type = std::type_index(typeid(T));
        if (!contexts.contains(type))
        {
            LogEngine->Warn("Context `{}` does not exist on input system.", type.name());
        }
        
        contexts.erase(type);
    }

    template <InputActionType T>
    T& Input::GetAction()
    {
        const auto type = std::type_index(typeid(T));
        if (!contexts.contains(type))
        {
            LogEngine->Warn("Action `{}` does not exist on any active context.", type.name());
        }

        return contexts[type];
    }
}
