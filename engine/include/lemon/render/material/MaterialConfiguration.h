#pragma once

#include <inja/inja.hpp>

namespace lemon::render {
    using ComposerDefinitions = inja::json;

    class MaterialConfiguration {
        ComposerDefinitions definitions{};

    public:
        using Value = std::variant<int32_t, uint32_t, float, bool, std::string>;

        template<typename TData>
        inline void
        define(const std::string& name, TData data)
        {
            if constexpr (std::is_base_of_v<Value, TData>) {
                std::visit([&](auto& v) { definitions[name] = v; }, data);
            } else {
                definitions[name] = data;
            }
        }

        template<typename TData>
        inline void
        define(gsl::czstring<> name, TData data)
        {
            definitions[name] = data;
        }

        inline const ComposerDefinitions&
        getDefinitions() const
        {
            return definitions;
        }

        inline void
        merge(const MaterialConfiguration& other)
        {
            // Definitions can be null or otherwise not suitable for merging.
            if (other.definitions.is_structured()) {
                definitions.update(other.definitions);
            }
        }
    };
} // namespace lemon::render

template<>
struct folly::hasher<lemon::render::MaterialConfiguration> {
    using folly_is_avalanching = std::true_type;

    size_t
    operator()(const lemon::render::MaterialConfiguration& data) const
    {
        return nlohmann::detail::hash(data.getDefinitions());
    }
};
