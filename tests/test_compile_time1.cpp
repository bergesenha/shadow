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


namespace refl_space_2
{
class myclass
{
public:
private:
    double d_;
};


REGISTER_TYPE_BEGIN()
REGISTER_TYPE(myclass)
REGISTER_TYPE_END()
}


TEST_CASE("one type registered", "[REGISTER_TYPE]")
{
    constexpr auto range_len = refl_space_2::type_line_range::size();
    constexpr auto compile_time_infos_len = metamusil::t_list::length_v<
        refl_space_2::instantiated_compile_time_infos>;
    constexpr auto type_info_array_length = std::extent<decltype(
        refl_space_2::type_info_array_holder::value)>::value;

    constexpr auto type_name_array_len = std::extent<decltype(
        refl_space_2::type_name_array_holder::value)>::value;

    std::string name = refl_space_2::type_info_array_holder::value[0].name;

    REQUIRE(range_len == 1);
    REQUIRE(compile_time_infos_len == 1);
    REQUIRE(type_name_array_len == 1);
    REQUIRE(type_info_array_length == 1);
    REQUIRE(name == std::string("myclass"));
}
