#pragma once

#include "Types.hpp"

namespace blackbox
{
    class InputValue
    {
        friend struct InputModifier;
        
        float2 value {0.0f, 0.0f};

    public:
        template <typename T>
        T Get();
    };

    template <typename T>
    T InputValue::Get()
    {
        static_assert(std::is_same_v<T, bool> || std::is_same_v<T, float> || std::is_same_v<T, float2>, "InputValue::Get only supports bool, float, and float2");

        if constexpr (std::is_same_v<T, bool>)
        {
            return value.x > 0.0f; // true if non-zero or positive
        }
        else if constexpr (std::is_same_v<T, float>)
        {
            return value.x;
        }
        else if constexpr (std::is_same_v<T, float2>)
        {
            return value;
        }
    }
}
