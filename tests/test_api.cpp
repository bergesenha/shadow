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

namespace test_space
{
REGISTER_TYPE_BEGIN()
REGISTER_TYPE_END()

REGISTER_FREE_FUNCTION(fun1)
REGISTER_FREE_FUNCTION(fun2)
REGISTER_FREE_FUNCTION(fun3)
REGISTER_FREE_FUNCTION(fun4)

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
}
