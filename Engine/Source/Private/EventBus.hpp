#pragma once

#include <functional>
#include <typeindex>
#include <unordered_map>
#include <vector>
#include <SDL3/SDL_events.h>

#include "Events.hpp"

namespace blackbox
{
    class EventBus
    {
        std::unordered_map<std::type_index, std::vector<std::function<void(Event&)>>> subscribers {};
        std::unordered_map<SDL_EventType, std::function<void(SDL_Event&)>> sdlConverters {};

    public:
        template <typename EventType, typename Class, typename ParamType>
        void Subscribe(Class* instance, void (Class::*method)(ParamType));

        template <typename EventType>
        void Broadcast(EventType event);
    };

    template <typename EventType, typename Class, typename ParamType>
    void EventBus::Subscribe(Class* instance, void (Class::*method)(ParamType))
    {
        auto callback = [instance, method](Event& e)
        {
            (instance->*method)(static_cast<const ParamType&>(e));
        };

        subscribers[typeid(EventType)].push_back(callback);
    }

    template <typename EventType>
    void EventBus::Broadcast(EventType event)
    {
        const auto typeIndex = std::type_index(typeid(EventType));
        if (const auto it = subscribers.find(typeIndex); it != subscribers.end())
        {
            for (const auto& callback : it->second)
            {
                callback(event);
            }
        }
    }
}
