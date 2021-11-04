#include "catch.h"
#include <lemon/shared/PtrRange.h>

using namespace lemon;

TEST_CASE("PtrRange")
{
    uint8_t nums[5] = {1, 2, 3, 4, 5};

    size_t sum = 0;
    for (const auto& num : range(&nums[0], 5)) {
        sum += num;
    }

    REQUIRE(sum == 15);
}
