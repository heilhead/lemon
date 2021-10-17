#pragma once

namespace cereal {
    template<class TArchive, typename TData>
    inline void
    save(TArchive& ar, const std::optional<TData>& v)
    {
        bool bHasValue = v.has_value();
        ar(bHasValue);

        if (bHasValue) {
            const TData& data = *v;
            ar(data);
        }
    }

    template<class TArchive, typename TData>
    inline void
    load(TArchive& ar, std::optional<TData>& v)
    {
        bool bHasValue;
        ar(bHasValue);

        if (bHasValue) {
            TData data;
            ar(data);
            v = std::move(data);
        } else {
            v = std::nullopt;
        }
    }
} // namespace cereal
