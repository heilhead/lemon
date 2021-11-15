#pragma once

#include <lemon/resource/types/material/common.h>
#include <lemon/resource/types/material/MaterialBlueprint.h>
#include <lemon/resource/types/material/MaterialComposerEnvironment.h>
#include <lemon/pch/inja.h>

namespace lemon::res::material {
    class MaterialComposer {
        AtomicCache<tl::expected<inja::Template, CompositionError>> cache;
        MaterialComposerEnvironment env;

    public:
        MaterialComposer(const std::filesystem::path& inBaseDir);

        tl::expected<MaterialBlueprint, CompositionError>
        getBlueprint(const std::filesystem::path& fullPath);
    };
} // namespace lemon::res::material
