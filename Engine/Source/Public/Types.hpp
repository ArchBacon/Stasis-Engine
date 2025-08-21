#pragma once

#include <glm/glm.hpp>
#include <concepts>

namespace blackbox
{
    class FileIO;
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

    // Concepts
    template <typename T>
    concept Numeric = std::integral<T> || std::floating_point<T>;

    template <typename T>
    concept Object = requires(T t, float deltaTime)
    {
        { t.BeginPlay() } -> std::same_as<void>;
        { t.Tick(deltaTime) } -> std::same_as<void>;
        { t.Shutdown() } -> std::same_as<void>;
    };
}
