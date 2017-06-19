#include "catch.hpp"
#include <type_traits>
#include <compile_time.hpp>
#include <string>


namespace myspace
{
REGISTER_TYPE_BEGIN()
REGISTER_TYPE(int)
REGISTER_TYPE(float)
REGISTER_TYPE(double)
REGISTER_TYPE_END()
}


TEST_CASE("", "")
{
}
