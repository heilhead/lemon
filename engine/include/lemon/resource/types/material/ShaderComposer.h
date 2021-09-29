#pragma once

#include <lemon/shared.h>
#include <lemon/shared/filesystem.h>
#include <lemon/shared/AtomicCache.h>
#include <lemon/utils/utils.h>
#include <inja/inja.hpp>
#include <inja/environment.hpp>

namespace lemon::res::material {
    enum class ShaderCompositionError {
        kUnknown,
    };

    using ComposerDefinitions = inja::json;

    class ShaderBlueprint {
        const inja::Template* tpl;
        inja::Environment* env;

    public:
        ShaderBlueprint(const inja::Template* inTpl, inja::Environment* inEnv) : tpl{inTpl}, env{inEnv} {}

        inline std::string
        getCode(ComposerDefinitions& defs)
        {
            return env->render(*tpl, defs);
        }
    };

    class ShaderComposer {
        AtomicCache<uint64_t, tl::expected<inja::Template, ShaderCompositionError>> tplCache;
        std::filesystem::path baseDir;
        inja::Environment tplEnv;

    public:
        ShaderComposer(std::filesystem::path& inBaseDir);

        tl::expected<ShaderBlueprint, ShaderCompositionError>
        getBlueprint(std::filesystem::path& fullPath);

    private:
        inja::Template
        loadShaderCode(std::filesystem::path& path);
    };
} // namespace lemon::res::material