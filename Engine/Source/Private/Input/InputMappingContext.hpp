#pragma once

#include <type_traits>

#include "InputMapping.hpp"
#include "KeyBinding.hpp"
#include "KeyMapping.hpp"

namespace blackbox
{
    template <typename Derived>
    struct InputMappingContext
    {
        std::unordered_map<InputKey, std::shared_ptr<KeyBinding>, InputKeyHash> keybinds {};

        InputMappingContext(const std::initializer_list<InputMappingBase> mappings)
        {
            for (auto& mapping : mappings)
            {
                for (auto& keyMapping : mapping.keyMappings)
                {
                    keybinds[keyMapping.key] = std::make_shared<KeyBinding>(KeyBinding{
                        .actionType = mapping.actionType,
                        .contextType = std::type_index(typeid(Derived)),
                        .modifiers = keyMapping.modifiers,
                    });
                } 
            } 
        }
    };

    template <typename T>
    concept InputMappingContextType = std::is_base_of_v<InputMappingContext<T>, T> && !std::is_same_v<InputMappingContext<T>, T>;
}
