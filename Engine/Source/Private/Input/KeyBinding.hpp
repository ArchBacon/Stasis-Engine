#pragma once

#include <memory>
#include <typeindex>
#include <vector>

namespace blackbox
{
    struct InputModifier;
    
    struct KeyBinding
    {
        std::type_index actionType;
        std::type_index contextType;
        std::vector<std::shared_ptr<InputModifier>> modifiers {};
    };
}
