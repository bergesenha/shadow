#include "catch.hpp"

#include <string>

#include <reflection_binding.hpp>


struct notconvertible
{
};

TEST_CASE("test conversion bindings between anys of different types",
          "[generic_conversion_bind_point]")
{
    auto int_to_float =
        &shadow::conversion_detail::generic_conversion_bind_point<float, int>;
}
