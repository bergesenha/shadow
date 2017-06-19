#include "catch.hpp"
#include <type_traits>
#include <compile_time.hpp>
#include <string>


namespace refl_space_1
{
REGISTER_TYPE_BEGIN()
REGISTER_TYPE_END()
}


TEST_CASE("No types registered", "[REGISTER_TYPE]")
{
    constexpr auto range_len =
        metamusil::int_seq::length_v<refl_space_1::type_line_range>;

    constexpr auto compile_time_infos_len = metamusil::t_list::length_v<
        refl_space_1::instantiated_compile_time_infos>;

    constexpr auto type_len =
        metamusil::t_list::length_v<refl_space_1::type_universe>;

    REQUIRE(range_len == 0);
    REQUIRE(compile_time_infos_len == 0);
    REQUIRE(refl_space_1::type_name_array_holder::value == nullptr);
    REQUIRE(refl_space_1::type_info_array_holder::value == nullptr);
    REQUIRE(type_len == 0);
}
