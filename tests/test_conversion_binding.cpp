#include "catch.hpp"

#include <string>

#include <reflection_binding.hpp>


TEST_CASE("test conversion bindings between anys of different types",
          "[generic_conversion_bind_point]")
{
    auto float_to_int =
        &shadow::conversion_detail::generic_conversion_bind_point<int, float>;

    auto int_to_float =
        &shadow::conversion_detail::generic_conversion_bind_point<float, int>;

    auto float_to_string =
        &shadow::conversion_detail::generic_conversion_bind_point<std::string,
                                                                  float>;
    auto int_to_string =
        &shadow::conversion_detail::generic_conversion_bind_point<std::string,
                                                                  int>;
    auto string_to_int =
        &shadow::conversion_detail::generic_conversion_bind_point<int,
                                                                  std::string>;
    auto string_to_float =
        &shadow::conversion_detail::generic_conversion_bind_point<float,
                                                                  std::string>;


    shadow::any anint = 10;
    shadow::any afloat = 2.5f;
    shadow::any astring = std::string("234.3");

    SECTION("convert any of float to int")
    {
        auto res = float_to_int(afloat);

        REQUIRE(res.get<int>() == 2);
    }

    SECTION("convert an int to float")
    {
        auto res = int_to_float(anint);

        REQUIRE(res.get<float>() == Approx(10.0f));
    }

    SECTION("convert an int to std::string")
    {
        auto res = int_to_string(anint);

        REQUIRE(res.get<std::string>() == std::to_string(10));
    }

    SECTION("convert a float to std::string")
    {
        auto res = float_to_string(afloat);

        REQUIRE(res.get<std::string>() == std::to_string(2.5f));
    }

    SECTION("convert a string to int")
    {
        auto res = string_to_int(astring);

        REQUIRE(res.get<int>() == 234);
    }

    SECTION("convert a string to float")
    {
        auto res = string_to_float(astring);

        REQUIRE(res.get<float>() == Approx(234.3));
    }
}
