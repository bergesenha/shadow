#include "catch.hpp"

#include <vector>
#include <type_traits>
#include <reflection_binding.hpp>
#include <any.hpp>


// return type and parameter type
int
test_function1(int i)
{
    return i * 2;
}

// return type and no parameter
int
test_function2()
{
    return 10;
}

// one parameter and no return type
void
test_function3(int i)
{
    // do nothing
}

// no return type and no parameter
void
test_function4()
{
}

// two parameters
int
test_function5(int i, char c)
{
    return i * c;
}


// const reference type parameter
int
test_function6(const int& i)
{
    return 3 * i;
}


TEST_CASE("test free function binding point",
          "[generic_free_function_bind_point]")
{
    auto bind_point1 =
        &shadow::generic_free_function_bind_point<decltype(&test_function1),
                                                  &test_function1>;

    auto bind_point2 =
        &shadow::generic_free_function_bind_point<decltype(&test_function2),
                                                  &test_function2>;

    auto bind_point3 =
        &shadow::generic_free_function_bind_point<decltype(&test_function3),
                                                  &test_function3>;

    auto bind_point4 =
        &shadow::generic_free_function_bind_point<decltype(&test_function4),
                                                  &test_function4>;

    auto bind_point5 =
        &shadow::generic_free_function_bind_point<decltype(&test_function5),
                                                  &test_function5>;

    auto bind_point6 =
        &shadow::generic_free_function_bind_point<decltype(&test_function6),
                                                  &test_function6>;

    bool has_same_signature =
        std::is_same<decltype(bind_point1), decltype(bind_point2)>::value;

    REQUIRE(has_same_signature == true);

    SECTION("create argument array and call 1")
    {
        shadow::any args[] = {2};

        auto ret_val = bind_point1(args);

        REQUIRE(ret_val.get<int>() == 4);

        SECTION("call 6 with same arguments")
        {
            ret_val = bind_point6(args);

            REQUIRE(ret_val.get<int>() == 6);
        }
    }

    SECTION("call 2")
    {
        auto ret_val = bind_point2(nullptr);

        REQUIRE(ret_val.get<int>() == 10);
    }

    SECTION("create argument array and call 3")
    {
        std::vector<shadow::any> args;
        args.emplace_back(100);

        auto ret_val = bind_point3(args.data());

        SECTION("add another argument and call 5")
        {
            char c = 3;
            args.emplace_back(c);

            ret_val = bind_point5(args.data());

            REQUIRE(ret_val.get<int>() == 300);
        }
    }

    SECTION("call 4")
    {
        auto ret_val = bind_point4(nullptr);
    }

    SECTION("call 5")
    {
    }
}
