#include "catch.hpp"
#include <any_reference.hpp>


TEST_CASE("create any_reference from values", "[any_reference]")
{
    using shadow::any_reference;
    int i = 10;

    any_reference ri(i);
    const any_reference cri(i);


    CHECK(ri.get<int>() == 10);

    SECTION("modify value through any_reference")
    {
        ri.get<int>() = 100;

        CHECK(i == 100);
    }

    SECTION("copy construct another any_reference")
    {
        any_reference rii = ri;
        any_reference crii = cri;

        CHECK(rii.get<int>() == 10);
        CHECK(crii.get<int>() == 10);

        SECTION("modify value through new any_reference")
        {
            rii.get<int>() = 100;

            CHECK(rii.get<int>() == 100);
            CHECK(&rii.get<int>() == &i);
        }
    }

    SECTION("move construct another any_reference")
    {
        any_reference rii = std::move(ri);

        CHECK(rii.get<int>() == 10);
    }

    SECTION("copy assign an any_reference to the first one")
    {
        int j = 20;
        any_reference rj(j);

        ri = rj;
        CHECK(ri.get<int>() == rj.get<int>());
        CHECK(&ri.get<int>() == &rj.get<int>());

    }
}

