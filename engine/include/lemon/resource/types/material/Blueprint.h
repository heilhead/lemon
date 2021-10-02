#pragma once

#include <lemon/resource/types/material/common.h>
#include <lemon/resource/types/material/BlueprintConfiguration.h>
#include <lemon/shared/Hash.h>
#include <inja/inja.hpp>
#include <inja/environment.hpp>

namespace lemon::res::material {
    class Blueprint {
        friend struct folly::hasher<Blueprint>;

        const inja::Template* tpl;
        inja::Environment* env;

    public:
        Blueprint(const inja::Template* inTpl, inja::Environment* inEnv) : tpl{inTpl}, env{inEnv} {}

        inline std::string
        renderShaderSource(const BlueprintConfiguration& config)
        {
            return env->render(*tpl, config.getDefinitions());
        }
    };
} // namespace lemon::res::material

template<>
struct folly::hasher<lemon::res::material::Blueprint> {
    using folly_is_avalanching = std::true_type;

    size_t
    operator()(const lemon::res::material::Blueprint& data) const
    {
        // TODO: This probably isn't enough and should at least include file path so that includes
        // and block imports would be resolved properly.
        return lemon::hash(data.tpl->content);
    }
};