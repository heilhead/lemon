#pragma once

#include <lemon/resource/types/material/common.h>
#include <lemon/resource/types/material/MaterialConfiguration.h>
#include <lemon/shared/Hash.h>
#include <lemon/shared/assert.h>
#include <inja/inja.hpp>
#include <inja/environment.hpp>

namespace lemon::res::material {
    class MaterialBlueprint {
        friend struct folly::hasher<MaterialBlueprint>;

        const inja::Template* tpl;
        inja::Environment* env;
        uint64_t hash;

    public:
        MaterialBlueprint(const inja::Template* inTpl, inja::Environment* inEnv) : tpl{inTpl}, env{inEnv}
        {
            hash = lemon::hash(*this);
        }

        inline std::string
        renderShaderSource(const MaterialConfiguration& config)
        {
            LEMON_ASSERT(tpl != nullptr);
            LEMON_ASSERT(env != nullptr);
            return env->render(*tpl, config.getDefinitions());
        }

        inline uint64_t
        getHash() const
        {
            return hash;
        }
    };
} // namespace lemon::res::material

template<>
struct folly::hasher<lemon::res::material::MaterialBlueprint> {
    using folly_is_avalanching = std::true_type;

    size_t
    operator()(const lemon::res::material::MaterialBlueprint& data) const
    {
        // TODO: This probably isn't enough and should at least include file path so that includes
        // and block imports would be resolved properly.
        return lemon::hash(data.tpl->content);
    }
};