#include "catch.hpp"
#include <compile_time.hpp>


void
tctff_fun1(void)
{
}

int
tctff_fun2(void)
{
    return 5;
}

int
tctff_fun3(int i)
{
    return 5 * i;
}

int
tctff_fun4(const int& i)
{
    return 5 * i;
}

void
tctff_fun5(int& out)
{
    out = 5;
}

int
tctff_fun6(const int* i)
{
    return *i * 5;
}

void
tctff_fun7(int* i)
{
    *i = 5;
}

int tctff_fun8(int&& i)
{
    return i;
}

namespace tctff_space
{
REGISTER_TYPE_BEGIN()
REGISTER_TYPE_END()

REGISTER_FREE_FUNCTION(tctff_fun1)
REGISTER_FREE_FUNCTION(tctff_fun2)
REGISTER_FREE_FUNCTION(tctff_fun3)
REGISTER_FREE_FUNCTION(tctff_fun4)
REGISTER_FREE_FUNCTION(tctff_fun5)
REGISTER_FREE_FUNCTION(tctff_fun6)
REGISTER_FREE_FUNCTION(tctff_fun7)
REGISTER_FREE_FUNCTION(tctff_fun8)

SHADOW_INIT()
}


TEST_CASE("", "")
{
    constexpr auto num_functions = std::extent<decltype(
        tctff_space::free_function_info_array_holder::value)>::value;

    auto fun_beg =
        std::begin(tctff_space::free_function_info_array_holder::value);
    auto fun_end =
        std::end(tctff_space::free_function_info_array_holder::value);

    auto found1 = std::find_if(fun_beg, fun_end, [](const auto& fi) {
        return std::string(fi.name) == std::string("tctff_fun1");
    });
    auto found2 = std::find_if(fun_beg, fun_end, [](const auto& fi) {
        return std::string(fi.name) == std::string("tctff_fun2");
    });
    auto found3 = std::find_if(fun_beg, fun_end, [](const auto& fi) {
        return std::string(fi.name) == std::string("tctff_fun3");
    });
    auto found4 = std::find_if(fun_beg, fun_end, [](const auto& fi) {
        return std::string(fi.name) == std::string("tctff_fun4");
    });
    auto found5 = std::find_if(fun_beg, fun_end, [](const auto& fi) {
        return std::string(fi.name) == std::string("tctff_fun5");
    });
    auto found6 = std::find_if(fun_beg, fun_end, [](const auto& fi) {
        return std::string(fi.name) == std::string("tctff_fun6");
    });
    auto found7 = std::find_if(fun_beg, fun_end, [](const auto& fi) {
        return std::string(fi.name) == std::string("tctff_fun7");
    });
    auto found8 = std::find_if(fun_beg, fun_end, [](const auto& fi) {
        return std::string(fi.name) == std::string("tctff_fun7");
    });

    REQUIRE(num_functions == 8);
    REQUIRE(found1 != fun_end);
    REQUIRE(found2 != fun_end);
    REQUIRE(found3 != fun_end);
    REQUIRE(found4 != fun_end);
    REQUIRE(found5 != fun_end);
    REQUIRE(found6 != fun_end);
    REQUIRE(found7 != fun_end);
    REQUIRE(found8 != fun_end);

    SECTION("call tctff_fun1")
    {
        auto ret = found1->bind_point(nullptr);

        CHECK(ret.has_value() == false);
    }

    SECTION("call tctff_fun2")
    {
        auto ret = found2->bind_point(nullptr);

        REQUIRE(ret.has_value() == true);
        CHECK(ret.get<int>() == 5);
    }

    SECTION("call tctff_fun3")
    {
        shadow::any args[] = {11};

        auto ret = found3->bind_point(args);

        REQUIRE(ret.has_value() == true);
        CHECK(ret.get<int>() == 55);
        REQUIRE(args[0].has_value() == true);
        CHECK(args[0].get<int>() == 11);
    }

    SECTION("call tctff_fun4")
    {
        const int argvalue = 100;
        shadow::any args[] = {argvalue};

        auto ret = found4->bind_point(args);

        REQUIRE(ret.has_value() == true);
        CHECK(ret.get<int>() == 500);
        REQUIRE(args[0].has_value() == true);
        CHECK(args[0].get<const int>() == 100);
    }

    SECTION("call tctff_fun5")
    {
        shadow::any args[] = { 10 };

        auto ret = found5->bind_point(args);

        CHECK(ret.has_value() == false);
        REQUIRE(args[0].has_value() == true);
        CHECK(args[0].get<int>() == 5);
    }

    SECTION("call tctff_fun6")
    {
        const int num = 10;

        shadow::any args[] = {&num};

        auto ret = found6->bind_point(args);

        REQUIRE(ret.has_value() == true);
        CHECK(ret.get<int>() == 50);
        REQUIRE(args[0].has_value() == true);
        CHECK(args[0].get<const int*>() == &num);
        CHECK(num == 10);
    }

    SECTION("call tctff_fun7")
    {
        int i = 10;

        shadow::any args[] = {&i};

        auto ret = found7->bind_point(args);

        CHECK(ret.has_value() == false);
        REQUIRE(args[0].has_value() == true);
        CHECK(args[0].get<int*>() == &i);
        CHECK(i == 5);
    }



    SECTION("call tctff_fun8")
    {
        int i = 10;

        shadow::any args[] = {std::move(i)};

        auto ret = found8->bind_point(args);
        REQUIRE(ret.has_value() == true);
        CHECK(ret.get<int>() == 10);
    }
}
