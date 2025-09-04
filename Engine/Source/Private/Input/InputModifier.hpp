#pragma once

namespace blackbox
{
    // TODO: Make into typed interface
    struct InputModifier
    {
        virtual ~InputModifier() = default;
        virtual float2 Execute(float2 value) = 0;
    };
    template <typename T>
    concept InputModifierType = std::is_base_of_v<InputModifier, T> && !std::is_same_v<InputModifier, T>;

    struct Negate : InputModifier {
        float2 Execute(const float2 value) override
        {
            return -value;
        }
    };

    struct Swizzle : InputModifier
    {
        float2 Execute(float2 value) override
        {
            return {value.y, value.x};
        }
    };
    
    struct Deadzone : InputModifier
    {
        float deadzone {0.2f};

        Deadzone(const float deadzone) : deadzone(deadzone) {}
        
        float2 Execute(const float2 value) override
        {
            return value * static_cast<float>(glm::length(value) > deadzone);
        }
    };
}
