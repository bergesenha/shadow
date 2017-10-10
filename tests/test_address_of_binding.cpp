#include "catch.hpp"

#include <reflection_binding.hpp>
#include <any.hpp>


TEST_CASE("test generic_address_of_bind_point",
          "[generic_address_of_bind_point]")
{
    shadow::any anint(20);

    int* intpointer = &(anint.get<int>());

    REQUIRE(anint.get<int>() == 20);
    REQUIRE(*intpointer == 20);


    SECTION("create an any containing pointer to int in anint")
    {
        auto anintpointer =
            shadow::address_of_detail::generic_address_of_bind_point<int>(
                anint);

        REQUIRE(anint.get<int>() == 20);
        REQUIRE(*anintpointer.get<int*>() == 20);
        REQUIRE(anintpointer.get<int*>() == intpointer);
        REQUIRE(*anintpointer.get<int*>() == anint.get<int>());

        SECTION("modify int through any containig pointer to it")
        {
            *anintpointer.get<int*>() = 10;


            REQUIRE(anint.get<int>() == 10);
            REQUIRE(*anintpointer.get<int*>() == anint.get<int>());
        }
    }
}
