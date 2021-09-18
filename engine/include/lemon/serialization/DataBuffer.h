#pragma once

#include <lemon/shared/DataBuffer.h>
#include <cereal/cereal.hpp>

namespace cereal {
    template<class TArchive, typename TAlloc>
    inline void
    save(TArchive& ar, const lemon::DataBuffer<TAlloc>& data) {
        size_t size = data.size();
        ar(size);

        if (size > 0) {
            ar.saveBinary(data.get<void>(), size);
        }
    }

    template<class TArchive, typename TAlloc>
    inline void
    load(TArchive& ar, lemon::DataBuffer<TAlloc>& data) {
        size_t size;
        ar(size);

        if (size > 0) {
            data.allocate(size);
            ar.loadBinary(data.get<void>(), size);
        }
    }
} // namespace cereal