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
        LambdaDelegate<int, int> d([&](int y) { return s.mul(y); });
        auto res = d.execute(2);
        REQUIRE(res == 6);
    }

    {
        MemberDelegate<int, int> d(&S::mul, &s);
        auto res = d.execute(2);
        REQUIRE(res == 6);
    }

    {
        FunctionDelegate<int, int> d(S::sqr);
        auto res = d.execute(2);
        REQUIRE(res == 4);
    }

    {
        DelegateStack<int> ds;

        auto h1 = ds.add([&](int y) { s.minc(y); });
        auto h2 = ds.add(&S::minc, &s);
        auto h3 = ds.add(S::sinc);

        ds.execute(1);
        REQUIRE(counter == 3);
    }

    {
        ReverseDelegateStack<int> ds;

        auto h1 = ds.add([&](int y) { s.minc(y); });
        auto h2 = ds.add(&S::minc, &s);
        auto h3 = ds.add(S::sinc);

        ds.execute(1);
        REQUIRE(counter == 6);

        REQUIRE(ds.remove(h2));

        ds.execute(1);
        REQUIRE(counter == 8);
    }
}
