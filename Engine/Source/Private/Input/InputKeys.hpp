#pragma once

#include <variant>

namespace blackbox
{
    enum class Keyboard : uint8_t
    {
        Any, None,
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

    inline const char* to_string(Keyboard e)
    {
        switch (e)
        {
        case Keyboard::Any: return "Any";
        case Keyboard::None: return "None";
        case Keyboard::A: return "A";
        case Keyboard::B: return "B";
        case Keyboard::C: return "C";
        case Keyboard::D: return "D";
        case Keyboard::E: return "E";
        case Keyboard::F: return "F";
        case Keyboard::G: return "G";
        case Keyboard::H: return "H";
        case Keyboard::I: return "I";
        case Keyboard::J: return "J";
        case Keyboard::K: return "K";
        case Keyboard::L: return "L";
        case Keyboard::M: return "M";
        case Keyboard::N: return "N";
        case Keyboard::O: return "O";
        case Keyboard::P: return "P";
        case Keyboard::Q: return "Q";
        case Keyboard::R: return "R";
        case Keyboard::S: return "S";
        case Keyboard::T: return "T";
        case Keyboard::U: return "U";
        case Keyboard::V: return "V";
        case Keyboard::W: return "W";
        case Keyboard::X: return "X";
        case Keyboard::Y: return "Y";
        case Keyboard::Z: return "Z";
        case Keyboard::Num0: return "Num0";
        case Keyboard::Num1: return "Num1";
        case Keyboard::Num2: return "Num2";
        case Keyboard::Num3: return "Num3";
        case Keyboard::Num4: return "Num4";
        case Keyboard::Num5: return "Num5";
        case Keyboard::Num6: return "Num6";
        case Keyboard::Num7: return "Num7";
        case Keyboard::Num8: return "Num8";
        case Keyboard::Num9: return "Num9";
        case Keyboard::F1: return "F1";
        case Keyboard::F2: return "F2";
        case Keyboard::F3: return "F3";
        case Keyboard::F4: return "F4";
        case Keyboard::F5: return "F5";
        case Keyboard::F6: return "F6";
        case Keyboard::F7: return "F7";
        case Keyboard::F8: return "F8";
        case Keyboard::F9: return "F9";
        case Keyboard::F10: return "F10";
        case Keyboard::F11: return "F11";
        case Keyboard::F12: return "F12";
        case Keyboard::Left: return "Left";
        case Keyboard::Right: return "Right";
        case Keyboard::Up: return "Up";
        case Keyboard::Down: return "Down";
        case Keyboard::Space: return "Space";
        case Keyboard::Enter: return "Enter";
        case Keyboard::Tab: return "Tab";
        case Keyboard::Backspace: return "Backspace";
        case Keyboard::Delete: return "Delete";
        case Keyboard::Escape: return "Escape";
        case Keyboard::LeftShift: return "LeftShift";
        case Keyboard::RightShift: return "RightShift";
        case Keyboard::LeftCtrl: return "LeftCtrl";
        case Keyboard::RightCtrl: return "RightCtrl";
        case Keyboard::LeftAlt: return "LeftAlt";
        case Keyboard::RightAlt: return "RightAlt";
        case Keyboard::LeftSuper: return "LeftSuper";
        case Keyboard::RightSuper: return "RightSuper";
        case Keyboard::CapsLock: return "CapsLock";
        case Keyboard::NumLock: return "NumLock";
        case Keyboard::ScrollLock: return "ScrollLock";
        case Keyboard::Home: return "Home";
        case Keyboard::End: return "End";
        case Keyboard::PageUp: return "PageUp";
        case Keyboard::PageDown: return "PageDown";
        case Keyboard::Insert: return "Insert";
        case Keyboard::Semicolon: return "Semicolon";
        case Keyboard::Equals: return "Equals";
        case Keyboard::Comma: return "Comma";
        case Keyboard::Minus: return "Minus";
        case Keyboard::Period: return "Period";
        case Keyboard::Slash: return "Slash";
        case Keyboard::Grave: return "Grave";
        case Keyboard::LeftBracket: return "LeftBracket";
        case Keyboard::Backslash: return "Backslash";
        case Keyboard::RightBracket: return "RightBracket";
        case Keyboard::Apostrophe: return "Apostrophe";
        case Keyboard::PrintScreen: return "PrintScreen";
        case Keyboard::Pause: return "Pause";
        case Keyboard::Menu: return "Menu";
        default: return "unknown";
        }
    }

    namespace Mouse
    {
        enum class Button : uint8_t { Left, Right, Middle, Wheel, X1, X2 };
        struct Motion {};
        struct Wheel {};
        
    }

    namespace Controller
    {
        enum class FaceButton : uint8_t { Down, Right, Left, Up }; // A, B, X, Y on Xbox
        enum class Shoulder : uint8_t { Left, Right };
        enum class Trigger : uint8_t { Left, Right };
        namespace Stick
        {
            enum class Motion : uint8_t { Left, Right };
            enum class Pressed : uint8_t { Left, Right };
        }
        enum class DPad : uint8_t { Up, Down, Left, Right };
        enum class Special : uint8_t { Left, Right };
    }

    using InputKey = std::variant<
        Keyboard,
        Mouse::Button,
        Mouse::Motion,
        Mouse::Wheel,
        Controller::FaceButton,
        Controller::Shoulder,
        Controller::Trigger,
        Controller::Stick::Motion,
        Controller::Stick::Pressed,
        Controller::DPad,
        Controller::Special
    >;
}
