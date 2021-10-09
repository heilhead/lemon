#pragma once

namespace lemon {
    class NonCopyable {
    protected:
        constexpr NonCopyable() = default;
        ~NonCopyable() = default;

    private:
        NonCopyable(const NonCopyable&) = delete;
        void
        operator=(const NonCopyable&) = delete;
    };

    class NonMovable : NonCopyable {
    protected:
        constexpr NonMovable() = default;
        ~NonMovable() = default;

    private:
        NonMovable(NonMovable&&) = delete;
        void
        operator=(NonMovable&&) = delete;
    };
} // namespace lemon