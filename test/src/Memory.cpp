#include <catch2/catch.hpp>
#include <lemon/shared/Memory.h>

using namespace lemon;

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
