#pragma once

#include <type_traits>
#include "KeyBinding.hpp"

namespace blackbox
{
    struct InputMappingContext
    {
        std::unordered_map<InputKey, std::shared_ptr<KeyBinding>, InputKeyHash> keybinds {};

        InputMappingContext(const std::vector<InputMapping>& mappings)
        {
            for (auto& mapping : mappings)
            {
                for (auto& keyMapping : mapping.keyMappings)
                {
                    keybinds[keyMapping.key] = std::make_shared<KeyBinding>(KeyBinding{
                        .actionType = mapping.actionType,
                        .contextType = std::type_index(typeid(*this)),
                        .modifiers = keyMapping.modifiers,
                    });
                } 
            } 
        }
    };
    template <typename T>
    concept InputMappingContextType = std::is_base_of_v<InputMappingContext, T> && !std::is_same_v<InputMappingContext, T>;
}
