#pragma once

#include <entt/entt.hpp>

namespace blackbox
{
    struct Entity
    {
        entt::entity entity {};
    };
    
    class EntityComponentSystem final
    {
        entt::registry registry {};
        
    public:
        EntityComponentSystem() = default;
        ~EntityComponentSystem() = default;

        EntityComponentSystem(const EntityComponentSystem& other) = delete;
        EntityComponentSystem &operator=(const EntityComponentSystem&) = delete;
        EntityComponentSystem(EntityComponentSystem&& other) = delete;
        EntityComponentSystem& operator=(EntityComponentSystem&& other) = delete;

        entt::registry& Registry();
        template <typename... T>
        Entity Create();
        template <typename... T>
        Entity Add(Entity entity);
    };

    inline entt::registry& EntityComponentSystem::Registry()
    {
        return registry;
    }

    template <typename... T>
    Entity EntityComponentSystem::Create()
    {
        const Entity entity {.entity = registry.create()};
        (registry.emplace<T>(entity.entity), ...); // Fold expression

        return entity;
    }

    template <typename... T>
    Entity EntityComponentSystem::Add(const Entity entity)
    {
        registry.emplace<T...>(entity.entity);

        return entity;
    }
}
