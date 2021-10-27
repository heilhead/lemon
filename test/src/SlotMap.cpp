#include <catch2/catch.hpp>
#include <lemon/shared/SlotMap.h>

using namespace lemon;

class Counter {
public:
    static inline int count = 0;
    int i;

    Counter() = default;

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

TEST_CASE("SlotMap")
{
    {
        SlotMap<Counter, 10> slotMap;

        {
            auto handle = slotMap.insert(1);

            REQUIRE(slotMap.isValid(handle));
            REQUIRE(slotMap[handle].i == 1);

            slotMap.remove(handle);

            REQUIRE(!slotMap.isValid(handle));
        }

        REQUIRE(slotMap.getSize() == 0);

        {
            auto h1 = slotMap.insert(1);
            auto h2 = slotMap.insert(2);
            auto h3 = slotMap.insert(3);
            auto h4 = slotMap.insert(4);

            slotMap.remove(h2);

            REQUIRE(slotMap.getSize() == 3);

            // `2` was removed, `4` took its place.
            REQUIRE(slotMap[1].i == 4);

            slotMap.remove(h3);

            REQUIRE(slotMap.getSize() == 2);

            slotMap.clear();
        }

        auto h1 = slotMap.insert(1);
        auto h2 = slotMap.insert(2);
        auto h3 = slotMap.insert(3);

        REQUIRE(slotMap.getSize() == 3);

        REQUIRE(slotMap[0].i == 1);
        REQUIRE(slotMap[1].i == 2);
        REQUIRE(slotMap[2].i == 3);

        REQUIRE(slotMap.remove(0));
        REQUIRE(!slotMap.remove(999));

        // `3` replaced `1` when it was removed.
        REQUIRE(slotMap.getHandle(0) == h3);
    }

    REQUIRE(Counter::count == 0);
}
