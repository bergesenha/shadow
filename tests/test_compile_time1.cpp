#include "catch.hpp"

#include <shadow.hpp>


class tct1_class
{
public:
    tct1_class() = default;

    tct1_class(int i) : i_(i)
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


int
extract_i(const tct1_class& a)
{
    return a.get_i();
}


namespace tct1_space
{
REGISTER_TYPE_BEGIN()
REGISTER_TYPE(tct1_class)
REGISTER_TYPE_END()

REGISTER_CONSTRUCTOR(tct1_class)

REGISTER_FREE_FUNCTION(extract_i)

SHADOW_INIT()
}


namespace tct1_space2
{
REGISTER_TYPE_BEGIN()
REGISTER_TYPE(tct1_class)
REGISTER_TYPE_END()

REGISTER_CONSTRUCTOR(tct1_class, int)

REGISTER_MEMBER_FUNCTION(tct1_class, get_i)

SHADOW_INIT()
}

TEST_CASE("create an int using static_construct", "[static_construct]")
{
    auto anint = tct1_space::static_construct<int>(23);

    REQUIRE(anint.type().name() == std::string("int"));

    SECTION(
        "find constructor for tct1_class from tct1_space2 namespace manager")
    {
        auto constructors = tct1_space2::manager.constructors();

        auto found = std::find_if(
            constructors.first, constructors.second, [](const auto& ct) {
                const auto name =
                    tct1_space2::manager.constructor_type(ct).name();

                return name == std::string("tct1_class");
            });

        auto paramtypes =
            tct1_space2::manager.constructor_parameter_types(*found);

        REQUIRE(found != constructors.second);
        REQUIRE(std::distance(paramtypes.first, paramtypes.second) == 1);
        REQUIRE(paramtypes.first->name() == std::string("int"));

        SECTION("construct a tct1_class with int from tct1_space namespace")
        {
            std::vector<shadow::object> args{anint};

            auto res = tct1_space2::manager.construct_object(
                *found, args.begin(), args.end());

            REQUIRE(res.type().name() == std::string("tct1_class"));
        }
    }
}
