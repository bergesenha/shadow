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
    REQUIRE(tct1_space::get_held_value<int>(anint) == 23);

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

        SECTION("construct a tct1_class from tct1_space2 with int from "
                "tct1_space namespace")
        {
            std::vector<shadow::object> args{anint};

            auto res = tct1_space2::manager.construct_object(
                *found, args.begin(), args.end());

            REQUIRE(res.type().name() == std::string("tct1_class"));
            REQUIRE(tct1_space2::get_held_value<tct1_class>(res).get_i() == 23);
            REQUIRE(tct1_space::get_held_value<tct1_class>(res).get_i() == 23);

            SECTION("find all free functions from tct1_space")
            {
                auto ffs = tct1_space::manager.free_functions();

                REQUIRE(std::distance(ffs.first, ffs.second) > 0);

                SECTION("find free function taking tct1_class as argument")
                {
                    auto ff_found =
                        std::find_if(ffs.first, ffs.second, [](const auto& ff) {

                            auto param_types =
                                tct1_space::manager
                                    .free_function_parameter_types(ff);

                            return (std::distance(param_types.first,
                                                  param_types.second) == 1) &&
                                   (param_types.first->name() ==
                                    std::string("tct1_class"));

                        });

                    REQUIRE(ff_found != ffs.second);

                    SECTION("call free function")
                    {
                        auto ff_res = tct1_space::manager.call_free_function(
                            *ff_found, &res, &res + 1);

                        REQUIRE(ff_res.type().name() == std::string("int"));
                        REQUIRE(tct1_space::get_held_value<int>(ff_res) == 23);
                    }
                }
            }
        }
    }

    SECTION("find all type conversions available")
    {
        auto conversions = tct1_space::manager.conversions();

        SECTION("find conversion from int to float")
        {
            auto found = std::find_if(
                conversions.first, conversions.second, [](const auto& cv) {
                    auto conv_types = tct1_space::manager.conversion_types(cv);

                    return conv_types.first.name() == std::string("int") &&
                           conv_types.second.name() == std::string("float");
                });

            REQUIRE(found != conversions.second);

            SECTION("convert int to float")
            {
                auto res = tct1_space::manager.convert(*found, anint);

                REQUIRE(res.type().name() == std::string("float"));
                REQUIRE(tct1_space::get_held_value<float>(res) ==
                        Approx(23.0f));
            }
        }
    }
}
