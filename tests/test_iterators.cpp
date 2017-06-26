#include "catch.hpp"

#include <reflection_manager.hpp>
#include <iterator>


TEST_CASE("test info_iterator_ with const type", "[info_iterator_]")
{
    constexpr shadow::type_info type_info_array[] = {
        {"type1", 1}, {"type2", 2}, {"type3", 3}, {"type4", 4},
    };

    shadow::reflection_manager man;

    SECTION("Construct info_iterator_'s with pointers to type_info in array")
    {
        shadow::reflection_manager::const_type_iterator it_begin(
            std::begin(type_info_array), &man);

        shadow::reflection_manager::const_type_iterator it_end(
            std::end(type_info_array), &man);

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

        SECTION("subtract number from it_end")
        {
            auto res = it_end - 2;

            REQUIRE(res->name() == std::string("type3"));
        }

        SECTION("take the difference between end and begin")
        {
            auto res = it_end - it_begin;

            REQUIRE(res == 4);
        }

        SECTION("array access")
        {
            auto res1 = it_begin[0];
            auto res2 = it_begin[1];
            auto res3 = it_end[-1];

            REQUIRE(res1.name() == std::string("type1"));
            REQUIRE(res2.name() == std::string("type2"));
            REQUIRE(res3.name() == std::string("type4"));
        }

        SECTION("comparisons")
        {
            auto res1 = it_begin < it_end;
            auto res2 = it_end < it_begin;

            auto res3 = it_end > it_begin;
            auto res4 = it_begin > it_end;

            auto res5 = it_begin <= it_end;
            auto res6 = it_begin <= it_begin;

            auto res7 = it_end >= it_begin;
            auto res8 = it_end >= it_end;

            REQUIRE(res1);
            REQUIRE(!res2);
            REQUIRE(res3);
            REQUIRE(!res4);
            REQUIRE(res5);
            REQUIRE(res6);
            REQUIRE(res7);
            REQUIRE(res8);
        }
    }


    SECTION("inspect iterator_traits of info_iterator_")
    {
        auto value_type_same = std::is_same<
            std::iterator_traits<
                shadow::reflection_manager::const_type_iterator>::value_type,
            const shadow::reflection_manager::type>::value;

        auto difference_type_same = std::is_same<
            std::iterator_traits<shadow::reflection_manager::
                                     const_type_iterator>::difference_type,
            std::iterator_traits<const shadow::type_info*>::difference_type>::
            value;

        auto reference_same = std::is_same<
            std::iterator_traits<
                shadow::reflection_manager::const_type_iterator>::reference,
            const shadow::reflection_manager::type>::value;

        auto pointer_same = std::is_same<
            std::iterator_traits<
                shadow::reflection_manager::const_type_iterator>::pointer,
            const shadow::reflection_manager::type>::value;

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


TEST_CASE("test indexed_info_iterator_", "[indexed_info_iterator_]")
{
    typedef shadow::indexed_info_iterator_<const shadow::type_info,
                                           const shadow::type_>
        ititer;

    std::vector<shadow::type_info> info_vector{
        {"type0", 0}, {"type1", 2}, {"type2", 8}};

    std::vector<std::size_t> indices_vector{0, 2, 1};

    shadow::reflection_manager manager;

    SECTION("create an indexed_info_iterator_ to first element of info_vector")
    {
        ititer fst(0, indices_vector.data(), info_vector.data(), &manager);

        auto type0 = *fst;

        REQUIRE((*fst).name() == std::string("type0"));
        REQUIRE(fst->name() == std::string("type0"));
        REQUIRE(fst->size() == 0);
        REQUIRE(fst[0].name() == std::string("type0"));
        REQUIRE(fst[1].name() == std::string("type2"));
        REQUIRE(fst[2].name() == std::string("type1"));

        SECTION("preincrement fst")
        {
            auto res = ++fst;

            REQUIRE(fst->name() == std::string("type2"));
            REQUIRE(res->size() == 8);

            SECTION("postincrement fst")
            {
                auto res2 = fst++;

                REQUIRE(res2->size() == 8);
                REQUIRE(fst->name() == std::string("type1"));
            }
        }
    }
}
