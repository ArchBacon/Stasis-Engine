#pragma once
#include <optional>
#include <SDL3/SDL_scancode.h>

#include "Input/InputKeys.hpp"

namespace blackbox
{
    constexpr Keyboard MapSDLKeyboard(const SDL_Scancode scancode) noexcept
    {
        switch (scancode)
        {
            case SDL_SCANCODE_A: return Keyboard::A;
            case SDL_SCANCODE_B: return Keyboard::B;
            case SDL_SCANCODE_C: return Keyboard::C;
            case SDL_SCANCODE_D: return Keyboard::D;
            case SDL_SCANCODE_E: return Keyboard::E;
            case SDL_SCANCODE_F: return Keyboard::F;
            case SDL_SCANCODE_G: return Keyboard::G;
            case SDL_SCANCODE_H: return Keyboard::H;
            case SDL_SCANCODE_I: return Keyboard::I;
            case SDL_SCANCODE_J: return Keyboard::J;
            case SDL_SCANCODE_K: return Keyboard::K;
            case SDL_SCANCODE_L: return Keyboard::L;
            case SDL_SCANCODE_M: return Keyboard::M;
            case SDL_SCANCODE_N: return Keyboard::N;
            case SDL_SCANCODE_O: return Keyboard::O;
            case SDL_SCANCODE_P: return Keyboard::P;
            case SDL_SCANCODE_Q: return Keyboard::Q;
            case SDL_SCANCODE_R: return Keyboard::R;
            case SDL_SCANCODE_S: return Keyboard::S;
            case SDL_SCANCODE_T: return Keyboard::T;
            case SDL_SCANCODE_U: return Keyboard::U;
            case SDL_SCANCODE_V: return Keyboard::V;
            case SDL_SCANCODE_W: return Keyboard::W;
            case SDL_SCANCODE_X: return Keyboard::X;
            case SDL_SCANCODE_Y: return Keyboard::Y;
            case SDL_SCANCODE_Z: return Keyboard::Z;
            
            case SDL_SCANCODE_0: return Keyboard::Num0;
            case SDL_SCANCODE_1: return Keyboard::Num1;
            case SDL_SCANCODE_2: return Keyboard::Num2;
            case SDL_SCANCODE_3: return Keyboard::Num3;
            case SDL_SCANCODE_4: return Keyboard::Num4;
            case SDL_SCANCODE_5: return Keyboard::Num5;
            case SDL_SCANCODE_6: return Keyboard::Num6;
            case SDL_SCANCODE_7: return Keyboard::Num7;
            case SDL_SCANCODE_8: return Keyboard::Num8;
            case SDL_SCANCODE_9: return Keyboard::Num9;
            
            case SDL_SCANCODE_F1: return Keyboard::F1;
            case SDL_SCANCODE_F2: return Keyboard::F2;
            case SDL_SCANCODE_F3: return Keyboard::F3;
            case SDL_SCANCODE_F4: return Keyboard::F4;
            case SDL_SCANCODE_F5: return Keyboard::F5;
            case SDL_SCANCODE_F6: return Keyboard::F6;
            case SDL_SCANCODE_F7: return Keyboard::F7;
            case SDL_SCANCODE_F8: return Keyboard::F8;
            case SDL_SCANCODE_F9: return Keyboard::F9;
            case SDL_SCANCODE_F10: return Keyboard::F10;
            case SDL_SCANCODE_F11: return Keyboard::F11;
            case SDL_SCANCODE_F12: return Keyboard::F12;
            
            case SDL_SCANCODE_LEFT: return Keyboard::Left;
            case SDL_SCANCODE_RIGHT: return Keyboard::Right;
            case SDL_SCANCODE_UP: return Keyboard::Up;
            case SDL_SCANCODE_DOWN: return Keyboard::Down;
            
            case SDL_SCANCODE_SPACE: return Keyboard::Space;
            case SDL_SCANCODE_RETURN: return Keyboard::Enter;
            case SDL_SCANCODE_TAB: return Keyboard::Tab;
            case SDL_SCANCODE_BACKSPACE: return Keyboard::Backspace;
            case SDL_SCANCODE_DELETE: return Keyboard::Delete;
            case SDL_SCANCODE_ESCAPE: return Keyboard::Escape;
            
            case SDL_SCANCODE_LSHIFT: return Keyboard::LeftShift;
            case SDL_SCANCODE_RSHIFT: return Keyboard::RightShift;
            case SDL_SCANCODE_LCTRL: return Keyboard::LeftCtrl;
            case SDL_SCANCODE_RCTRL: return Keyboard::RightCtrl;
            case SDL_SCANCODE_LALT: return Keyboard::LeftAlt;
            case SDL_SCANCODE_RALT: return Keyboard::RightAlt;
            case SDL_SCANCODE_LGUI: return Keyboard::LeftSuper;
            case SDL_SCANCODE_RGUI: return Keyboard::RightSuper;
            
            case SDL_SCANCODE_CAPSLOCK: return Keyboard::CapsLock;
            case SDL_SCANCODE_NUMLOCKCLEAR: return Keyboard::NumLock;
            case SDL_SCANCODE_SCROLLLOCK: return Keyboard::ScrollLock;
            
            case SDL_SCANCODE_HOME: return Keyboard::Home;
            case SDL_SCANCODE_END: return Keyboard::End;
            case SDL_SCANCODE_PAGEUP: return Keyboard::PageUp;
            case SDL_SCANCODE_PAGEDOWN: return Keyboard::PageDown;
            case SDL_SCANCODE_INSERT: return Keyboard::Insert;
            
            case SDL_SCANCODE_SEMICOLON: return Keyboard::Semicolon;
            case SDL_SCANCODE_EQUALS: return Keyboard::Equals;
            case SDL_SCANCODE_COMMA: return Keyboard::Comma;
            case SDL_SCANCODE_MINUS: return Keyboard::Minus;
            case SDL_SCANCODE_PERIOD: return Keyboard::Period;
            case SDL_SCANCODE_SLASH: return Keyboard::Slash;
            
            case SDL_SCANCODE_GRAVE: return Keyboard::Grave;
            case SDL_SCANCODE_LEFTBRACKET: return Keyboard::LeftBracket;
            case SDL_SCANCODE_BACKSLASH: return Keyboard::Backslash;
            case SDL_SCANCODE_RIGHTBRACKET: return Keyboard::RightBracket;
            case SDL_SCANCODE_APOSTROPHE: return Keyboard::Apostrophe;
            
            case SDL_SCANCODE_PRINTSCREEN: return Keyboard::PrintScreen;
            case SDL_SCANCODE_PAUSE: return Keyboard::Pause;
            case SDL_SCANCODE_MENU: return Keyboard::Menu;
            
            default: return Keyboard::None;
        }
    }
}
