#include "catch.hpp"

#include <shadow.hpp>


REGISTER_TYPE_BEGIN()
REGISTER_TYPE_END()

SHADOW_INIT()


TEST_CASE("test special member functions of shadow::variable", "[variable]")
{
    shadow::variable a;

    REQUIRE(a.has_value() == false);
}
