#include "catch.hpp"

#include <reflection_manager.hpp>
#include <iterator>


TEST_CASE("test type_iterator_", "[type_iterator_]")
{
    constexpr shadow::type_info type_info_array[] = {
        {"type1", 1}, {"type2", 2}, {"type3", 3}, {"type4", 4},
    };

}
