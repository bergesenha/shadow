#include "catch.hpp"

#include <reflection_binding.hpp>


struct test_construct1
{
    test_construct1(int in, float fn) : i(2 * in), f(fn)
    {
    }

    int i;
    float f;
};

struct test_construct2
{
    char c;
    double d;
};

TEST_CASE("test constructor binding", "[generic_constructor_bind_point]")
{
    auto int_construct_from_int =
        &shadow::constructor_detail::generic_constructor_bind_point<int, int>;

    auto int_construct_from_float =
        &shadow::constructor_detail::generic_constructor_bind_point<int, float>;

    auto test_construct1_constructor =
        &shadow::constructor_detail::
            generic_constructor_bind_point<test_construct1, int, float>;

    auto test_construct2_constructor =
        &shadow::constructor_detail::
            generic_constructor_bind_point<test_construct2, char, double>;


    SECTION("construct int with int")
    {
        shadow::any intarg = 11;

        auto result = int_construct_from_int(&intarg);

        REQUIRE(result.get<int>() == 11);
    }

    SECTION("construct int with float")
    {
        shadow::any floatarg = 23.5f;

        auto result = int_construct_from_float(&floatarg);

        REQUIRE(result.get<int>() == 23);
    }

    SECTION("when constructing a test_construct1 with int, float args, the "
            "constructor should be chosen")
    {
        shadow::any args[] = {10, 23.4f};

        auto result = test_construct1_constructor(args);

        REQUIRE(result.get<test_construct1>().i == 20);
        REQUIRE(result.get<test_construct1>().f == 23.4f);
    }

    SECTION("when constructing a test_construct2, braced-init-list "
            "construction should be chosen")
    {
        shadow::any args[] = {'a', 25.3};

        auto result = test_construct2_constructor(args);

        REQUIRE(result.get<test_construct2>().c == 'a');
        REQUIRE(result.get<test_construct2>().d == 25.3);
    }
}
