#pragma once

#include <lemon/resource/types/material/common.h>
#include <lemon/resource/types/material/MaterialBlueprint.h>
#include <inja/inja.hpp>
#include <inja/environment.hpp>

namespace lemon::res::material {
    class MaterialComposer {
        AtomicCache<tl::expected<inja::Template, CompositionError>> cache;
        std::filesystem::path baseDir;
        inja::Environment tplEnv;

    public:
        MaterialComposer(std::filesystem::path& inBaseDir);

        tl::expected<MaterialBlueprint, CompositionError>
        getBlueprint(std::filesystem::path& fullPath);

    private:
        inja::Template
        loadTemplate(std::filesystem::path& path);
    };
} // namespace lemon::res::material
