#include "catch.hpp"
#include <any_reference.hpp>


TEST_CASE("create any_reference from native types", "[any_reference]")
{
    int i = 20;
    const int j = 10;

    shadow::any_reference a = i;
    shadow::any_reference b = j;

    SECTION("get referred to value")
    {
        auto& ii = a.get<int>();

        CHECK(ii == 20);
        CHECK(&ii == &i);
    }
}
