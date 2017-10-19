#include "catch.hpp"

#include <shadow.hpp>


namespace tct1_space
{
REGISTER_TYPE_BEGIN()
REGISTER_TYPE_END()

SHADOW_INIT()
}

TEST_CASE("create an int using static_construct", "[static_construct]")
{
    auto anint = tct1_space::static_construct<int>(23);

    REQUIRE(anint.type().name() == std::string("int"));
}
