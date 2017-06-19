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

    auto int_to_explicitly_convertible =
        &shadow::conversion_detail::
            generic_conversion_bind_point<explicitly_convertible, int>;

    shadow::any anint = 23;

    auto afloat = int_to_float(anint);

    REQUIRE(afloat.get<float>() == Approx(23.0f));
}
