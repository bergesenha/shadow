#include "catch.hpp"

#include <api_types.hpp>


TEST_CASE("create a type_tag object", "[type_tag]")
{
    shadow::type_info t_info{"double", 8};

    auto t_tag = shadow::type_tag(t_info);

    REQUIRE(t_tag.name() == std::string("double"));
    REQUIRE(t_tag.size() == 8);
}
