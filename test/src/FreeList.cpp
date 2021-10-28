#include "catch.h"
#include "utils.h"
#include <lemon/shared/FreeList.h>

using namespace lemon;

TEST_COUNTER(Counter);

TEST_CASE("FreeList")
{
    REQUIRE(Counter::aliveCount == 0);

    {
        // Stack allocated.
        FreeList<Counter, 3, false> list;
        REQUIRE(list.getFreeCount() == 3);

        auto i1 = list.insert(1);
        REQUIRE(list.getFreeCount() == 2);
        REQUIRE(list[i1].data == 1);

        auto i2 = list.insert(2);
        REQUIRE(list.getFreeCount() == 1);
        REQUIRE(list[i2].data == 2);

        auto i3 = list.insert(3);
        REQUIRE(list.getFreeCount() == 0);
        REQUIRE(list[i3].data == 3);

        REQUIRE(Counter::aliveCount == 3);

        list.remove(i1);
        list.remove(i2);
        list.remove(i3);

        REQUIRE(list.getFreeCount() == 3);
    }

    REQUIRE(Counter::aliveCount == 0);

    {
        // Heap allocated.
        FreeList<Counter, 3, true> list;
        REQUIRE(list.getFreeCount() == 3);

        auto i1 = list.insert(1);
        REQUIRE(list.getFreeCount() == 2);
        REQUIRE(list[i1].data == 1);

        auto i2 = list.insert(2);
        REQUIRE(list.getFreeCount() == 1);
        REQUIRE(list[i2].data == 2);

        auto i3 = list.insert(3);
        REQUIRE(list.getFreeCount() == 0);
        REQUIRE(list[i3].data == 3);

        REQUIRE(Counter::aliveCount == 3);

        list.remove(i1);
        list.remove(i2);
        list.remove(i3);

        REQUIRE(list.getFreeCount() == 3);
    }

    REQUIRE(Counter::aliveCount == 0);
}
