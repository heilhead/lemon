#pragma once

#include <lemon/render/material/MaterialConfiguration.h>
#include <lemon/resource/types/material/common.h>
#include <lemon/resource/types/material/MaterialComposerEnvironment.h>
#include <lemon/pch/inja.h>

namespace lemon::res::material {
    class MaterialBlueprint {
        friend struct folly::hasher<MaterialBlueprint>;

        using TemplateRef = KeepAlive<tl::expected<inja::Template, CompositionError>>;

        TemplateRef tplRef;
        MaterialComposerEnvironment* env;
        uint64_t hash;

    public:
        MaterialBlueprint(TemplateRef inTplRef, MaterialComposerEnvironment* inEnv);

        std::string
        renderShaderSource(const render::MaterialConfiguration& config) const;

        uint64_t
        getHash() const;
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
