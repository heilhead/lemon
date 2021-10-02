#pragma once

#include <inja/inja.hpp>

namespace lemon::res::material {
    using ComposerDefinitions = inja::json;

    enum class CompositionError {
        kUnknown,
    };
} // namespace lemon::res::material