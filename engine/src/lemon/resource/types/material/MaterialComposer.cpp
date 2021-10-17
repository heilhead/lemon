#include <lemon/resource/types/material/MaterialComposer.h>

using namespace lemon::res::material;

MaterialComposer::MaterialComposer(std::filesystem::path& resourceRootDir) : tplEnv{}
{
    baseDir = resourceRootDir / "internal" / "shaders";

    tplEnv.set_line_statement("//#");
    tplEnv.set_search_included_templates_in_files(false);
    tplEnv.set_include_callback([&](auto&, auto& name) {
        auto fullPath = baseDir / name;
        return loadTemplate(fullPath);
    });
}

tl::expected<MaterialBlueprint, CompositionError>
MaterialComposer::getBlueprint(std::filesystem::path& fullPath)
{
    auto hash = folly::hash::fnv64(fullPath.string());
    auto tplRef = cache.get(hash, [&]() {
        // TODO: Actually handle errors in shader composition.
        return new tl::expected<inja::Template, CompositionError>(loadTemplate(fullPath));
    });

    if (tplRef->has_value()) {
        return MaterialBlueprint(std::move(tplRef), &tplEnv);
    } else {
        return tl::make_unexpected(tplRef->error());
    }
}

inja::Template
MaterialComposer::loadTemplate(std::filesystem::path& path)
{
    auto res = lemon::io::readTextFile(path);
    if (res) {
        return tplEnv.parse(*res);
    } else {
        std::string strErr = "Failed to resolve shader include: [" + path.string() +
                             "] error: " + std::to_string((int)res.error());
        logger::err(strErr);
        return tplEnv.parse(strErr);
    }
}
