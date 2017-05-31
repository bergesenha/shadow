#include "catch.hpp"

#include <reflection_binding.hpp>

class test_class_mv1
{
public:
    int var1 = 10;
    double var2 = 20.5;
};


TEST_CASE("test binding of member variables", "[member_variable_detail::]")
{
    auto get_bind_point1 =
        &shadow::member_variable_detail::generic_member_variable_get_bind_point<
            decltype(&test_class_mv1::var1),
            &test_class_mv1::var1>;

    auto set_bind_point1 =
        &shadow::member_variable_detail::generic_member_variable_set_bind_point<
            decltype(&test_class_mv1::var1),
            &test_class_mv1::var1>;

    auto get_bind_point2 =
        &shadow::member_variable_detail::generic_member_variable_get_bind_point<
            decltype(&test_class_mv1::var2),
            &test_class_mv1::var2>;

    auto set_bind_point2 =
        &shadow::member_variable_detail::generic_member_variable_set_bind_point<
            decltype(&test_class_mv1::var2),
            &test_class_mv1::var2>;

    SECTION("make an any of test_class_mv1")
    {
        shadow::any t1 = test_class_mv1();

        SECTION("get var1")
        {
            auto res = get_bind_point1(t1);

            REQUIRE(res.get<int>() == 10);
        }

        SECTION("get var2")
        {
            auto res = get_bind_point2(t1);

            REQUIRE(res.get<double>() == 20.5);
        }

        SECTION("set var1 and var2")
        {
            set_bind_point1(t1, shadow::any(100));
            set_bind_point2(t1, shadow::any(150.4));

            REQUIRE(get_bind_point1(t1).get<int>() == 100);
            REQUIRE(get_bind_point2(t1).get<double>() == 150.4);
        }
    }

    SECTION("make a const any of test_class_mv1")
    {
        const shadow::any t1 = test_class_mv1();

        SECTION("get var1")
        {
            auto res = get_bind_point1(t1);

            REQUIRE(res.get<int>() == 10);
        }

        SECTION("get var2")
        {
            auto res = get_bind_point2(t1);

            REQUIRE(res.get<double>() == 20.5);
        }
    }
}
