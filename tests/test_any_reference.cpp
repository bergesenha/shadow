#include "catch.hpp"
#include <any_reference.hpp>


TEST_CASE("create any_reference from any", "[any_reference]")
{
    using shadow::any;
    using shadow::any_reference;

    any a = 10;

    any_reference ra(a);

    CHECK(ra.get<int>() == 10);
    CHECK(&ra.get<int>() == &a.get<int>());
}

