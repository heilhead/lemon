#pragma once

#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/xml.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>
#include <magic_enum.hpp>
#include <lemon/serialization/common.h>

using namespace magic_enum::bitwise_operators;

namespace lemon {
    /// <summary>
    /// Re-export `magic_enum` functions for flags and non-flags based enums in a way that can be used with
    /// templates.
    /// </summary>
    struct EnumTraits {
        template<typename TEnum, typename TVal>
        inline static constexpr auto
        cast(TVal val)
        {
            return magic_enum::enum_cast<TEnum>(val);
        };

        template<typename TEnum>
        inline static constexpr auto
        name(TEnum val)
        {
            return magic_enum::enum_name(val);
        };

        template<typename TEnum>
        inline static constexpr auto
        integer(TEnum val)
        {
            return magic_enum::enum_integer(val);
        };

        struct Flags {
            template<typename TEnum, typename TVal>
            inline static constexpr auto
            cast(TVal val)
            {
                return magic_enum::flags::enum_cast<TEnum>(val);
            };

            template<typename TEnum>
            inline static constexpr auto
            name(TEnum val)
            {
                return magic_enum::flags::enum_name(val);
            };

            template<typename TEnum>
            inline static constexpr auto
            integer(TEnum val)
            {
                return magic_enum::flags::enum_integer(val);
            };
        };
    };

    namespace serialization_detail {
        template<typename T>
        constexpr bool
        isEnum()
        {
            return magic_enum::is_scoped_enum_v<T> || magic_enum::is_unscoped_enum_v<T>;
        }

        template<typename TArchive>
        constexpr bool
        isOutputArchive()
        {
            return std::is_same_v<typename TArchive::is_saving, std::true_type>;
        }

        template<typename TArchive>
        constexpr bool
        isInputArchive()
        {
            return std::is_same_v<typename TArchive::is_loading, std::true_type>;
        }

        template<typename TArchive>
        constexpr bool
        isTextArchive()
        {
            return std::is_base_of_v<cereal::traits::TextArchive, TArchive>;
        }

        template<typename TEnum, bool bFlags>
        std::optional<TEnum>
        enum_cast(std::string_view& val)
        {
            if constexpr (bFlags) {
                return magic_enum::flags::enum_cast<TEnum>(val);
            } else {
                return magic_enum::enum_cast<TEnum>(val);
            }
        }

        template<typename TArchive, typename TEnum, typename TName = void, typename TEnumTraits = EnumTraits>
        void
        serializeEnum(TArchive& ar, TEnum& val, const TName* name = nullptr)
        {
            if constexpr (isTextArchive<TArchive>()) {
                constexpr bool bUseNVP = std::is_same_v<TName, char>;

                if constexpr (isOutputArchive<TArchive>()) {
                    std::string str(TEnumTraits::template name(val));
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
                    val = *TEnumTraits::template cast<TEnum>(str);
                } else {
                    static_assert(false, "TArchive must be either output or input");
                }
            } else {
                if constexpr (isOutputArchive<TArchive>()) {
                    int ival = TEnumTraits::template integer(val);
                    ar(ival);
                } else if constexpr (isInputArchive<TArchive>()) {
                    int ival;
                    ar(ival);
                    val = *TEnumTraits::template cast<TEnum>(ival);
                } else {
                    static_assert(false, "TArchive must be either output or input");
                }
            }
        }

        template<typename TArchive, typename TValue, typename TName = void>
        inline void
        serializeImpl(TArchive& ar, TValue& val, const TName* name = nullptr)
        {
            if constexpr (isEnum<TValue>()) {
                serializeEnum(ar, val, name);
            } else {
                if constexpr (std::is_same_v<TName, void> || !isTextArchive<TArchive>()) {
                    ar(val);
                } else {
                    ar(cereal::make_nvp(name, val));
                }
            }
        }

        template<typename TArchive, typename TEnum, typename TName = void>
        void
        serializeFlags(TArchive& ar, TEnum& val, const TName* name = nullptr)
        {
            serializeEnum<TArchive, TEnum, TName, EnumTraits::Flags>(ar, val, name);
        }
    } // namespace serialization_detail
} // namespace lemon

#define LEMON_SERIALIZE(ar, val) ::lemon::serialization_detail::serializeImpl(ar, val, #val)
#define LEMON_SERIALIZE_NVP(ar, name, val) ::lemon::serialization_detail::serializeImpl(ar, val, #name)
#define LEMON_SERIALIZE_FLAGS(ar, val) ::lemon::serialization_detail::serializeFlags(ar, val, #val)
#define LEMON_SERIALIZE_NVP_FLAGS(ar, name, val) ::lemon::serialization_detail::serializeFlags(ar, val, #name)
