#include <catch2/catch.hpp>
#include <lemon/shared/FreeList.h>

using namespace lemon;

class Counter {
public:
    static inline int count = 0;
    int i;

    Counter(int ii)
    {
        i = ii;
        count++;
    }

    ~Counter()
    {
        count--;
    }
};

TEST_CASE("FreeList")
{
    REQUIRE(Counter::count == 0);

    {
        // Stack allocated.
        FreeList<Counter, 3, false> list;
        REQUIRE(list.getFreeCount() == 3);

        auto i1 = list.insert(1);
        REQUIRE(list.getFreeCount() == 2);
        REQUIRE(list[i1].i == 1);

        auto i2 = list.insert(2);
        REQUIRE(list.getFreeCount() == 1);
        REQUIRE(list[i2].i == 2);

        auto i3 = list.insert(3);
        REQUIRE(list.getFreeCount() == 0);
        REQUIRE(list[i3].i == 3);

        REQUIRE(Counter::count == 3);

        list.remove(i1);
        list.remove(i2);
        list.remove(i3);

        REQUIRE(list.getFreeCount() == 3);
    }

    REQUIRE(Counter::count == 0);

    {
        // Heap allocated.
        FreeList<Counter, 3, true> list;
        REQUIRE(list.getFreeCount() == 3);

        auto i1 = list.insert(1);
        REQUIRE(list.getFreeCount() == 2);
        REQUIRE(list[i1].i == 1);

        auto i2 = list.insert(2);
        REQUIRE(list.getFreeCount() == 1);
        REQUIRE(list[i2].i == 2);

        auto i3 = list.insert(3);
        REQUIRE(list.getFreeCount() == 0);
        REQUIRE(list[i3].i == 3);

        REQUIRE(Counter::count == 3);

        list.remove(i1);
        list.remove(i2);
        list.remove(i3);

        REQUIRE(list.getFreeCount() == 3);
    }

    REQUIRE(Counter::count == 0);
}
