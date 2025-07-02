// Copyright (c) 2025, Christian Delicaat. All rights reserved.

#pragma once

#include "Core/Logger.hpp"

inline auto LogEngine = std::make_unique<Stasis::Logger>("LogEngine");
inline auto LogRenderer = std::make_unique<Stasis::Logger>("LogRenderer");
