#pragma once

#include <optional>
#include <lemon/shared.h>
#include <lemon/shared/filesystem.h>
#include <lemon/shared/AtomicCache.h>
#include <lemon/resource/types/material/common.h>
#include <lemon/resource/types/material/Blueprint.h>
#include <inja/inja.hpp>
#include <inja/environment.hpp>

namespace lemon::res::material {
    class MaterialComposer {
        AtomicCache<uint64_t, tl::expected<inja::Template, CompositionError>> tplCache;
        std::filesystem::path baseDir;
        inja::Environment tplEnv;

    public:
        MaterialComposer(std::filesystem::path& inBaseDir);

        tl::expected<Blueprint, CompositionError>
        getBlueprint(std::filesystem::path& fullPath);

    private:
        inja::Template
        loadTemplate(std::filesystem::path& path);
    };
} // namespace lemon::res::material