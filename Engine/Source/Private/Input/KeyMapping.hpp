#pragma once

#include <memory>
#include <vector>

#include "InputKeys.hpp"
#include "InputModifier.hpp"

namespace blackbox
{
    struct KeyMapping
    {
        InputKey key {Keyboard::None};
        std::vector<std::shared_ptr<InputModifier>> modifiers {};

        template <InputModifierType... T>
        KeyMapping(const InputKey key, T&&... mods) : key(key)
        {
            (modifiers.push_back(std::make_shared<std::decay_t<T>>(std::forward<T>(mods))), ...);
        }
    };
}
