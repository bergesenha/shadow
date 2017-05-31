#include "catch.hpp"


#include <reflection_binding.hpp>


class test_class1
{
public:
    int
    test_member1(int i)
    {
        return i * 2;
    }
};


TEST_CASE("test binding of member functions",
          "[member_function_detail::generic_member_function_bind_point]")
{
    auto bind_point1 =
        &shadow::member_function_detail::generic_member_function_bind_point<
            decltype(&test_class1::test_member1),
            &test_class1::test_member1>;


    SECTION("create a test_class1 and an int")
    {
        shadow::any t1 = test_class1();

        SECTION("create an int as argument and call 1")
        {
            shadow::any anint = 20;

            auto ret_val = bind_point1(t1, &anint);

            REQUIRE(ret_val.get<int>() == 40);
        }
    }
}
