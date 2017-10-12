#include "catch.hpp"

#include <list>
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

    void
    fun5(int& i)
    {
        i = 20000;
    }

    double
    fun6(const int& i, const double& j) const
    {
        return i * j;
    }

    int
    fun7(const int* i) const
    {
        return *i + 2;
    }

    void
    fun8(int* i)
    {
        *i = public_value;
    }

    void
    fun9(int& i)
    {
        i *= 2;
    }

    void
    fun9(double& d)
    {
        d *= 2.0;
    }

    void
    fun9(char* c)
    {
        *c += 2;
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
REGISTER_MEMBER_FUNCTION(tmfapi1, fun5)
REGISTER_MEMBER_FUNCTION(tmfapi1, fun6)
REGISTER_MEMBER_FUNCTION(tmfapi1, fun7)
REGISTER_MEMBER_FUNCTION(tmfapi1, fun8)

REGISTER_MEMBER_FUNCTION_EXPLICIT(tmfapi1, fun9, void, int&)
REGISTER_MEMBER_FUNCTION_EXPLICIT(tmfapi1, fun9, void, double&)
REGISTER_MEMBER_FUNCTION_EXPLICIT(tmfapi1, fun9, void, char*)

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

    SECTION("find fun5 and call it")
    {
        auto found = std::find_if(
            mem_fun_pair.first, mem_fun_pair.second, [](const auto& mf) {
                return mf.name() == std::string("fun5");
            });

        REQUIRE(found != mem_fun_pair.second);

        auto arg = tmfapi_space::static_create<int>();

        auto res = test_obj.call_member_function(*found, &arg, &arg + 1);

        REQUIRE(res.type().name() == std::string("void"));
        REQUIRE(tmfapi_space::static_value_cast<int>(arg) == 20000);
    }

    SECTION("find fun6 and call it")
    {
        auto found = std::find_if(
            mem_fun_pair.first, mem_fun_pair.second, [](const auto& mf) {
                return mf.name() == std::string("fun6");
            });

        REQUIRE(found != mem_fun_pair.second);

        std::list<shadow::variable> args{
            tmfapi_space::static_create<int>(2),
            tmfapi_space::static_create<double>(10.0)};

        auto res =
            test_obj.call_member_function(*found, args.begin(), args.end());

        REQUIRE(res.type().name() == std::string("double"));
        REQUIRE(tmfapi_space::static_value_cast<double>(res) == Approx(20.0));
        REQUIRE(tmfapi_space::static_value_cast<int>(args.front()) == 2);
        REQUIRE(tmfapi_space::static_value_cast<double>(args.back()) == 10.0);
    }


    SECTION("find fun7 and call it")
    {
        auto found = std::find_if(
            mem_fun_pair.first, mem_fun_pair.second, [](const auto& mf) {
                return mf.name() == std::string("fun7");
            });

        REQUIRE(found != mem_fun_pair.second);

        shadow::variable args[] = {tmfapi_space::static_create<int>(100)};

        auto res = test_obj.call_member_function(
            *found, std::begin(args), std::end(args));

        REQUIRE(res.type().name() == std::string("int"));
        REQUIRE(tmfapi_space::static_value_cast<int>(res) == 102);
        REQUIRE(tmfapi_space::static_value_cast<int>(args[0]) == 100);
    }

    SECTION("find fun8 and call it")
    {
        auto found = std::find_if(
            mem_fun_pair.first, mem_fun_pair.second, [](const auto& mf) {
                return mf.name() == std::string("fun8");
            });

        REQUIRE(found != mem_fun_pair.second);

        shadow::variable args[] = {tmfapi_space::static_create<int>()};

        auto res = test_obj.call_member_function(
            *found, std::begin(args), std::end(args));

        REQUIRE(res.type().name() == std::string("void"));
        REQUIRE(tmfapi_space::static_value_cast<int>(args[0]) == 20);
        REQUIRE(
            tmfapi_space::static_value_cast<tmfapi1>(test_obj).public_value ==
            20);
    }

    SECTION("find fun9 with int& param")
    {
        auto found = std::find_if(
            mem_fun_pair.first, mem_fun_pair.second, [](const auto& mf) {
                return mf.name() == std::string("fun9") &&
                       mf.parameter_types().first->name() == std::string("int");
            });

        REQUIRE(found != mem_fun_pair.second);

        SECTION("call it")
        {
            auto anint = tmfapi_space::static_create<int>(22);

            test_obj.call_member_function(found, &anint, &anint + 1);

            REQUIRE(tmfapi_space::static_value_cast<int>(anint) == 44);
        }
    }

    SECTION("find fun9 with double& param")
    {
        auto found = std::find_if(
            mem_fun_pair.first, mem_fun_pair.second, [](const auto& mf) {
                return mf.name() == std::string("fun9") &&
                       mf.parameter_types().first->name() ==
                           std::string("double");
            });

        REQUIRE(found != mem_fun_pair.second);

        SECTION("call it")
        {
            auto adouble = tmfapi_space::static_create<double>(1.5);

            test_obj.call_member_function(found, &adouble, &adouble + 1);

            REQUIRE(tmfapi_space::static_value_cast<double>(adouble) ==
                    Approx(3.0));
        }
    }

    SECTION("find fun9 with char* param")
    {
        auto found = std::find_if(
            mem_fun_pair.first, mem_fun_pair.second, [](const auto& mf) {
                return mf.name() == std::string("fun9") &&
                       mf.parameter_types().first->name() ==
                           std::string("char");
            });

        REQUIRE(found != mem_fun_pair.second);

        SECTION("call it")
        {
            auto achar = tmfapi_space::static_create<char>('a');

            test_obj.call_member_function(found, &achar, &achar + 1);

            REQUIRE(tmfapi_space::static_value_cast<char>(achar) == 'c');
        }
    }
}
