#include "catch.h"
#include "utils.h"
#include <lemon/shared/AtomicCache.h>

using namespace lemon;

TEST_COUNTER(Data);

TEST_CASE("AtomicCache")
{
    auto initializer = []() { return new Data(1); };

    {
        AtomicCache<Data> cache;

        {
            KeepAlive<Data> ref0;

            REQUIRE((bool)ref0 == false);

            {
                auto [ref1, bRef1Initialized] = cache.findOrInsert(1, initializer);

                REQUIRE(Data::aliveCount == 1);
                REQUIRE(bRef1Initialized);
                REQUIRE(ref1.refCount() == 1);

                auto [ref2, bRef2Initialized] = cache.findOrInsert(1, initializer);

                REQUIRE(Data::aliveCount == 1);
                REQUIRE(!bRef2Initialized);
                REQUIRE(ref1.refCount() == 2);
                REQUIRE(ref2.refCount() == 2);

                REQUIRE(ref1 == ref2);

                // Copy assignment.
                ref0 = ref1;
            }

            REQUIRE(ref0.refCount() == 1);

            // Move constructor.
            KeepAlive<Data> ref3(std::move(ref0));

            REQUIRE(ref3.refCount() == 1);

            // Copy constructor.
            KeepAlive<Data> ref4(ref3);

            REQUIRE(ref4.refCount() == 2);

            // Move assignment.
            KeepAlive<Data> ref5 = std::move(ref3);

            REQUIRE(ref5.refCount() == 2);
        }

        auto [ref, bInitialized] = cache.findOrInsert(1, initializer);

        REQUIRE((bool)ref);
        REQUIRE(Data::aliveCount == 1);
        REQUIRE(bInitialized);

        REQUIRE(ref.get().data == 1);
        REQUIRE(ref->data == 1);
        REQUIRE((*ref).data == 1);
    }

    REQUIRE(Data::aliveCount == 0);
}
