#include "catch.hpp"

#include <api_types.hpp>


TEST_CASE("create a type_tag object", "[type_tag]")
{
    shadow::type_info t_info{"double", 8};

    auto t_tag = shadow::type_tag(t_info);

    REQUIRE(t_tag.name() == std::string("double"));
    REQUIRE(t_tag.size() == 8);

    SECTION("create another type_tag object with same type_info")
    {
        auto t_tag2 = shadow::type_tag(t_info);

        REQUIRE(t_tag == t_tag2);
    }

    SECTION("create another type_tag with an equivalent type_info")
    {
        shadow::type_info t_info2{"double", 8};

        auto t_tag2 = shadow::type_tag(t_info2);

        REQUIRE(t_tag == t_tag2);
    }

    SECTION("create another type_tag with another type_info")
    {
        shadow::type_info t_info2{"int", 4};

        auto t_tag2 = shadow::type_tag(t_info2);

        REQUIRE(t_tag != t_tag2);
    }
}


TEST_CASE("default construction of shadow::object", "[shadow::object]")
{
    shadow::object a;

    REQUIRE(a.type().name() == std::string("void"));
}
