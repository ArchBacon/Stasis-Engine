#pragma once

#include <cstdint>

namespace blackbox
{
    enum class ActionType : uint8_t
    {
        Digital, // bool
        Axis1D,  // float
        Axis2D,  // float2
    };
}
