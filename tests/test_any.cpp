#include "catch.hpp"

#include <any.hpp>
#include <vector>


TEST_CASE("test class any", "[any]")
{
    shadow::any a;


    SECTION("construct another any with a value")
    {
        shadow::any b(23);

        REQUIRE(b.get<int>() == 23);

        SECTION("assign b to a")
        {
            a = b;

            REQUIRE(a.get<int>() == 23);
            REQUIRE(b.get<int>() == 23);
        }

        SECTION("copy construct any from b")
        {
            shadow::any c(b);

            REQUIRE(b.get<int>() == 23);
            REQUIRE(c.get<int>() == 23);
        }

        SECTION("assign a to b")
        {
            b = a;
        }

        SECTION("modify underlying int value of b")
        {
            b.get<int>() = 100;

            REQUIRE(b.get<int>() == 100);
        }

        SECTION("move construct an any with b")
        {
            shadow::any c(std::move(b));

            REQUIRE(c.get<int>() == 23);
        }

        SECTION("move assign an any by b")
        {
            shadow::any c;

            c = std::move(b);

            REQUIRE(c.get<int>() == 23);
        }
    }
}


TEST_CASE("test move construction of any", "[any]")
{
    shadow::any a(shadow::any(14.5));

    REQUIRE(a.get<double>() == 14.5);
}


TEST_CASE("std::vector of any", "[any]")
{
    std::vector<shadow::any> any_vec;

    SECTION("construct any of int in place")
    {
        any_vec.emplace_back(10);

        REQUIRE(any_vec.front().get<int>() == 10);

        SECTION("emplace back an any of float")
        {
            any_vec.emplace_back(10.6f);

            REQUIRE(any_vec.front().get<int>() == 10);
            REQUIRE(any_vec.back().get<float>() == 10.6f);
        }
    }

    SECTION("push back rvalue")
    {
        any_vec.push_back(shadow::any(10));

        REQUIRE(any_vec.front().get<int>() == 10);
    }

    SECTION("push back lvalue")
    {
        shadow::any a = 23.5;

        any_vec.push_back(a);

        REQUIRE(a.get<double>() == 23.5);
        REQUIRE(any_vec.front().get<double>() == 23.5);
    }
}
