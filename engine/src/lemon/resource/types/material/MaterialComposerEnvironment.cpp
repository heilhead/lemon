#include <lemon/resource/ResourceManager.h>
#include <lemon/resource/types/material/MaterialComposerEnvironment.h>

using namespace lemon;
using namespace lemon::res;
using namespace lemon::res::material;

struct SourceRenderContext {
    const inja::json* definitions;
    std::unordered_set<std::string> dynamicDefinitions;
};

MaterialComposerEnvironment::MaterialComposerEnvironment(const std::filesystem::path& resourceRootDir)
    : inja::Environment()
{
    set_line_statement("#");
    set_search_included_templates_in_files(false);

    baseDir = resourceRootDir / "internal" / "shaders";

    set_include_callback([&](auto&, auto& name) { return loadTemplate(baseDir / name); });

    add_callback_with_context("defined", 1, [](const inja::Arguments& args, const void* pContext) {
        const auto* arg = args[0];
        const auto* pRenderContext = reinterpret_cast<const SourceRenderContext*>(pContext);

        LEMON_ASSERT(pRenderContext);
        LEMON_ASSERT(arg->is_string(), "definition must be string");

        return pRenderContext->dynamicDefinitions.contains(arg->get<std::string>());
    });

    add_void_callback_with_context("define", 1, [](const inja::Arguments& args, void* pContext) {
        const auto* arg = args[0];
        auto* pRenderContext = reinterpret_cast<SourceRenderContext*>(pContext);

        LEMON_ASSERT(pRenderContext);
        LEMON_ASSERT(arg->is_string(), "definition must be string");

        pRenderContext->dynamicDefinitions.insert(arg->get<std::string>());
    });

    add_callback_with_context("require", 1, [&](const inja::Arguments& args, void* pContext) {
        const auto* arg = args[0];
        auto* pRenderContext = reinterpret_cast<SourceRenderContext*>(pContext);

        LEMON_ASSERT(pRenderContext);
        LEMON_ASSERT(arg->is_string(), "required file path must be string");

        const std::string name = arg->get<std::string>();
        const std::string guardKey = name + "_GUARD";

        if (pRenderContext->dynamicDefinitions.contains(guardKey)) {
            return std::string{};
        }

        pRenderContext->dynamicDefinitions.insert(guardKey);

        // TODO: This template should be cached somehow.
        const auto tpl = loadTemplate(baseDir / name);

        return render(tpl, *pRenderContext->definitions, pContext);
    });
}

std::string
MaterialComposerEnvironment::renderShaderSource(const inja::Template& tpl,
                                                const inja::json& definitions) const
{
    SourceRenderContext context;
    context.definitions = &definitions;
    return render(tpl, definitions, &context);
}

inja::Template
MaterialComposerEnvironment::loadTemplate(const std::filesystem::path& path)
{
    auto res = lemon::io::readTextFile(path);
    if (res) {
        return parse(*res);
    } else {
        std::string strErr = "failed to resolve shader include: [" + path.string() +
                             "] error: " + std::to_string((int)res.error());
        logger::err(strErr);
        return parse(strErr);
    }
}
