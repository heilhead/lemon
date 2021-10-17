#pragma once

namespace cereal {
    template<class TArchive, size_t Alignment>
    inline void
    save(TArchive& ar, const lemon::AlignedMemory<Alignment>& data)
    {
        size_t size = data.size();
        ar(size);

        if (size > 0) {
            ar.saveBinary(data, size);
        }
    }

    template<class TArchive, size_t Alignment>
    inline void
    load(TArchive& ar, lemon::AlignedMemory<Alignment>& data)
    {
        size_t size;
        ar(size);

        if (size > 0) {
            data.allocate(size);
            ar.loadBinary(data, size);
        }
    }
} // namespace cereal
