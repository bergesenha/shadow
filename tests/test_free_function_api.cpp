#include "catch.hpp"

#include <shadow.hpp>

#include <algorithm>
#include <vector>

void
fun1(void)
{
}

// value parameter
void
fun2(int i)
{
}

int
fun3(const int& i)
{
    return i * 2;
}

void
fun4(int& i)
{
    i *= 2;
}

int
fun5(const int* i)
{
    return *i * 2;
}

void
fun6(int* i)
{
    *i *= 2;
}

void
fun7(int& i)
{
    i *= 2;
}

void
fun7(double& d)
{
    d *= 2.0;
}

void
fun7(char* c)
{
    *c += 2;
}

namespace test_space
{
REGISTER_TYPE_BEGIN()
REGISTER_TYPE_END()

REGISTER_FREE_FUNCTION(fun1)
REGISTER_FREE_FUNCTION(fun2)
REGISTER_FREE_FUNCTION(fun3)
REGISTER_FREE_FUNCTION(fun4)
REGISTER_FREE_FUNCTION(fun5)
REGISTER_FREE_FUNCTION(fun6)

REGISTER_FREE_FUNCTION_EXPLICIT(fun7, void, int&)
REGISTER_FREE_FUNCTION_EXPLICIT(fun7, void, double&)
REGISTER_FREE_FUNCTION_EXPLICIT(fun7, void, char*)

SHADOW_INIT()
}


TEST_CASE("get iterator pair to all available functions",
          "[reflection_manager]")
{
    auto ff_pair = test_space::manager.free_functions();


    SECTION("find fun1")
    {
        auto found =
            std::find_if(ff_pair.first, ff_pair.second, [](const auto& ff) {
                return ff.name() == std::string("fun1");
            });

        REQUIRE(found != ff_pair.second);

        SECTION("call fun1 through reflection system")
        {
            auto fun1_refl = *found;

            // call with no param overload
            auto res = fun1_refl();

            // call with regular overload
            std::vector<shadow::variable> noargs;
            auto res2 = fun1_refl(noargs.begin(), noargs.end());

            REQUIRE(res.type().name() == std::string("void"));
            REQUIRE(res2.type().name() == std::string("void"));
        }
    }


    SECTION("find fun2")
    {
        auto found =
            std::find_if(ff_pair.first, ff_pair.second, [](const auto& ff) {
                return ff.name() == std::string("fun2");
            });

        REQUIRE(found != ff_pair.second);

        SECTION("call fun2 through reflection system")
        {
            auto fun2_refl = *found;

            std::vector<shadow::variable> args{
                test_space::static_create<int>(23)};

            auto res = fun2_refl(args.begin(), args.end());
            REQUIRE(res.type().name() == std::string("void"));
            REQUIRE(test_space::static_value_cast<int>(args[0]) == 23);
        }
    }


    SECTION("find fun3")
    {
        auto found =
            std::find_if(ff_pair.first, ff_pair.second, [](const auto& ff) {
                using namespace std::string_literals;
                return ff.name() == "fun3"s;
            });

        REQUIRE(found != ff_pair.second);

        SECTION("call fun3 through reflection system")
        {
            auto fun3_refl = *found;

            std::vector<shadow::variable> args{
                test_space::static_create<int>(10)};

            auto res = fun3_refl(args.begin(), args.end());

            REQUIRE(res.type().name() == std::string("int"));
            REQUIRE(test_space::static_value_cast<int>(res) == 20);
            REQUIRE(test_space::static_value_cast<int>(args[0]) == 10);
        }
    }


    SECTION("find fun4")
    {
        auto found =
            std::find_if(ff_pair.first, ff_pair.second, [](const auto& ff) {
                using namespace std::string_literals;
                return ff.name() == "fun4"s;
            });

        REQUIRE(found != ff_pair.second);

        SECTION("call fun4 through reflection system")
        {
            auto fun4_refl = *found;

            std::vector<shadow::variable> args{
                test_space::static_create<int>(10)};

            auto res = fun4_refl(args.begin(), args.end());

            REQUIRE(res.type().name() == std::string("void"));
            REQUIRE(test_space::static_value_cast<int>(args[0]) == 20);
        }
    }

    SECTION("find fun5")
    {
        auto found =
            std::find_if(ff_pair.first, ff_pair.second, [](const auto& ff) {
                using namespace std::string_literals;
                return ff.name() == "fun5"s;
            });

        REQUIRE(found != ff_pair.second);

        SECTION("call fun5 through reflection system")
        {
            auto fun5_refl = *found;

            std::vector<shadow::variable> args{
                test_space::static_create<int>(10)};

            auto res = fun5_refl(args.begin(), args.end());

            REQUIRE(res.type().name() == std::string("int"));
            REQUIRE(test_space::static_value_cast<int>(res) == 20);
            REQUIRE(test_space::static_value_cast<int>(args[0]) == 10);
        }
    }

    SECTION("find fun6")
    {
        auto found =
            std::find_if(ff_pair.first, ff_pair.second, [](const auto& ff) {
                return ff.name() == std::string("fun6");
            });

        REQUIRE(found != ff_pair.second);

        SECTION("call fun6 through reflection system")
        {
            auto fun6_refl = *found;

            std::vector<shadow::variable> args{
                test_space::static_create<int>(10)};

            auto res = fun6_refl(args.begin(), args.end());

            REQUIRE(res.type().name() == std::string("void"));
            REQUIRE(test_space::static_value_cast<int>(args[0]) == 20);
        }
    }

    SECTION("find fun7 with int& param")
    {
        auto found =
            std::find_if(ff_pair.first, ff_pair.second, [](const auto& ff) {
                return ff.name() == std::string("fun7") &&
                       ff.parameter_types().first->name() == std::string("int");
            });

        REQUIRE(found != ff_pair.second);

        SECTION("call fun7(int&) through reflection system")
        {
            auto fun7_refl = *found;

            auto anint = test_space::static_create<int>(200);

            auto res = fun7_refl(&anint, &anint + 1);

            REQUIRE(res.type().name() == std::string("void"));
            REQUIRE(test_space::static_value_cast<int>(anint) == 400);
        }
    }

    SECTION("find fun7 with double& param")
    {
        auto found =
            std::find_if(ff_pair.first, ff_pair.second, [](const auto& ff) {
                return ff.name() == std::string("fun7") &&
                       ff.parameter_types().first->name() ==
                           std::string("double");
            });

        REQUIRE(found != ff_pair.second);

        SECTION("call fun7(double&) through reflection system")
        {
            auto fun7_refl = *found;

            auto adouble = test_space::static_create<double>(1.5);

            auto res = fun7_refl(&adouble, &adouble + 1);

            REQUIRE(res.type().name() == std::string("void"));
            REQUIRE(test_space::static_value_cast<double>(adouble) ==
                    Approx(3.0));
        }
    }

    SECTION("find fun7 with char* param")
    {
        auto found =
            std::find_if(ff_pair.first, ff_pair.second, [](const auto& ff) {
                return ff.name() == std::string("fun7") &&
                       ff.parameter_types().first->name() ==
                           std::string("char");
            });

        REQUIRE(found != ff_pair.second);

        SECTION("call fun7(char*) through reflection system")
        {
            auto fun7_refl = *found;

            auto achar = test_space::static_create<char>('a');

            auto res = fun7_refl(&achar, &achar + 1);

            REQUIRE(res.type().name() == std::string("void"));
            REQUIRE(test_space::static_value_cast<char>(achar) == 'c');
        }
    }
}
