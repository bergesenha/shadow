#include "catch.hpp"

#include <shadow.hpp>

class tmfapi1
{
public:
    tmfapi1() : public_value(0)
    {
    }

    tmfapi1(int i) : public_value(i)
    {
    }

    int public_value;

    void
    fun1()
    {
        public_value = 100;
    }

    void
    fun2(int i)
    {
        public_value = i;
    }

    int
    fun3() const
    {
        return 23;
    }

    int
    fun4(int i) const
    {
        return 2 * i;
    }
};


namespace tmfapi_space
{
REGISTER_TYPE_BEGIN()
REGISTER_TYPE(tmfapi1)
REGISTER_TYPE_END()

REGISTER_MEMBER_FUNCTION(tmfapi1, fun1)
REGISTER_MEMBER_FUNCTION(tmfapi1, fun2)
REGISTER_MEMBER_FUNCTION(tmfapi1, fun3)
REGISTER_MEMBER_FUNCTION(tmfapi1, fun4)

SHADOW_INIT()
}


TEST_CASE("set up shadow::variable containing a tmfapi object",
          "[reflection_manager]")
{
    auto test_obj = tmfapi_space::static_create<tmfapi1>(20);

    auto mem_fun_pair = test_obj.member_functions();

    SECTION("find fun1 and call it")
    {
        auto found = std::find_if(
            mem_fun_pair.first, mem_fun_pair.second, [](const auto& mf) {
                return mf.name() == std::string("fun1");
            });

        REQUIRE(found != mem_fun_pair.second);

        auto res = test_obj.call_member_function(*found);

        REQUIRE(res.type().name() == std::string("void"));
        REQUIRE(
            tmfapi_space::static_value_cast<tmfapi1>(test_obj).public_value ==
            100);
    }

    SECTION("find fun2 and call it")
    {
        auto found = std::find_if(
            mem_fun_pair.first, mem_fun_pair.second, [](const auto& mf) {
                return mf.name() == std::string("fun2");
            });

        REQUIRE(found != mem_fun_pair.second);

        shadow::variable args[] = {tmfapi_space::static_create<int>(55)};

        auto res = test_obj.call_member_function(
            *found, std::begin(args), std::end(args));

        REQUIRE(res.type().name() == std::string("void"));
        REQUIRE(
            tmfapi_space::static_value_cast<tmfapi1>(test_obj).public_value ==
            55);
    }

    SECTION("find fun3 and call it")
    {
        auto found = std::find_if(
            mem_fun_pair.first, mem_fun_pair.second, [](const auto& mf) {
                return mf.name() == std::string("fun3");
            });

        REQUIRE(found != mem_fun_pair.second);

        std::vector<shadow::variable> empty_args;

        auto res = test_obj.call_member_function(
            *found, empty_args.begin(), empty_args.end());

        REQUIRE(res.type().name() == std::string("int"));
        REQUIRE(tmfapi_space::static_value_cast<int>(res) == 23);
        REQUIRE(
            tmfapi_space::static_value_cast<tmfapi1>(test_obj).public_value ==
            20);
    }

    SECTION("find fun4 and call it")
    {
        auto found = std::find_if(
            mem_fun_pair.first, mem_fun_pair.second, [](const auto& mf) {
                return mf.name() == std::string("fun4");
            });

        REQUIRE(found != mem_fun_pair.second);

        std::vector<shadow::variable> args{
            tmfapi_space::static_create<int>(30)};

        auto res =
            test_obj.call_member_function(*found, args.begin(), args.end());

        REQUIRE(res.type().name() == std::string("int"));
        REQUIRE(tmfapi_space::static_value_cast<int>(res) == 60);
        REQUIRE(
            tmfapi_space::static_value_cast<tmfapi1>(test_obj).public_value ==
            20);
    }
}
