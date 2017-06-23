#include "catch.hpp"

#include <reflection_manager.hpp>
#include <iterator>


TEST_CASE("test info_iterator_", "[info_iterator_]")
{
    constexpr shadow::type_info type_info_array[] = {
        {"type1", 1}, {"type2", 2}, {"type3", 3}, {"type4", 4},
    };

    ////////////////////////////////////////////////////////////////////////////
    // Concepts

    SECTION("Construct info_iterator_'s with pointers to type_info in array")
    {
        shadow::reflection_manager::type_iterator it_begin(
            std::begin(type_info_array));

        shadow::reflection_manager::type_iterator it_end(
            std::end(type_info_array));

        SECTION("copy construct an info_iterator_ with it_begin")
        {
            auto copy_constructed_it = it_begin;

            // copy_constructed_it should be equivalent to it_begin

            SECTION("move construct an info_iterator_ with copy_constructed_it")
            {
                auto move_constructed_it = std::move(copy_constructed_it);

                // move_constructed_it should be equivalent to it_begin
            }

            SECTION("default construct info_iterator_")
            {
                shadow::reflection_manager::type_iterator
                    default_constructed_it1;

                shadow::reflection_manager::type_iterator
                    default_constructed_it2{};

                shadow::reflection_manager::type_iterator();

                shadow::reflection_manager::type_iterator{};

                SECTION("copy assign it_begin to default_constructed_it1")
                {
                    default_constructed_it1 = it_begin;

                    // default_constructed_it1 should be equivalent to
                    // copy_constructied_it

                    // it_begin should be unchanged
                }

                SECTION("move assign it_begin to default_constructed_it1")
                {
                    default_constructed_it1 = std::move(it_begin);

                    // default_constructed_it1 should be equivalent to
                    // copy_constructied_it
                }
            }

            SECTION("swap copy_constructed_it and it_end")
            {
                using std::swap;
                swap(copy_constructed_it, it_end);

                // it_end should be equivalent to it_begin
            }
        }

        SECTION("pre-increment info_iterator_")
        {
            ++it_begin;
        }
    }

    SECTION("inspect iterator_traits of info_iterator_")
    {
        auto value_type_same = std::is_same<
            std::iterator_traits<
                shadow::reflection_manager::type_iterator>::value_type,
            shadow::type>::value;

        auto difference_type_same = std::is_same<
            std::iterator_traits<
                shadow::reflection_manager::type_iterator>::difference_type,
            std::iterator_traits<const shadow::type_info*>::difference_type>::
            value;

        REQUIRE(value_type_same);
        REQUIRE(difference_type_same);
    }
}
