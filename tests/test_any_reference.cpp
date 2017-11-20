#include "catch.hpp"
#include <any_reference.hpp>


TEST_CASE("create any_reference from values", "[any_reference]")
{
    using shadow::any_reference;
    using shadow::any;
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

    SECTION("implicitly convert to any")
    {
        any a;
        a = ri;

        REQUIRE(a.has_value() == true);
        CHECK(a.get<int>() == 10);
    }

    SECTION("copy assign an any_reference without a reference")
    {
        any_reference r;
        ri = r;

        CHECK(ri.has_reference() == false);
    }
}

TEST_CASE("default construct an any_reference")
{
    using shadow::any_reference;
    using shadow::any;

    any_reference r;

    CHECK(r.has_reference() == false);

    SECTION("implicitly convert to any")
    {
        any a;
        a = r;

        CHECK(a.has_value() == false);
    }

    SECTION("copy construct from r")
    {
        any_reference rr = r;

        CHECK(r.has_reference() == false);
        CHECK(rr.has_reference() == false);
    }

    SECTION("move construct from r")
    {
        any_reference rr = std::move(r);

        CHECK(rr.has_reference() == false);
    }

    SECTION("copy assign r to any_reference with a reference")
    {
        double d = 2.3;
        any_reference rd(d);

        rd = r;

        CHECK(rd.has_reference() == false);
        CHECK(r.has_reference() == false);
    }

    SECTION("copy assign any_reference with reference to r")
    {
        double d = 2.3;
        any_reference rd(d);

        r = rd;

        REQUIRE(r.has_reference() == true);
        CHECK(r.get<double>() == Approx(2.3));
        CHECK(rd.get<double>() == Approx(2.3));
        CHECK(&r.get<double>() == &rd.get<double>());
    }
}

