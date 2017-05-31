#include "catch.hpp"


#include <reflection_binding.hpp>
#include <vector>


class test_class1
{
public:
    int
    test_member1(int i)
    {
        return i * 2;
    }

    int
    test_member2()
    {
        return local_;
    }

    int
    test_member3() const
    {
        return local_;
    }

    void
    test_member4(int i)
    {
        local_ = i;
    }

    int
    test_member5(int i, char c)
    {
        return i + c;
    }

    int
    test_member6(int i)
    {
        return i;
    }

    int
    test_member6(double d)
    {
        return d;
    }

    int local_ = 111;
};


TEST_CASE("test binding of member functions",
          "[member_function_detail::generic_member_function_bind_point]")
{
    auto bind_point1 =
        &shadow::member_function_detail::generic_member_function_bind_point<
            decltype(&test_class1::test_member1),
            &test_class1::test_member1>;

    auto bind_point2 =
        &shadow::member_function_detail::generic_member_function_bind_point<
            decltype(&test_class1::test_member2),
            &test_class1::test_member2>;

    auto bind_point3 =
        &shadow::member_function_detail::generic_member_function_bind_point<
            decltype(&test_class1::test_member3),
            &test_class1::test_member3>;

    auto bind_point4 =
        &shadow::member_function_detail::generic_member_function_bind_point<
            decltype(&test_class1::test_member4),
            &test_class1::test_member4>;

    auto bind_point5 =
        &shadow::member_function_detail::generic_member_function_bind_point<
            decltype(&test_class1::test_member5),
            &test_class1::test_member5>;

    auto bind_point6_ol1 =
        &shadow::member_function_detail::generic_member_function_bind_point<
            decltype(static_cast<int (test_class1::*)(int)>(
                &test_class1::test_member6)),
            &test_class1::test_member6>;

    auto bind_point6_ol2 =
        &shadow::member_function_detail::generic_member_function_bind_point<
            decltype(static_cast<int (test_class1::*)(double)>(
                &test_class1::test_member6)),
            &test_class1::test_member6>;

    SECTION("create a test_class1 and an int")
    {
        shadow::any t1 = test_class1();

        SECTION("create an int as argument and call 1")
        {
            shadow::any anint = 20;

            auto ret_val = bind_point1(t1, &anint);

            REQUIRE(ret_val.get<int>() == 40);
        }

        SECTION("call 2")
        {
            auto ret_val = bind_point2(t1, nullptr);

            REQUIRE(ret_val.get<int>() == 111);
        }

        SECTION("call 3")
        {
            auto ret_val = bind_point3(t1, nullptr);

            REQUIRE(ret_val.get<int>() == 111);
        }

        SECTION("create an int as argument and call 4")
        {
            shadow::any anint = 44;

            bind_point4(t1, &anint);

            REQUIRE(t1.get<test_class1>().local_ == 44);
        }

        SECTION("create argument array and call 5")
        {
            std::vector<shadow::any> arg_arr;
            arg_arr.emplace_back(10);
            arg_arr.emplace_back(char(3));

            auto ret_val = bind_point5(t1, arg_arr.data());

            REQUIRE(ret_val.get<int>() == 13);
        }

        SECTION("create int and double and call overloads of 6")
        {
            shadow::any anint = 10;
            shadow::any adouble = 11.1;

            auto ret_val1 = bind_point6_ol1(t1, &anint);
            auto ret_val2 = bind_point6_ol2(t1, &adouble);

            REQUIRE(ret_val1.get<int>() == 10);
            REQUIRE(ret_val2.get<int>() == 11);
        }
    }
}
