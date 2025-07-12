#pragma once

#include <glm/glm.hpp>
#include "glm/ext/matrix_transform.hpp"

namespace blackbox
{
    // integers
    using int2 = glm::ivec2;
    using int3 = glm::ivec3;
    using int4 = glm::ivec4;

    // unsigned integers
    using uint2 = glm::uvec2;
    using uint3 = glm::uvec3;
    using uint4 = glm::uvec4;

    // floats
    using float2 = glm::vec2;
    using float3 = glm::vec3;
    using float4 = glm::vec4;

    // doubles
    using double2 = glm::dvec2;
    using double3 = glm::dvec3;
    using double4 = glm::dvec4;

    // include the rest of glm
    using namespace glm;
}
