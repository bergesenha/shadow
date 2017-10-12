#include "catch.hpp"

#include <shadow.hpp>
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
        }
    }
}
