#include "catch.hpp"

#include <any.hpp>
#include <vector>

struct big
{
    double a, b, c;
};

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


TEST_CASE("initialize any with small lvalue", "[shadow::any]")
{
    int i = 10;
    shadow::any a(i);

    REQUIRE(a.has_value() == true);
    CHECK(a.on_heap() == false);

    SECTION("get contained value")
    {
        auto ii = a.get<int>();

        CHECK(ii == 10);
    }

    SECTION("get as reference")
    {
        int& ii = a.get<int&>();

        CHECK(ii == 10);
    }

    SECTION("bind contiained value to const int&")
    {
        const int& ii = a.get<const int&>();

        CHECK(ii == 10);
    }

    SECTION("get as rvalue reference")
    {
        int&& ii = a.get<int&&>();

        CHECK(ii == 10);
    }
}

TEST_CASE("initialize const any with small value", "[shadow::any]")
{
    int i = 10;
    const shadow::any a(i);

    REQUIRE(a.has_value() == true);
    CHECK(a.on_heap() == false);

    SECTION("bind contained value to int")
    {
        int ii = a.get<int>();

        CHECK(ii == 10);
    }

    SECTION("bind contained value to const int&")
    {
        const int& ii = a.get<const int&>();

        CHECK(ii == 10);
        CHECK(&ii == &(a.get<const int&>()));
    }
}

TEST_CASE("initialize with large lvalue", "[shadow::any]")
{
    big b{1.1, 2.2, 3.3};
    shadow::any a(b);

    REQUIRE(a.has_value() == true);
    CHECK(a.on_heap() == true);

    SECTION("get contained value")
    {
        auto bb = a.get<big>();

        CHECK(bb.a == Approx(1.1));
        CHECK(bb.b == Approx(2.2));
        CHECK(bb.c == Approx(3.3));
    }

    SECTION("get as reference")
    {
        big& bb = a.get<big&>();

        CHECK(bb.a == Approx(1.1));
        CHECK(bb.b == Approx(2.2));
        CHECK(bb.c == Approx(3.3));
    }

    SECTION("get as const reference")
    {
        const big& bb = a.get<const big&>();

        CHECK(bb.a == Approx(1.1));
        CHECK(bb.b == Approx(2.2));
        CHECK(bb.c == Approx(3.3));
    }

    SECTION("get as rvalue reference")
    {
        big&& bb = a.get<big&&>();

        CHECK(bb.a == Approx(1.1));
        CHECK(bb.b == Approx(2.2));
        CHECK(bb.c == Approx(3.3));
    }
}

TEST_CASE("initialize const any with large value", "[shadow::any]")
{
    big b{1.1, 2.2, 3.3};
    const shadow::any a(b);

    REQUIRE(a.has_value() == true);
    CHECK(a.on_heap() == true);

    SECTION("bind contained value to big")
    {
        big bb = a.get<big>();

        CHECK(bb.a == Approx(1.1));
        CHECK(bb.b == Approx(2.2));
        CHECK(bb.c == Approx(3.3));
    }

    SECTION("bind contained value to const big&")
    {
        const big& bb = a.get<const big&>();

        CHECK(bb.a == Approx(1.1));
        CHECK(bb.b == Approx(2.2));
        CHECK(bb.c == Approx(3.3));
    }
}

TEST_CASE("initialize with small rvalue", "[shadow::any]")
{
    int i = 10;
    shadow::any a(std::move(i));

    REQUIRE(a.has_value() == true);
    CHECK(a.on_heap() == false);

    SECTION("get contained value")
    {
        auto ii = a.get<int>();

        CHECK(ii == 10);
    }

    SECTION("get as reference")
    {
        int& ii = a.get<int&>();

        CHECK(ii == 10);
    }

    SECTION("get as rvalue reference")
    {
        int&& ii = a.get<int&&>();

        CHECK(ii == 10);
    }
}

TEST_CASE("initialize with big rvalue", "[shadow::any]")
{
    big b{1.1, 2.2, 3.3};
    shadow::any a(std::move(b));

    REQUIRE(a.has_value() == true);
    CHECK(a.on_heap() == true);

    SECTION("get contained value")
    {
        auto bb = a.get<big>();

        CHECK(bb.a == Approx(1.1));
        CHECK(bb.b == Approx(2.2));
        CHECK(bb.c == Approx(3.3));
    }

    SECTION("get as reference")
    {
        big& bb = a.get<big&>();

        CHECK(bb.a == Approx(1.1));
        CHECK(bb.b == Approx(2.2));
        CHECK(bb.c == Approx(3.3));
    }
}
