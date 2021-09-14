#pragma once

#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/xml.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/vector.hpp>
#include <magic_enum.hpp>

namespace lemon {
    namespace {
        template<typename T>
        constexpr bool
        isEnum() {
            return magic_enum::is_scoped_enum_v<T> || magic_enum::is_unscoped_enum_v<T>;
        }

        template<typename TArchive>
        constexpr bool
        isOutputArchive() {
            return std::is_same_v<typename TArchive::is_saving, std::true_type>;
        }

        template<typename TArchive>
        constexpr bool
        isInputArchive() {
            return std::is_same_v<typename TArchive::is_loading, std::true_type>;
        }

        template<typename TArchive>
        constexpr bool
        isTextArchive() {
            return std::is_base_of_v<cereal::traits::TextArchive, TArchive>;
        }
    } // namespace

    template<typename TArchive, typename TEnum, typename TName = void>
    void
    serializeEnum(TArchive& ar, TEnum& val, const TName* name = nullptr) {
        if constexpr (isTextArchive<TArchive>()) {
            constexpr bool bUseNVP = std::is_same_v<TName, char>;

            if constexpr (isOutputArchive<TArchive>()) {
                std::string str(magic_enum::enum_name(val));
                if constexpr (bUseNVP) {
                    ar(str);
                } else {
                    ar(cereal::make_nvp(name, str));
                }
            } else if constexpr (isInputArchive<TArchive>()) {
                std::string str;
                if constexpr (bUseNVP) {
                    ar(str);
                } else {
                    ar(cereal::make_nvp(name, str));
                }
                val = *magic_enum::enum_cast<TEnum>(str);
            } else {
                static_assert(false, "TArchive must be either output or input");
            }
        } else {
            if constexpr (isOutputArchive<TArchive>()) {
                int ival = magic_enum::enum_integer(val);
                ar(ival);
            } else if constexpr (isInputArchive<TArchive>()) {
                int ival;
                ar(ival);
                val = *magic_enum::enum_cast<TEnum>(ival);
            } else {
                static_assert(false, "TArchive must be either output or input");
            }
        }
    }

    template<typename TArchive, typename TValue, typename TName = void>
    inline void
    serialize(TArchive& ar, TValue& val, const TName* name = nullptr) {
        if constexpr (isEnum<TValue>()) {
            serializeEnum(ar, val, name);
        } else {
            if constexpr (std::is_same_v<TName, void>) {
                ar(val);
            } else {
                ar(cereal::make_nvp(name, val));
            }
        }
    }
} // namespace lemon

#define LEMON_SERIALIZE(ar, val) lemon::serialize(ar, val, #val)
#define LEMON_SERIALIZE_NVP(ar, name, val) lemon::serialize(ar, val, name)
