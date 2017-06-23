#include "catch.hpp"

#include <reflection_manager.hpp>
#include <iterator>


TEST_CASE("test info_iterator_ with const type", "[info_iterator_]")
{
    constexpr shadow::type_info type_info_array[] = {
        {"type1", 1}, {"type2", 2}, {"type3", 3}, {"type4", 4},
    };


    SECTION("Construct info_iterator_'s with pointers to type_info in array")
    {
        shadow::reflection_manager::const_type_iterator it_begin(
            std::begin(type_info_array));

        shadow::reflection_manager::const_type_iterator it_end(
            std::end(type_info_array));

        SECTION("self comparison")
        {
            REQUIRE(it_begin == it_begin);
            REQUIRE(it_end == it_end);
        }

        SECTION("it_end and it_begin should not compare equal")
        {
            REQUIRE(it_begin != it_end);
            REQUIRE(!(it_begin == it_end));
        }

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
                shadow::reflection_manager::const_type_iterator
                    default_constructed_it1;

                shadow::reflection_manager::const_type_iterator
                    default_constructed_it2{};

                shadow::reflection_manager::const_type_iterator();

                shadow::reflection_manager::const_type_iterator{};

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
                REQUIRE(it_end == it_begin);
            }
        }

        SECTION("pre-increment info_iterator_")
        {
            auto res = ++it_begin;

            REQUIRE(it_begin->name() == std::string("type2"));
            REQUIRE(res->name() == std::string("type2"));

            SECTION("pre-decrement to get back")
            {
                --it_begin;

                REQUIRE(it_begin->name() == std::string("type1"));
            }

            SECTION("post-decrement to get back")
            {
                it_begin--;

                REQUIRE(it_begin->name() == std::string("type1"));
            }

            SECTION("dereference and post-decrement")
            {
                auto res2 = *it_begin--;

                REQUIRE(res2.name() == std::string("type2"));
                REQUIRE(it_begin->name() == std::string("type1"));
            }
        }

        SECTION("post-increment and dereference in one expression")
        {
            auto res = *it_begin++;

            REQUIRE(res->name() == std::string("type1"));
            REQUIRE(it_begin->name() == std::string("type2"));
        }

        SECTION("dereference it_begin")
        {
            auto front = *it_begin;

            REQUIRE(front.name() == std::string("type1"));
        }

        SECTION("increment by adding integer to it_begin")
        {
            it_begin += 4;

            REQUIRE(it_begin == it_end);
        }

        SECTION("add number to it_begin")
        {
            auto res = it_begin + 3;

            REQUIRE(res->name() == std::string("type4"));
        }

        SECTION("add it_begin to number")
        {
            auto res = 3 + it_begin;

            REQUIRE(res->name() == std::string("type4"));
        }

        SECTION("decrement by -=")
        {
            it_end -= 1;

            REQUIRE(it_end->name() == std::string("type4"));
        }
    }


    SECTION("inspect iterator_traits of info_iterator_")
    {
        auto value_type_same = std::is_same<
            std::iterator_traits<
                shadow::reflection_manager::const_type_iterator>::value_type,
            const shadow::type>::value;

        auto difference_type_same = std::is_same<
            std::iterator_traits<shadow::reflection_manager::
                                     const_type_iterator>::difference_type,
            std::iterator_traits<const shadow::type_info*>::difference_type>::
            value;

        auto reference_same = std::is_same<
            std::iterator_traits<
                shadow::reflection_manager::const_type_iterator>::reference,
            const shadow::type>::value;

        auto pointer_same = std::is_same<
            std::iterator_traits<
                shadow::reflection_manager::const_type_iterator>::pointer,
            const shadow::type>::value;

        auto category_same = std::is_same<
            std::iterator_traits<shadow::reflection_manager::
                                     const_type_iterator>::iterator_category,
            std::iterator_traits<const shadow::type_info*>::iterator_category>::
            value;

        REQUIRE(value_type_same);
        REQUIRE(difference_type_same);
        REQUIRE(reference_same);
        REQUIRE(pointer_same);
        REQUIRE(category_same);
    }
}
