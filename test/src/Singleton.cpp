#include <catch2/catch.hpp>
#include <lemon/shared/Singleton.h>

using namespace lemon;

TEST_CASE("Singleton")
{
    static int i = 0;

    class S : public Singleton<S> {
    public:
        S(const Pass& pass) : Singleton(pass)
        {
            i++;
        }

        ~S()
        {
            i--;
        }
    };

    REQUIRE(i == 0);
    REQUIRE(S::getUnchecked() == nullptr);

    auto& s = S::get();

    REQUIRE(i == 1);
    REQUIRE(&s == S::getUnchecked());

    S::reset();

    REQUIRE(i == 0);
    REQUIRE(S::getUnchecked() == nullptr);
}
