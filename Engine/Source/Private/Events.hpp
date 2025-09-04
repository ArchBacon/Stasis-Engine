#pragma once

#include "Types.hpp"
#include "Input/InputKeys.hpp"

namespace blackbox
{
    struct Event {};
    
    // Engine Events
    struct ShutdownEvent : Event {};
    struct TickEvent : Event { float deltaTime {0.0f}; };
    
    // Keyboard Input Events
    struct KeyPressedEvent : Event { Keyboard key {}; };
    struct KeyRepeatEvent : Event { Keyboard key {}; };
    struct KeyReleasedEvent : Event { Keyboard key {}; };
    
    // Mouse Input Events
    struct MouseButtonPressedEvent : Event { Mouse::Button button {}; };
    struct MouseButtonReleasedEvent : Event { Mouse::Button button {}; };
    struct MouseMotionEvent : Event { float2 xy {}; };
    struct MouseWheelEvent : Event { float y {}; };
    
    // Controller Input Events
    struct FaceButtonPressedEvent : Event { Controller::FaceButton button {}; };
    struct FaceButtonReleasedEvent : Event { Controller::FaceButton button {}; };
    struct ShoulderPressedEvent : Event { Controller::Shoulder shoulder {}; };
    struct ShoulderReleasedEvent : Event { Controller::Shoulder shoulder {}; };
    struct TriggerEvent : Event { Controller::Trigger trigger {}; float value {}; };
    struct DPadPressedEvent : Event { Controller::DPad button {}; };
    struct DPadReleasedEvent : Event { Controller::DPad button {}; };
    struct SpecialPressedEvent : Event { Controller::Special button {}; };
    struct SpecialReleasedEvent : Event { Controller::Special button {}; };
    struct StickMotionEvent : Event { Controller::Stick::Motion xy {}; };
    struct StickPressedEvent : Event { Controller::Stick::Pressed stick {}; };
    struct StickReleasedEvent : Event { Controller::Stick::Pressed stick {}; };
    
    // Window Events
    struct WindowMinimizedEvent : Event {};
    struct WindowRestoredEvent : Event {};
    struct WindowResizedEvent : Event { float2 windowSize {}; };
    struct WindowFocusLostEvent : Event {};
    struct WindowFocusGainedEvent : Event {};
}
