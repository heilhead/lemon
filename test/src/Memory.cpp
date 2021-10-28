#include "catch.h"
#include "utils.h"
#include <lemon/shared/Memory.h>

using namespace lemon;

TEST_COUNTER(Counter);

struct CustomAlign {
    alignas(128) int x;
    int y;
    alignas(32) int z;
};

TEST_CASE("Storage")
{
    REQUIRE(sizeof(MaybeUninit<Counter>) == sizeof(Counter));
    REQUIRE(alignof(MaybeUninit<Counter>) == alignof(Counter));
    REQUIRE(alignof(MaybeUninit<CustomAlign>) == alignof(CustomAlign));
    REQUIRE(alignof(MaybeUninit<CustomAlign>) == alignof(CustomAlign));

    {
        REQUIRE(Counter::aliveCount == 0);

        MaybeUninit<Counter> mem;

        // No constructor should be called.
        REQUIRE(Counter::aliveCount == 0);

        mem.init(1);

        REQUIRE(Counter::aliveCount == 1);

        REQUIRE((*mem).data == 1);
    }

    // No destructor called, counter remains the same.
    REQUIRE(Counter::aliveCount == 1);

    {
        MaybeUninit<Counter> mem;

        // Use destructor without constructing first. UB in real world, OK for test.
        mem.destroy();

        REQUIRE(Counter::aliveCount == 0);
    }
}

TEST_CASE("AlignedMemory")
{
    using Mem = AlignedMemory<4>;

    static constexpr size_t testSize = 64;

    {
        Mem mem;
        mem.allocate(testSize);

        REQUIRE(mem.size() == testSize);
    }

    {
        Mem mem1;
        mem1.allocate(testSize);

        Mem mem2;
        mem2 = mem1;

        // `mem1` was copied, both should be 64 bytes.
        REQUIRE(mem1.size() == testSize);
        REQUIRE(mem2.size() == testSize);

        Mem mem3;
        mem3 = std::move(mem2);

        // `mem2` was moved, should be empty now.
        REQUIRE(mem2.size() == 0);
        REQUIRE(mem3.size() == testSize);
    }
}
