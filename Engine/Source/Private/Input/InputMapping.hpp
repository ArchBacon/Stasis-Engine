#pragma once

#include "KeyMapping.hpp"
#include <typeindex>

namespace blackbox
{
    struct InputMappingBase
    {
        std::type_index actionType;
        std::vector<KeyMapping> keyMappings {};

        InputMappingBase() : actionType(typeid(*this)) {}
        virtual ~InputMappingBase() = default;
    };
    
    template <typename T>
    struct InputMapping : InputMappingBase
    {
        InputMapping(const std::initializer_list<KeyMapping> inKeyMappings)
        {
            actionType = std::type_index(typeid(T));
            keyMappings = inKeyMappings;
        }
    };
}
