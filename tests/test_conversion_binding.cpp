#include "catch.hpp"

#include <string>

#include <reflection_binding.hpp>


struct explicitly_convertible
{
    template <class T>
    explicit explicitly_convertible(T)
    {
    }
};

TEST_CASE("test conversion bindings between anys of different types",
          "[generic_conversion_bind_point]")
{
    auto int_to_float =
        &shadow::conversion_detail::generic_conversion_bind_point<float, int>;

    auto intptr_to_voidstar =
        &shadow::conversion_detail::generic_conversion_bind_point<void*, int*>;


    shadow::any anint = 23;
    int i = 300;
    int* iptr = &i;

    shadow::any anintptr = iptr;

    auto afloat = int_to_float(anint);

    auto avoidstar = intptr_to_voidstar(anintptr);

    REQUIRE(afloat.get<float>() == Approx(23.0f));
    REQUIRE(*((int*)(avoidstar.get<void*>())) == 300);
}
