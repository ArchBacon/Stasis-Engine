#pragma once

#include <vector>

#include "InputKeys.hpp"
#include "InputModifier.hpp"

namespace blackbox
{
    class KeyMapping
    {
        InputKey key {Keyboard::None};
        std::vector<InputModifier> modifiers {};

    public:
        KeyMapping(const InputKey key, std::vector<InputModifier> modifiers)
            : key(key), modifiers(std::move(modifiers))
        {}
    };
}
