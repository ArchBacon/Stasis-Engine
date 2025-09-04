#pragma once

#include <utility>

#include "InputAction.hpp"
#include "KeyMapping.hpp"

namespace blackbox
{
    class InputMapping
    {
        friend class InputMappingContext;
        
        std::unique_ptr<InputAction> action {nullptr};
        std::vector<KeyMapping> keyMappings {};
        
    public:
        template <InputActionType T>
        InputMapping(std::vector<KeyMapping> keyMappings);
    };

    template <InputActionType T>
    InputMapping::InputMapping(std::vector<KeyMapping> keyMappings)
        : keyMappings(std::move(keyMappings))
    {
        action = std::make_unique<T>();
    }
}
