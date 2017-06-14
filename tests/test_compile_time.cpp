#include "catch.hpp"
#include <type_traits>
#include <compile_time.hpp>
#include <string>
REGISTER_TYPE(int)
REGISTER_TYPE(float)
REGISTER_TYPE(double)


TEST_CASE("", "")
{
    typedef shadow::generate_vector_of_strings<
        metamusil::t_list::type_list<shadow::compile_time_type_info<5>,
                                     shadow::compile_time_type_info<6>,
                                     shadow::compile_time_type_info<7>>>
        stringsholder;

    auto length = std::extent<decltype(stringsholder::value)>::value;

    std::string first(stringsholder::value[0]);
    std::string second(stringsholder::value[1]);
    std::string third(stringsholder::value[2]);

    REQUIRE(length == 3);
    REQUIRE(first == std::string("int"));
    REQUIRE(second == std::string("float"));
    REQUIRE(third == std::string("double"));
}
