#include <catch2/catch.hpp>
#include <lemon/shared/AtomicCache.h>

using namespace lemon;

TEST_CASE("AtomicCache")
{
    static uint32_t staticRefs = 0;
    struct Data {
        uint32_t data = 1;

        Data()
        {
            staticRefs++;
        }

        ~Data()
        {
            staticRefs--;
        }
    };

    auto initializer = []() { return new Data(); };

    {
        AtomicCache<Data> cache;

        {
            ResourceRef<Data> ref0;

            REQUIRE((bool)ref0 == false);

            {
                auto [ref1, bRef1Initialized] = cache.findOrInsert(1, initializer);

                REQUIRE(staticRefs == 1);
                REQUIRE(bRef1Initialized);
                REQUIRE(ref1.refCount() == 1);

                auto [ref2, bRef2Initialized] = cache.findOrInsert(1, initializer);

                REQUIRE(staticRefs == 1);
                REQUIRE(!bRef2Initialized);
                REQUIRE(ref1.refCount() == 2);
                REQUIRE(ref2.refCount() == 2);

                REQUIRE(ref1 == ref2);

                // Copy assignment.
                ref0 = ref1;
            }

            REQUIRE(ref0.refCount() == 1);

            // Move constructor.
            ResourceRef<Data> ref3(std::move(ref0));

            REQUIRE(ref3.refCount() == 1);

            // Copy constructor.
            ResourceRef<Data> ref4(ref3);

            REQUIRE(ref4.refCount() == 2);

            // Move assignment.
            ResourceRef<Data> ref5 = std::move(ref3);

            REQUIRE(ref5.refCount() == 2);
        }

        auto [ref, bInitialized] = cache.findOrInsert(1, initializer);

        REQUIRE((bool)ref);
        REQUIRE(staticRefs == 1);
        REQUIRE(bInitialized);

        REQUIRE(ref.get().data == 1);
        REQUIRE(ref->data == 1);
        REQUIRE((*ref).data == 1);
    }

    REQUIRE(staticRefs == 0);
}