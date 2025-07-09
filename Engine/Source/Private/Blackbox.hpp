#pragma once

#include "Core/Logger.hpp"

inline auto LogEngine = std::make_unique<Blackbox::Logger>("LogEngine");
inline auto LogRenderer = std::make_unique<Blackbox::Logger>("LogRenderer");
