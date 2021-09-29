#include <lemon/resource/types/material/ShaderComposer.h>
#include <folly/Hash.h>

using namespace lemon::res::material;

ShaderComposer::ShaderComposer(std::filesystem::path& resourceRootDir) : tplEnv{}
{
    baseDir = resourceRootDir / "internal" / "shaders";

    tplEnv.set_line_statement("//#");
    tplEnv.set_search_included_templates_in_files(false);
    tplEnv.set_include_callback([&](auto&, auto& name) {
        auto fullPath = baseDir / name;
        return loadShaderCode(fullPath);
    });
}

tl::expected<ShaderBlueprint, ShaderCompositionError>
ShaderComposer::getBlueprint(std::filesystem::path& fullPath)
{
    auto hash = folly::hash::fnv64(fullPath.string());
    auto [result, bInserted] = tplCache.findOrInsert(hash, [&]() {
        // TODO: Actually handle errors in shader composition.
        return new tl::expected<inja::Template, ShaderCompositionError>(loadShaderCode(fullPath));
    });

    if (result->has_value()) {
        auto& val = result->value();
        return ShaderBlueprint(&val, &tplEnv);
    } else {
        return tl::make_unexpected(result->error());
    }
}

inja::Template
ShaderComposer::loadShaderCode(std::filesystem::path& path)
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