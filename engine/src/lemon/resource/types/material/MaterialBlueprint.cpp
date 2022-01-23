#include <lemon/resource/types/material/MaterialBlueprint.h>

using namespace lemon;
using namespace lemon::res;
using namespace lemon::res::material;

MaterialBlueprint::MaterialBlueprint(TemplateRef inTplRef, MaterialComposerEnvironment* inEnv) : env{inEnv}
{
    tplRef = std::move(inTplRef);
    hash = lemon::hash(*this);
}

std::string
MaterialBlueprint::renderShaderSource(const render::MaterialConfiguration& config) const
{
    OPTICK_EVENT();

    LEMON_ASSERT(tplRef);
    LEMON_ASSERT(tplRef->has_value());
    LEMON_ASSERT(env != nullptr);
    return env->renderShaderSource(tplRef->value(), config.getDefinitions());
}

uint64_t
MaterialBlueprint::getHash() const
{
    return hash;
}
