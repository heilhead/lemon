#pragma once

#include <cstdint>

namespace lemon::render {
    static constexpr uint8_t kUserBindGroupIndex = 1;
    static constexpr size_t kMaterialMaxUniforms = 4;
    static constexpr size_t kMaterialMaxUniformMembers = 12;
    static constexpr const char* kMaterialUniformPropertyDelimiter = ".";
} // namespace lemon::render