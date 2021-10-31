#include "catch.h"
#include "utils.h"
#include <lemon/shared/SlotMap.h>

using namespace lemon;

TEST_COUNTER(Counter);

TEST_CASE("SlotMap")
{
    {
        SlotMap<Counter> slotMap;

        {
            auto handle = slotMap.insert(1);

            REQUIRE(slotMap.isValid(handle));
            REQUIRE(slotMap[handle].data == 1);

            slotMap.remove(handle);

            REQUIRE(!slotMap.isValid(handle));
        }

        REQUIRE(slotMap.getSize() == 0);
        REQUIRE(slotMap.getCapacity() == 1);

        {
            auto h1 = slotMap.insert(1);
            auto h2 = slotMap.insert(2);
            auto h3 = slotMap.insert(3);
            auto h4 = slotMap.insert(4);

            // move++
            slotMap.remove(h2);

            REQUIRE(slotMap.getSize() == 3);

            // `2` was removed, `4` took its place.
            REQUIRE(slotMap[1].data == 4);

            // no move
            slotMap.remove(h3);

            REQUIRE(slotMap.getSize() == 2);

            slotMap.clear();
        }

        REQUIRE(slotMap.getCapacity() == 4);

        auto h1 = slotMap.insert(1);
        auto h2 = slotMap.insert(2);
        auto h3 = slotMap.insert(3);
        auto h4 = slotMap.insert(4);
        auto h5 = slotMap.insert(5);

        REQUIRE(slotMap.getSize() == 5);
        REQUIRE(slotMap.getCapacity() == 8);

        REQUIRE(slotMap[0].data == 1);
        REQUIRE(slotMap[1].data == 2);
        REQUIRE(slotMap[2].data == 3);

        // move++
        REQUIRE(slotMap.remove(0));
        REQUIRE(!slotMap.remove(999));

        // `3` replaced `1` when it was removed.
        REQUIRE(slotMap.getHandle(0) == h5);
    }

    REQUIRE(Counter::aliveCount == 0);

    // Make sure move/copy counts are correct.
    REQUIRE(Counter::moveCtors == 2);
    REQUIRE(Counter::copyCtors == 0);
    REQUIRE(Counter::moveAssigns == 0);
    REQUIRE(Counter::copyAssigns == 0);
}
