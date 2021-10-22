#pragma once

namespace lemon::game {
    constexpr auto kVectorUp = glm::f32vec3(0.f, 1.f, 0.f);
    constexpr auto kVectorDown = glm::f32vec3(0.f, -1.f, 0.f);

    constexpr auto kVectorForward = glm::f32vec3(0.f, 0.f, 1.f);
    constexpr auto kVectorBackward = glm::f32vec3(0.f, 0.f, -1.f);

    constexpr auto kVectorLeft = glm::f32vec3(1.f, 0.f, 0.f);
    constexpr auto kVectorRight = glm::f32vec3(-1.f, 0.f, 0.f);

    constexpr auto kVectorXAxis = glm::f32vec3(1.f, 0.f, 0.f);
    constexpr auto kVectorYAxis = glm::f32vec3(0.f, 1.f, 0.f);
    constexpr auto kVectorZAxis = glm::f32vec3(0.f, 0.f, 1.f);

    constexpr auto kVectorOne = glm::f32vec3(1.f, 1.f, 1.f);
    constexpr auto kVectorZero = glm::f32vec3(0.f, 0.f, 0.f);

    constexpr auto kMatrixIdentity = glm::identity<glm::f32mat4>();
    constexpr auto kQuatIdentity = glm::identity<glm::quat>();
    const auto kQuatUEOrientation = glm::quatLookAt(kVectorDown, kVectorBackward);

    enum class Axis { X, Y, Z };
} // namespace lemon::game
