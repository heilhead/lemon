#pragma once

namespace lemon {
    template<typename T>
    concept Sized = sizeof(T) > 0;

    template<typename TDerived, typename TBase>
    concept DerivedFrom = std::derived_from<TDerived, TBase>;

    template<typename TDerived, typename TBase>
    concept Base = DerivedFrom<TDerived, TBase>;
} // namespace lemon
