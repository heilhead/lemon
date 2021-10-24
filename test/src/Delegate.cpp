#include <catch2/catch.hpp>
#include <lemon/shared/common.h>
#include <lemon/shared/NonCopyable.h>
#include <lemon/misc/Delegate.h>

using namespace lemon;

TEST_CASE("Delegate")
{
    static uint32_t counter = 0;

    struct S {
        int x;

        S(int x) : x{x} {}

        int
        mul(int y)
        {
            return x * y;
        }

        static int
        sqr(int x)
        {
            return x * x;
        }

        void
        minc(int val)
        {
            counter += val;
        }

        static void
        sinc(int val)
        {
            counter += val;
        }
    };

    S s(3);

    {
        Delegate<int, int> d([&](int y) { return s.mul(y); });
        auto res = d.invoke(2);
        REQUIRE(res == 6);
    }

    {
        Delegate<int, int> d(&S::mul, &s);
        auto res = d.invoke(2);
        REQUIRE(res == 6);
    }

    {
        Delegate<int, int> d(S::sqr);
        auto res = d.invoke(2);
        REQUIRE(res == 4);
    }

    {
        MulticastDelegate<int> md;

        auto h1 = md.add([&](int y) { s.minc(y); });
        auto h2 = md.add(&S::minc, &s);
        auto h3 = md.add(S::sinc);

        md.invoke(1);
        REQUIRE(counter == 3);
    }

    {
        ReverseMulticastDelegate<int> md;

        auto h1 = md.add([&](int y) { s.minc(y); });
        auto h2 = md.add(&S::minc, &s);
        auto h3 = md.add(S::sinc);

        md.invoke(1);
        REQUIRE(counter == 6);

        REQUIRE(md.remove(h2));

        md.invoke(1);
        REQUIRE(counter == 8);
    }
}
