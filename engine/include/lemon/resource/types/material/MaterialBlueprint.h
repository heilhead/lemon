#pragma once

#include <lemon/resource/types/material/common.h>
#include <lemon/render/material/MaterialConfiguration.h>
#include <inja/inja.hpp>
#include <inja/environment.hpp>

namespace lemon::res::material {
    class MaterialBlueprint {
        friend struct folly::hasher<MaterialBlueprint>;

        using TemplateRef = KeepAlive<tl::expected<inja::Template, CompositionError>>;

        TemplateRef tplRef;
        inja::Environment* env;
        uint64_t hash;

    public:
        MaterialBlueprint(TemplateRef inTplRef, inja::Environment* inEnv) : env{inEnv}
        {
            tplRef = std::move(inTplRef);
            hash = lemon::hash(*this);
        }

        inline std::string
        renderShaderSource(const render::MaterialConfiguration& config) const
        {
            LEMON_ASSERT(tplRef);
            LEMON_ASSERT(tplRef->has_value());
            LEMON_ASSERT(env != nullptr);
            return env->render(tplRef->value(), config.getDefinitions());
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
        if (data.tplRef) {
            // TODO: This probably isn't enough and should at least include file path so that includes
            // and block imports would be resolved properly.
            return lemon::hash(data.tplRef->value().content);
        } else {
            return 0;
        }
    }
};
