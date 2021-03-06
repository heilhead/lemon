#pragma once

#include <lemon/pch/inja.h>

namespace lemon::res::material {
    class MaterialComposerEnvironment : public inja::Environment {
        std::filesystem::path baseDir;
        std::mutex parserLock;

    public:
        MaterialComposerEnvironment(const std::filesystem::path& resourceRootDir);

        std::string
        renderShaderSource(const inja::Template& tpl, const inja::json& definitions) const;

        inja::Template
        loadTemplate(const std::filesystem::path& path);

    private:
        inja::Template
        loadTemplateByName(const std::string& name);

        inja::Template
        loadTemplateByPath(const std::filesystem::path& fullPath);
    };
} // namespace lemon::res::material
