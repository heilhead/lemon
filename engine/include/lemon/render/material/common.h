#pragma once

#include <cstdint>
#include <gsl/gsl>

namespace lemon::render {
    static constexpr size_t kMaterialMaxUniforms = 4;
    static constexpr size_t kMaterialMaxUniformMembers = 12;
    static constexpr gsl::czstring<> kMaterialUniformPropertyDelimiter = ".";
} // namespace lemon::render
