#pragma once

#include <glm/glm.hpp>
#include <cereal/cereal.hpp>

namespace cereal {
    template<class TArchive, size_t NComponents, typename TComponent, glm::qualifier Q>
    inline void
    serialize(TArchive& ar, glm::vec<NComponents, TComponent, Q>& v) {
        if constexpr (NComponents == 1) {
            ar(v.x);
        } else if constexpr (NComponents == 2) {
            ar(v.x, v.y);
        } else if constexpr (NComponents == 3) {
            ar(v.x, v.y, v.z);
        } else if constexpr (NComponents == 4) {
            ar(v.x, v.y, v.z, v.w);
        } else {
            static_assert(false);
        }
    }

    template<class TArchive, size_t NColumns, size_t NRows, typename TComponent, glm::qualifier Q>
    inline void
    serialize(TArchive& ar, glm::mat<NColumns, NRows, TComponent, Q>& m) {
        if constexpr (NColumns == 2) {
            ar(m[0], m[1]);
        } else if constexpr (NColumns == 3) {
            ar(m[0], m[1], m[2]);
        } else if constexpr (NColumns == 4) {
            ar(m[0], m[1], m[2], m[3]);
        } else {
            static_assert(false);
        }
    }
} // namespace cereal
