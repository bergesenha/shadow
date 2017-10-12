#include "catch.hpp"
#include <shadow.hpp>


struct tmv1
{
    int i_;
    float f_;
};

struct tmv2
{
    tmv2(int i, float f) : i_(2 * i), f_(2 * f)
    {
    }


    int i_;
    float f_;
};

namespace tmv_space
{
REGISTER_TYPE_BEGIN()
REGISTER_TYPE(tmv1)
REGISTER_TYPE(tmv2)
REGISTER_TYPE_END()

REGISTER_MEMBER_VARIABLE(tmv1, i_)
REGISTER_MEMBER_VARIABLE(tmv1, f_)


REGISTER_MEMBER_VARIABLE(tmv2, i_)
REGISTER_MEMBER_VARIABLE(tmv2, f_)

SHADOW_INIT()
}


TEST_CASE("create tmv1", "[reflection_manager]")
{
    auto obj = tmv_space::static_create<tmv1>(10, 23.4f);

    REQUIRE(tmv_space::static_value_cast<tmv1>(obj).i_ == 10);
    REQUIRE(tmv_space::static_value_cast<tmv1>(obj).f_ == Approx(23.4f));

    SECTION("obtain member variables")
    {
        auto mv_pair = obj.member_variables();

        REQUIRE(std::distance(mv_pair.first, mv_pair.second) == 2);

        SECTION("find member variable called i_")
        {
            auto found =
                std::find_if(mv_pair.first, mv_pair.second, [](const auto& mv) {
                    return mv.name() == std::string("i_");
                });

            REQUIRE(found != mv_pair.second);
            REQUIRE(found->type().name() == std::string("int"));
            REQUIRE(found->object_type().name() == std::string("tmv1"));

            SECTION("get value from member variable")
            {
                auto i_value = obj.get_member_variable(*found);

                REQUIRE(tmv_space::static_value_cast<int>(i_value) == 10);
            }

            SECTION("set value of member variable")
            {
                obj.set_member_variable(*found,
                                        tmv_space::static_create<int>(100));

                REQUIRE(tmv_space::static_value_cast<tmv1>(obj).i_ == 100);
                REQUIRE(tmv_space::static_value_cast<int>(
                            obj.get_member_variable(*found)) == 100);
            }
        }

        SECTION("find member variable called f_")
        {
            auto found =
                std::find_if(mv_pair.first, mv_pair.second, [](const auto& mv) {
                    return mv.name() == std::string("f_");
                });

            REQUIRE(found != mv_pair.second);
            REQUIRE(found->type().name() == std::string("float"));
            REQUIRE(found->object_type().name() == std::string("tmv1"));

            SECTION("get value from member variable")
            {
                auto f_value = obj.get_member_variable(*found);

                REQUIRE(tmv_space::static_value_cast<float>(f_value) ==
                        Approx(23.4f));
            }
        }
    }
}


TEST_CASE("create tmv2", "[reflection_manager]")
{
    auto obj = tmv_space::static_create<tmv2>(10, 23.4f);

    REQUIRE(tmv_space::static_value_cast<tmv2>(obj).i_ == 20);
    REQUIRE(tmv_space::static_value_cast<tmv2>(obj).f_ == Approx(2 * 23.4f));

    SECTION("obtain member variables")
    {
        auto mv_pair = obj.member_variables();

        REQUIRE(std::distance(mv_pair.first, mv_pair.second) == 2);
    }
}
