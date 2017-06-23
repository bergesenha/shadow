#include "catch.hpp"

#include <reflection_manager.hpp>
#include <iterator>


TEST_CASE("test type_iterator_", "[type_iterator_]")
{
    constexpr shadow::type_info type_info_array[] = {
        {"type1", 1}, {"type2", 2}, {"type3", 3}, {"type4", 4},
    };

    ////////////////////////////////////////////////////////////////////////////
    // Concepts

    SECTION("Construct type_iterator_ with pointer to type_info")
    {
        shadow::reflection_manager::type_iterator it1(
            std::begin(type_info_array));

        SECTION("pre-increment type_iterator_")
        {
            ++it1;
        }
    }
}
