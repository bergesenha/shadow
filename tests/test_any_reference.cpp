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
}

