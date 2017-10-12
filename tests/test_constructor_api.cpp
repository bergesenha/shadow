#include "catch.hpp"

#include <shadow.hpp>
#include <vector>
#include <iostream>


struct tca1
{
    int i;
    char c;
};

class tca2
{
public:
    tca2() : c_('a')
    {
    }

    tca2(char c) : c_(std::move(c))
    {
    }

    char
    get_c() const
    {
        return c_;
    }

private:
    char c_;
};

class tca3
{
public:
    tca3(const int& i) : i_(i)
    {
    }

    int
    get_i() const
    {
        return i_;
    }

private:
    int i_;
};


namespace tca_space
{
REGISTER_TYPE_BEGIN()
REGISTER_TYPE(tca1)
REGISTER_TYPE(tca2)
REGISTER_TYPE(tca3)
REGISTER_TYPE_END()

REGISTER_CONSTRUCTOR(tca1, int, char)

REGISTER_CONSTRUCTOR(tca2)
REGISTER_CONSTRUCTOR(tca2, char)

REGISTER_CONSTRUCTOR(tca3, const int&)

SHADOW_INIT()
}


TEST_CASE("get all types", "[reflection_manager]")
{
    auto types_pair = tca_space::manager.types();

    SECTION("find double type")
    {
        auto found = std::find_if(
            types_pair.first, types_pair.second, [](const auto& tp) {
                return tp.name() == std::string("double");
            });

        REQUIRE(found != types_pair.second);

        SECTION("get all constructors for double")
        {
            auto constr_pair = tca_space::manager.constructors_by_type(*found);

            REQUIRE(std::distance(constr_pair.first, constr_pair.second) == 2);

            SECTION("find constructor taking 0 arguments")
            {
                auto found_constr = std::find_if(
                    constr_pair.first, constr_pair.second, [](const auto& ctr) {
                        return ctr.num_parameters() == 0;
                    });

                REQUIRE(found_constr != constr_pair.second);

                SECTION("constructor double with 0 args")
                {
                    // construct with no arg overload
                    auto obj = tca_space::manager.construct(*found_constr);

                    // construct with overload taking iterators to args
                    std::vector<shadow::variable> args;
                    auto obj2 = tca_space::manager.construct(
                        *found_constr, args.begin(), args.end());

                    REQUIRE(obj.type().name() == std::string("double"));
                    REQUIRE(obj2.type().name() == std::string("double"));
                    REQUIRE(tca_space::static_value_cast<double>(obj) ==
                            Approx(0.0));
                    REQUIRE(tca_space::static_value_cast<double>(obj2) ==
                            Approx(0.0));
                }
            }

            SECTION("find constructor taking 1 argument")
            {
                auto found_constr = std::find_if(
                    constr_pair.first, constr_pair.second, [](const auto& ctr) {
                        return ctr.num_parameters() == 1;
                    });

                REQUIRE(found_constr != constr_pair.second);

                SECTION("constructor double with 1 arg")
                {
                    // construct with overload taking iterators to args
                    std::vector<shadow::variable> args{
                        tca_space::static_create<double>(20.0)};
                    auto obj = tca_space::manager.construct(
                        *found_constr, args.begin(), args.end());

                    REQUIRE(obj.type().name() == std::string("double"));
                    REQUIRE(tca_space::static_value_cast<double>(obj) ==
                            Approx(20.0));
                }
            }
        }
    }


    SECTION("find tca1 type")
    {
        auto found = std::find_if(
            types_pair.first, types_pair.second, [](const auto& tp) {
                return tp.name() == std::string("tca1");
            });

        REQUIRE(found != types_pair.second);

        SECTION("get all constructors for tca1")
        {
            auto constr_pair = tca_space::manager.constructors_by_type(*found);

            REQUIRE(std::distance(constr_pair.first, constr_pair.second) == 1);

            SECTION("find constructor taking two args")
            {
                auto found_constr = std::find_if(
                    constr_pair.first, constr_pair.second, [](const auto& ctr) {
                        return ctr.num_parameters() == 2;
                    });

                REQUIRE(found_constr != constr_pair.second);

                SECTION("construct a tca1 with two arguments")
                {
                    std::vector<shadow::variable> args{
                        tca_space::static_create<int>(10),
                        tca_space::static_create<char>('c')};

                    auto obj = tca_space::manager.construct(
                        *found_constr, args.begin(), args.end());

                    REQUIRE(obj.type().name() == std::string("tca1"));
                    REQUIRE(tca_space::static_value_cast<tca1>(obj).i == 10);
                    REQUIRE(tca_space::static_value_cast<tca1>(obj).c == 'c');
                }
            }
        }
    }


    SECTION("find tca2 type")
    {
        auto found = std::find_if(
            types_pair.first, types_pair.second, [](const auto& tp) {
                return tp.name() == std::string("tca2");
            });

        REQUIRE(found != types_pair.second);

        SECTION("get all constructors for tca2")
        {
            auto constr_pair = tca_space::manager.constructors_by_type(*found);

            REQUIRE(std::distance(constr_pair.first, constr_pair.second) == 2);

            SECTION("find default constructor for tca2")
            {
                auto found_constr = std::find_if(
                    constr_pair.first, constr_pair.second, [](const auto& ctr) {
                        return ctr.num_parameters() == 0;
                    });

                REQUIRE(found_constr != constr_pair.second);

                SECTION("construct tca2 with default constructor")
                {
                    auto obj = tca_space::manager.construct(*found_constr);

                    REQUIRE(obj.type().name() == std::string("tca2"));
                    REQUIRE(tca_space::static_value_cast<tca2>(obj).get_c() ==
                            'a');
                }
            }

            SECTION("find constructor for tca2 taking one argument")
            {
                auto found_constr = std::find_if(
                    constr_pair.first, constr_pair.second, [](const auto& ctr) {
                        return ctr.num_parameters() == 1;
                    });

                REQUIRE(found_constr != constr_pair.second);

                SECTION("construct tca2 with one arg constructor")
                {
                    std::vector<shadow::variable> args{
                        tca_space::static_create<char>('w')};

                    auto obj = tca_space::manager.construct(
                        *found_constr, args.begin(), args.end());

                    REQUIRE(obj.type().name() == std::string("tca2"));
                    REQUIRE(tca_space::static_value_cast<tca2>(obj).get_c() ==
                            'w');
                }
            }
        }
    }


    SECTION("find tca3 type")
    {
        auto found = std::find_if(
            types_pair.first, types_pair.second, [](const auto& tp) {
                return tp.name() == std::string("tca3");
            });

        REQUIRE(found != types_pair.second);

        SECTION("get all constructors for tca3")
        {
            auto constr_pair = tca_space::manager.constructors_by_type(*found);

            REQUIRE(std::distance(constr_pair.first, constr_pair.second) == 1);

            SECTION("find constructor taking one arg")
            {
                auto found_constr = std::find_if(
                    constr_pair.first, constr_pair.second, [](const auto& ctr) {
                        return ctr.num_parameters() == 1;
                    });

                REQUIRE(found_constr != constr_pair.second);

                SECTION("construct tca3 with one argument")
                {
                    std::vector<shadow::variable> args{
                        tca_space::static_create<int>(100)};

                    auto obj = tca_space::manager.construct(
                        *found_constr, args.begin(), args.end());

                    REQUIRE(obj.type().name() == std::string("tca3"));
                }
            }
        }
    }
}
