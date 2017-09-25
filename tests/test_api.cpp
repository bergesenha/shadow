#include "catch.hpp"

#include <shadow.hpp>

#include <algorithm>

void
fun1(void)
{
}

// value parameter
void
fun2(int i)
{
}

namespace test_space
{
REGISTER_TYPE_BEGIN()
REGISTER_TYPE_END()

REGISTER_FREE_FUNCTION(fun1)
REGISTER_FREE_FUNCTION(fun2)

SHADOW_INIT()
}


TEST_CASE("get iterator pair to all available functions",
          "[reflection_manager]")
{
    auto ff_pair = test_space::manager.free_functions();

    REQUIRE(ff_pair.second - ff_pair.first == 2);

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

            auto res = fun1_refl();

            REQUIRE(res.type().name() == std::string("void"));
        }
    }
}
