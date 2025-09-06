#pragma once

#include "KeyMapping.hpp"
#include <typeindex>

namespace blackbox
{
    struct InputMapping
    {
        std::type_index actionType;
        std::vector<KeyMapping> keyMappings {};

        template <typename T>
        InputMapping(const std::initializer_list<KeyMapping>& inKeyMappings)
            : actionType(std::type_index(typeid(T)))
        {
            keyMappings = inKeyMappings;
        }
    };
}
