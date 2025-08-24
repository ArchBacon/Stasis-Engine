#pragma once

#include <any>
#include <concepts>
#include <memory>
#include <typeindex>
#include <unordered_map>

#include "Blackbox.hpp"

namespace blackbox
{
    class Container
    {
        std::unordered_map<std::type_index, std::any> instances {};

    public:
        Container() = default;
        ~Container() = default;

        Container(const Container& other) = delete;
        Container& operator=(const Container&) = delete;
        Container(Container&& other) = delete;
        Container& operator=(Container&& other) = delete;

        template <typename T, typename... Deps, typename... Args>
        T* Register(Args&&... args);

    private:
        template <typename... T>
        [[nodiscard]] std::tuple<T&...> Get();
    };

    template <typename T, typename... Dependencies, typename... Args>
    T* Container::Register(Args&&... args)
    {
        static_assert(std::constructible_from<T, Dependencies..., Args...>, "T must be constructible with Dependencies and Args");

        auto dependencies = Get<std::remove_reference_t<Dependencies>...>();
        auto instance = std::apply([&args...](auto&... deps)
        {
            return std::make_shared<T>(deps..., std::forward<Args>(args)...);
        }, dependencies);

        T* raw = instance.get();
        instances[std::type_index(typeid(T))] = std::move(instance);
        return raw;
    }

    template <typename... T>
    std::tuple<T&...> Container::Get()
    {
    #ifndef SHIPPING
        auto validate = [&](auto typeIndex)
        {
            if (!instances.contains(typeIndex))
            {
                LogEngine->Error("Missing instance for type \"{}\"",  typeIndex.name());
            }
        };

        (validate(std::type_index(typeid(T))), ...);
    #endif
        
        return std::tuple<T&...>
        {
            *std::any_cast<std::shared_ptr<std::remove_reference_t<T>>>(instances.at(std::type_index(typeid(T))))...
        };
    }
}
