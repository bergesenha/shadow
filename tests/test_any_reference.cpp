#include "catch.hpp"
#include <any_reference.hpp>


TEST_CASE("create any_reference from any", "[any_reference]")
{
    using shadow::any;
    using shadow::any_reference;

    any a = 10;

    any_reference ra(a);

    CHECK(ra.get<int>() == 10);
    CHECK(&ra.get<int>() == &a.get<int>());

    SECTION("set a to another value through ra")
    {
        any b = 20;

        ra = b;

        CHECK(a.get<int>() == 20);
        CHECK(ra.get<int>() == 20);
        CHECK(&ra.get<int>() == &a.get<int>());
    }

    SECTION("set a to another value of another type")
    {
        any b = 20.5;

        ra = b;

        CHECK(a.get<double>() == Approx(20.5));
        CHECK(ra.get<double>() == Approx(20.5));
        CHECK(&ra.get<double>() == &a.get<double>());
    }
}

