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


TEST_CASE("create a variable with an int using static_create", "[variable]")
{
    shadow::variable a = tv_space::static_create<int>(10);

    REQUIRE(a.has_value() == true);
    REQUIRE(tv_space::static_value_cast<int>(a) == 10);

    SECTION("copy construct another variable from a")
    {
        shadow::variable b = a;

        REQUIRE(a.has_value() == true);
        REQUIRE(tv_space::static_value_cast<int>(a) == 10);
        REQUIRE(b.has_value() == true);
        REQUIRE(tv_space::static_value_cast<int>(b) == 10);
    }
}
