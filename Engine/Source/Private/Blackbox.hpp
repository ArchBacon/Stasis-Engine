#pragma once

#include "Core/Logger.hpp"

inline auto LogEngine = std::make_unique<blackbox::Logger>("LogEngine");
