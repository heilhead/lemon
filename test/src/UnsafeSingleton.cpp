#include "catch.h"
#include "utils.h"
#include <lemon/shared/UnsafeSingleton.h>

using namespace lemon;

TEST_CASE("UnsafeSingleton")
{
    static int i = 0;

    class S : public UnsafeSingleton<S> {
    public:
        S()
        {
            i++;
        }

        ~S()
        {
            i--;
        }
    };

    REQUIRE(i == 0);
    REQUIRE(S::get() == nullptr);

    {
        S s;

        REQUIRE(i == 1);
        REQUIRE(S::get() == &s);
    }

    REQUIRE(i == 0);
    REQUIRE(S::get() == nullptr);
}
