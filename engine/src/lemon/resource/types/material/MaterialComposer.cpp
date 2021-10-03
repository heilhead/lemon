#include <lemon/resource/types/material/MaterialComposer.h>
#include <lemon/shared/Hash.h>
#include <lemon/utils/utils.h>

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
    auto [result, bInserted] = tplCache.findOrInsert(hash, [&]() {
        // TODO: Actually handle errors in shader composition.
        return new tl::expected<inja::Template, CompositionError>(loadTemplate(fullPath));
    });

    if (result->has_value()) {
        auto& val = result->value();
        return MaterialBlueprint(&val, &tplEnv);
    } else {
        return tl::make_unexpected(result->error());
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
        lemon::utils::logErr(strErr);
        return tplEnv.parse(strErr);
    }
}