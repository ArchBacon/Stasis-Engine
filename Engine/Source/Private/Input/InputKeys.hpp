#pragma once

#include <typeindex>

namespace blackbox
{
    enum class Keyboard : uint8_t
    {
        None, Any,
        A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
        Num0, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9,
        F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
        Left, Right, Up, Down,
        Space, Enter, Tab, Backspace, Delete, Escape,
        LeftShift, RightShift, LeftCtrl, RightCtrl, LeftAlt, RightAlt,
        LeftSuper, RightSuper, CapsLock, NumLock, ScrollLock,
        Home, End, PageUp, PageDown, Insert,
        Semicolon, Equals, Comma, Minus, Period, Slash,
        Grave, LeftBracket, Backslash, RightBracket, Apostrophe,
        PrintScreen, Pause, Menu,
    };

    namespace Mouse
    {
        enum class Button : uint8_t { Left, Right, Middle, Wheel, X1, X2 };
        struct Motion {};
        struct Wheel {};
    }

    namespace Controller
    {
        enum class FaceButton : uint8_t { Down, Right, Left, Up };
        enum class Shoulder : uint8_t { Left, Right };
        enum class Trigger : uint8_t { Left, Right };
        enum class DPad : uint8_t { Up, Down, Left, Right };
        enum class Special : uint8_t { Left, Right };
        namespace Stick
        {
            enum class Motion : uint8_t { Left, Right };
            enum class Pressed : uint8_t { Left, Right };
        }
    }

    struct InputKey
    {
        std::type_index type;
        uint32_t value;

        template <typename EnumType>
        InputKey(EnumType enumValue)
            : type(std::type_index(typeid(EnumType)))
            , value(static_cast<uint32_t>(enumValue))
        {}

        bool operator==(const InputKey&) const = default;
    };

    struct InputKeyHash
    {
        size_t operator()(const InputKey& key) const
        {
            return key.type.hash_code() ^ (static_cast<size_t>(key.value) << 1);
        }
    };
}
