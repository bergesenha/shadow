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
}
