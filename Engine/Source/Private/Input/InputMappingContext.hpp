#pragma once

#include <vector>

#include "InputMapping.hpp"

namespace blackbox
{
    class InputMappingContext
    {
        friend class Input;
        
        // std::vector<InputMapping> mappings {};
        std::unordered_map<std::type_index, InputMapping> mappings {};
        
    public:
        InputMappingContext(const std::vector<InputMapping>& inMappings)
        {
            for (auto mapping : inMappings)
            {
                mappings[std::type_index(typeid(mapping.action))] = std::move(mapping);
            } 
        }
    };

    template <typename T>
    concept InputMappingContextType = std::is_base_of_v<InputMappingContext, T> && !std::is_same_v<InputMappingContext, T>;

    // TODO: Example code for API design
    struct SteeringAction : InputAction {};
    struct ThrottleAction : InputAction {};

    class IMC_Driving final : public InputMappingContext
    {
        IMC_Driving() : InputMappingContext({
            InputMapping<SteeringAction> {
                {Keyboard::A, {Swizzle{}, Negate{}}},
                {Keyboard::D, {Swizzle{}}},
                {Controller::Stick::Motion::Left, {Deadzone{.deadzone = 0.2f}}},
            },
            InputMapping<ThrottleAction> {
                {Keyboard::W, {}},
                {Keyboard::S, {Negate{}}},
            },
        }) {}
    };
}
