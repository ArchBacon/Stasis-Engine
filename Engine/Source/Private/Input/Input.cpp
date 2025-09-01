#include "Input.hpp"

#include <ranges>

#include "EventBus.hpp"

namespace blackbox
{
    Input::Input(EventBus& eventbus)
        : eventbus(eventbus)
    {
        eventbus.Subscribe<KeyPressedEvent>(this, &Input::OnKeyDown);
        eventbus.Subscribe<KeyReleasedEvent>(this, &Input::OnKeyUp);
    }

    /** Event handlers */
    void Input::Update()
    {
        for (auto& context : activeContexts | std::views::values)
        {
            for (auto& action : context.actions | std::views::values)
            {
                std::visit([&](auto&& inputAction)
                {
                    using ActionType = std::decay_t<decltype(inputAction)>;
                    using ValueType = typename ActionType::type;
                    
                    if (inputAction.active)
                    {
                        for (auto& callback : inputAction.onTriggeredCallbacks)
                        {
                            callback(ValueType(true));
                        }
                    }
                }, action);
            } 
        }
    }

    void Input::OnKeyDown(KeyPressedEvent event)
    {
        for (auto& context : activeContexts | std::views::values)
        {
            for (auto& action : context.actions | std::views::values)
            {
                std::visit([&](auto&& inputAction)
                {
                    using ActionType = std::decay_t<decltype(inputAction)>;
                    using ValueType = typename ActionType::type;

                    for (auto key : inputAction.keys)
                    {
                        if (std::get<Keyboard>(key) == event.key)
                        {
                            inputAction.active = true;
                            for (auto& callback : inputAction.onStartedCallbacks)
                            {
                                callback(ValueType(true));
                            }
                        }
                    }
                }, action);
            } 
        }
    }

    void Input::OnKeyUp(const KeyReleasedEvent event)
    {
        for (auto& context : activeContexts | std::views::values)
        {
            for (auto& action : context.actions | std::views::values)
            {
                std::visit([&](auto&& inputAction)
                {
                    using ActionType = std::decay_t<decltype(inputAction)>;
                    using ValueType = typename ActionType::type;

                    for (auto key : inputAction.keys)
                    {
                        if (std::get<Keyboard>(key) == event.key)
                        {
                            inputAction.active = false;
                            for (auto& callback : inputAction.onEndedCallbacks)
                            {
                                callback(ValueType(false));
                            } 
                        }
                    } 
                }, action);
            } 
        }
    }

    /** Context methods */
    InputContext& Input::GetContext(const std::string& name)
    {
        // If context is active, return it
        if (activeContexts.contains(name))
            return activeContexts[name];

        // otherwise return existing or created context
        auto& context = inactiveContexts[name];
        context.name = name;
        return context;
    }
    
    void Input::EnableContext(const std::string& name)
    {
        if (inactiveContexts.contains(name))
        {
            // Move context from inactive to active
            auto context = inactiveContexts.extract(name);
            activeContexts.insert(std::move(context));
        }
    }

    void Input::DisableContext(const std::string& name)
    {
        if (activeContexts.contains(name))
        {
            // Move context from active to inactive
            auto context = activeContexts.extract(name);
            inactiveContexts.insert(std::move(context));
        }
    }

    void Input::DisableAllContexts()
    {
        inactiveContexts.merge(activeContexts);
    }
}
