#include <lemon/resource/types/material/MaterialComposer.h>

using namespace lemon::res::material;

MaterialComposer::MaterialComposer(const std::filesystem::path& resourceRootDir) : env{resourceRootDir} {}

tl::expected<MaterialBlueprint, CompositionError>
MaterialComposer::getBlueprint(const std::filesystem::path& fullPath)
{
    auto hash = folly::hash::fnv64(fullPath.string());
    auto tplRef = cache.get(hash, [&]() {
        // TODO: Actually handle errors in shader composition.
        return new tl::expected<inja::Template, CompositionError>(env.loadTemplate(fullPath));
    });

    if (tplRef->has_value()) {
        return MaterialBlueprint(std::move(tplRef), &env);
    } else {
        return tl::make_unexpected(tplRef->error());
    }
}
