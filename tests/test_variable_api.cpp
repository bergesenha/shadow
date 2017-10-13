#include "catch.hpp"

#include <shadow.hpp>
#include <utility>


namespace tv_space
{
REGISTER_TYPE_BEGIN()
REGISTER_TYPE_END()

SHADOW_INIT()
}


TEST_CASE("default construct a variable", "[variable]")
{
    shadow::variable a;

    REQUIRE(a.has_value() == false);

    SECTION("copy construct another variable from a")
    {
        shadow::variable b = a;

        REQUIRE(b.has_value() == false);
    }

    SECTION("move construct another variable from a")
    {
        shadow::variable b = std::move(a);
    }

    SECTION("default constructo another variable")
    {
        shadow::variable b;

        SECTION("copy assign a to b")
        {
            b = a;

            REQUIRE(b.has_value() == false);
        }

        SECTION("move assign a to b")
        {
            b = std::move(a);

            REQUIRE(b.has_value() == false);
        }
    }
}
