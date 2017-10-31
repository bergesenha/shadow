#include "catch.hpp"

#include <shadow.hpp>
#include <sstream>
#include <vector>
#include <algorithm>


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

    void
    set_i(int i)
    {
        i_ = i;
    }

    void
    output_i(int& i)
    {
        i = i_;
    }

    void
    pointer_out(int* i)
    {
        *i = i_;
    }

private:
    int i_;
};

struct tct1_struct
{
    int i;
    double d;
};


struct tct1_struct2
{
    std::size_t index;
    tct1_struct the_struct;
};

int
extract_i(const tct1_class& a)
{
    return a.get_i();
}


int
mult(const int* i)
{
    return *i * 2;
}

void
modify(int* i)
{
    *i += 10;
}

void
triple(int& i)
{
    i *= 3;
}

namespace tct1_space
{
REGISTER_TYPE_BEGIN()
REGISTER_TYPE(tct1_class)
REGISTER_TYPE(tct1_struct)
REGISTER_TYPE_END()

REGISTER_CONSTRUCTOR(tct1_class)

REGISTER_CONSTRUCTOR(tct1_struct)
REGISTER_CONSTRUCTOR(tct1_struct, int)
REGISTER_CONSTRUCTOR(tct1_struct, int, double)

REGISTER_FREE_FUNCTION(extract_i)

REGISTER_MEMBER_VARIABLE(tct1_struct, d)
REGISTER_MEMBER_VARIABLE(tct1_struct, i)

SHADOW_INIT()
}


namespace tct1_space2
{
REGISTER_TYPE_BEGIN()
REGISTER_TYPE(tct1_class)
REGISTER_TYPE_END()

REGISTER_CONSTRUCTOR(tct1_class, int)

REGISTER_MEMBER_FUNCTION(tct1_class, get_i)
REGISTER_MEMBER_FUNCTION(tct1_class, set_i)
REGISTER_MEMBER_FUNCTION(tct1_class, output_i)
REGISTER_MEMBER_FUNCTION(tct1_class, pointer_out)

REGISTER_FREE_FUNCTION(mult)
REGISTER_FREE_FUNCTION(modify)
REGISTER_FREE_FUNCTION(triple)

SHADOW_INIT()
}


namespace tct1_space3
{
REGISTER_TYPE_BEGIN()
REGISTER_TYPE(tct1_struct)
REGISTER_TYPE(tct1_struct2)
REGISTER_TYPE_END()

REGISTER_MEMBER_VARIABLE(tct1_struct, i)
REGISTER_MEMBER_VARIABLE(tct1_struct, d)
REGISTER_MEMBER_VARIABLE(tct1_struct2, index)
REGISTER_MEMBER_VARIABLE(tct1_struct2, the_struct)

SHADOW_INIT()
}

TEST_CASE("create an int using static_construct", "[static_construct]")
{
    auto anint = tct1_space::static_construct<int>(23);
    auto anint2 = tct1_space2::static_construct<int>(100);

    REQUIRE(anint.type().name() == std::string("int"));
    REQUIRE(tct1_space::get_held_value<int>(anint) == 23);

    SECTION("find all conversions from type int")
    {
        auto conversions =
            tct1_space::manager.conversions_by_from_type(anint.type());

        REQUIRE(std::distance(conversions.first, conversions.second) > 0);

        SECTION("find the conversion from int to float")
        {
            auto found = std::find_if(
                conversions.first, conversions.second, [](const auto& conv) {
                    return tct1_space::manager.conversion_types(conv)
                               .second.name() == std::string("float");
                });

            REQUIRE(found != conversions.second);

            SECTION("convert anint to a float")
            {
                auto res = tct1_space::manager.convert(*found, anint);

                REQUIRE(res.type().name() == std::string("float"));
                REQUIRE(tct1_space::get_held_value<float>(res) ==
                        Approx(23.0f));
            }
        }
    }

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

    SECTION("find all functions from tct1_space2")
    {
        auto ffs = tct1_space2::manager.free_functions();

        REQUIRE(std::distance(ffs.first, ffs.second) == 3);

        SECTION("find function 'mult'")
        {
            auto found =
                std::find_if(ffs.first, ffs.second, [](const auto& ff) {
                    return tct1_space2::manager.free_function_name(ff) ==
                           std::string("mult");
                });

            REQUIRE(found != ffs.second);

            SECTION("call mult with anint2")
            {
                auto res = tct1_space2::manager.call_free_function(
                    *found, &anint2, &anint2 + 1);

                REQUIRE(res.type().name() == std::string("int"));
                REQUIRE(tct1_space2::get_held_value<int>(res) == 200);
                REQUIRE(tct1_space2::get_held_value<int>(anint2) == 100);
            }
        }

        SECTION("fund function 'modify'")
        {
            auto found =
                std::find_if(ffs.first, ffs.second, [](const auto& ff) {
                    return tct1_space2::manager.free_function_name(ff) ==
                           std::string("modify");
                });

            REQUIRE(found != ffs.second);

            SECTION("call mult with anint2")
            {
                auto res = tct1_space2::manager.call_free_function(
                    *found, &anint2, &anint2 + 1);

                REQUIRE(res.type().name() == std::string("void"));
                REQUIRE(tct1_space2::get_held_value<int>(anint2) == 110);
            }

            SECTION("call mult with anint in a vector")
            {
                std::vector<shadow::object> args{anint};

                auto res = tct1_space2::manager.call_free_function(
                    *found, args.begin(), args.end());

                REQUIRE(res.type().name() == std::string("void"));
                REQUIRE(tct1_space2::get_held_value<int>(args[0]) == 33);
            }
        }

        SECTION("find function 'triple'")
        {
            auto found =
                std::find_if(ffs.first, ffs.second, [](const auto& ff) {
                    return tct1_space2::manager.free_function_name(ff) ==
                           std::string("triple");
                });

            REQUIRE(found != ffs.second);

            SECTION("call triple with anint2")
            {
                auto res = tct1_space2::manager.call_free_function(
                    *found, &anint2, &anint2 + 1);

                REQUIRE(res.type().name() == std::string("void"));
                REQUIRE(tct1_space2::get_held_value<int>(anint2) == 300);
            }

            SECTION("call triple with anint in vector")
            {
                std::vector<shadow::object> args{anint};

                auto res = tct1_space2::manager.call_free_function(
                    *found, args.begin(), args.end());

                REQUIRE(res.type().name() == std::string("void"));
                REQUIRE(tct1_space::get_held_value<int>(args[0]) == 23 * 3);
            }
        }
    }

    SECTION("get all member functions in tct1_space2")
    {
        auto mfs = tct1_space2::manager.member_functions();

        REQUIRE(std::distance(mfs.first, mfs.second) == 4);

        SECTION("find tct1_class::get_i")
        {
            auto found =
                std::find_if(mfs.first, mfs.second, [](const auto& mf) {
                    return tct1_space2::manager.member_function_name(mf) ==
                               std::string("get_i") &&
                           tct1_space2::manager.member_function_class_type(mf)
                                   .name() == std::string("tct1_class");
                });

            REQUIRE(found != mfs.second);

            SECTION("get parameter types")
            {
                auto param_types =
                    tct1_space2::manager.member_function_parameter_types(
                        *found);

                REQUIRE(std::distance(param_types.first, param_types.second) ==
                        0);

                SECTION(
                    "call get_i on an ctc1_class object from same namespace")
                {
                    auto obj = tct1_space2::static_construct<tct1_class>(33);

                    std::vector<shadow::object> args;

                    auto res = tct1_space2::manager.call_member_function(
                        obj, *found, args.begin(), args.end());

                    REQUIRE(res.type().name() == std::string("int"));
                    REQUIRE(tct1_space2::get_held_value<int>(res) == 33);
                }
            }
        }

        SECTION("find tct1_class::set_i")
        {
            auto found =
                std::find_if(mfs.first, mfs.second, [](const auto& mf) {
                    return tct1_space2::manager.member_function_name(mf) ==
                               std::string("set_i") &&
                           tct1_space2::manager.member_function_class_type(mf)
                                   .name() == std::string("tct1_class");
                });

            REQUIRE(found != mfs.second);

            SECTION("get parameter types")
            {
                auto param_types =
                    tct1_space2::manager.member_function_parameter_types(
                        *found);

                REQUIRE(std::distance(param_types.first, param_types.second) ==
                        1);
                REQUIRE(param_types.first->name() == std::string("int"));

                SECTION("call set_i on a ctc1_class object from same namespace")
                {
                    auto obj = tct1_space2::static_construct<tct1_class>(33);

                    auto res = tct1_space2::manager.call_member_function(
                        obj, *found, &anint, &anint + 1);

                    REQUIRE(res.type().name() == std::string("void"));
                    REQUIRE(
                        tct1_space2::get_held_value<tct1_class>(obj).get_i() ==
                        23);
                }
            }
        }

        SECTION("find tct1_class::output_i")
        {
            auto found =
                std::find_if(mfs.first, mfs.second, [](const auto& mf) {
                    return tct1_space2::manager.member_function_name(mf) ==
                               std::string("output_i") &&
                           tct1_space2::manager.member_function_class_type(mf)
                                   .name() == std::string("tct1_class");
                });

            REQUIRE(found != mfs.second);

            SECTION("get parameter types")
            {
                auto param_types =
                    tct1_space2::manager.member_function_parameter_types(
                        *found);

                REQUIRE(std::distance(param_types.first, param_types.second) ==
                        1);
                REQUIRE(param_types.first->name() == std::string("int"));

                SECTION("call output_i on a ctc1_class object")
                {
                    auto obj = tct1_space2::static_construct<tct1_class>(2000);

                    std::vector<shadow::object> args{anint};

                    auto res = tct1_space2::manager.call_member_function(
                        obj, *found, args.begin(), args.end());

                    REQUIRE(res.type().name() == std::string("void"));
                    REQUIRE(tct1_space::get_held_value<int>(args[0]) == 2000);
                }
            }
        }

        SECTION("find tct1_class::pointer_out")
        {
            auto found =
                std::find_if(mfs.first, mfs.second, [](const auto& mf) {
                    return tct1_space2::manager.member_function_name(mf) ==
                               std::string("output_i") &&
                           tct1_space2::manager.member_function_class_type(mf)
                                   .name() == std::string("tct1_class");
                });

            REQUIRE(found != mfs.second);

            SECTION("get parameter types")
            {
                auto param_types =
                    tct1_space2::manager.member_function_parameter_types(
                        *found);

                REQUIRE(std::distance(param_types.first, param_types.second) ==
                        1);
                REQUIRE(param_types.first->name() == std::string("int"));

                SECTION("call pointer_out on a ctc1_class object")
                {
                    auto obj = tct1_space2::static_construct<tct1_class>(450);

                    std::vector<shadow::object> args{anint};

                    auto res = tct1_space2::manager.call_member_function(
                        obj, *found, args.begin(), args.end());

                    REQUIRE(res.type().name() == std::string("void"));
                    REQUIRE(tct1_space::get_held_value<int>(args[0]) == 450);
                }
            }
        }
    }
}


TEST_CASE("test member variables", "[reflection_manager]")
{
    auto mvs = tct1_space::manager.member_variables();

    REQUIRE(std::distance(mvs.first, mvs.second) == 2);

    SECTION("find member variable int tct1_struct::i")
    {
        auto found = std::find_if(mvs.first, mvs.second, [](const auto& mv) {

            return tct1_space::manager.member_variable_name(mv) ==
                       std::string("i") &&
                   tct1_space::manager.member_variable_class_type(mv).name() ==
                       std::string("tct1_struct");

        });

        REQUIRE(found != mvs.second);
        REQUIRE(tct1_space::manager.member_variable_type(*found).name() ==
                std::string("int"));

        SECTION("get member variable of a tct1_struct")
        {
            auto s = tct1_space::static_construct<tct1_struct>(20, 44.2);

            auto mv_value = tct1_space::manager.get_member_variable(s, *found);

            REQUIRE(mv_value.type().name() == std::string("int"));

            SECTION("set member variable of a tct1_struct")
            {
                tct1_space::manager.set_member_variable(
                    s, *found, tct1_space::static_construct<int>(40));

                REQUIRE(tct1_space::get_held_value<tct1_struct>(s).i == 40);
            }
        }
    }
}


TEST_CASE("test member variables by class type",
          "[reflection_manager::member_variables_by_class_type]")
{
    auto anobject = tct1_space::static_construct<tct1_struct>(10, 100.3);

    SECTION("get all member variables belonging to the object")
    {
        auto mv_pair =
            tct1_space::manager.member_variables_by_class_type(anobject.type());

        REQUIRE(std::distance(mv_pair.first, mv_pair.second) == 2);
        REQUIRE(tct1_space::manager.member_variable_name(*mv_pair.first) ==
                std::string("i"));
        REQUIRE(tct1_space::manager.member_variable_name(mv_pair.first[1]) ==
                std::string("d"));
    }
}


TEST_CASE("test constructors_by_type for tct1_struct",
          "[reflection_manager::constructors_by_type]")
{
    const shadow::reflection_manager& manager = tct1_space::manager;

    auto types = manager.types();

    auto found = std::find_if(types.first, types.second, [](const auto& tt) {
        return tt.name() == std::string("tct1_struct");
    });

    REQUIRE(found != types.second);

    SECTION("get all constructors for tct1_struct")
    {
        auto constructors = manager.constructors_by_type(*found);

        REQUIRE(std::distance(constructors.first, constructors.second) == 3);

        SECTION("find default constructor")
        {
            auto found_const =
                std::find_if(constructors.first,
                             constructors.second,
                             [&manager](const auto& ctr) {
                                 auto param_types =
                                     manager.constructor_parameter_types(ctr);

                                 return std::distance(param_types.first,
                                                      param_types.second) == 0;
                             });

            REQUIRE(found_const != constructors.second);
        }

        SECTION("find constructor taking int")
        {
            auto found_const = std::find_if(
                constructors.first,
                constructors.second,
                [&manager](const auto& ctr) {
                    auto param_types = manager.constructor_parameter_types(ctr);

                    if(std::distance(param_types.first, param_types.second) ==
                       1)
                    {
                        return param_types.first->name() == std::string("int");
                    }

                    return false;
                });

            REQUIRE(found_const != constructors.second);
        }

        SECTION("find constructor taking int and double")
        {
            auto found_const = std::find_if(
                constructors.first,
                constructors.second,
                [&manager](const auto& ctr) {
                    auto param_types = manager.constructor_parameter_types(ctr);

                    if(std::distance(param_types.first, param_types.second) ==
                       2)
                    {
                        return (param_types.first->name() ==
                                std::string("int")) &&
                               (param_types.first[1].name() ==
                                std::string("double"));
                    }

                    return false;
                });

            REQUIRE(found_const != constructors.second);
        }
    }
}


TEST_CASE("test serialization and deserialization", "[operator<</>>]")
{
    std::ostringstream out;

    SECTION("serialize int")
    {
        auto intobj = tct1_space3::static_construct<int>(112);

        out << intobj;

        REQUIRE(out.str() == std::string("112"));

        SECTION("deserialize to int")
        {
            auto deserialized_int = tct1_space3::static_construct<int>();
            std::istringstream in(out.str());

            in >> deserialized_int;

            REQUIRE(tct1_space3::get_held_value<int>(deserialized_int) == 112);
        }
    }

    SECTION("serialize float")
    {
        auto floatobj = tct1_space3::static_construct<float>(23.53f);

        out << floatobj;

        REQUIRE(out.str() == std::string("23.53"));

        SECTION("deserialize float")
        {
            std::istringstream in(out.str());

            auto desfloat = tct1_space3::static_construct<float>();

            in >> desfloat;

            REQUIRE(tct1_space3::get_held_value<float>(desfloat) ==
                    Approx(23.53f));
        }
    }

    SECTION("serialize std::string")
    {
        auto stringobj =
            tct1_space3::static_construct<std::string>("hello world");

        out << stringobj;

        REQUIRE(out.str() == std::string("\"hello world\""));

        SECTION("deserialize std::string")
        {
            std::istringstream in(out.str());

            auto deserialized = tct1_space3::static_construct<std::string>();

            in >> deserialized;

            REQUIRE(tct1_space3::get_held_value<std::string>(deserialized) ==
                    std::string("hello world"));
        }
    }

    SECTION("serialize char")
    {
        auto charobj = tct1_space3::static_construct<char>('c');

        out << charobj;

        REQUIRE(out.str() == std::string("c"));

        SECTION("deserialize char")
        {
            std::istringstream in(out.str());
        }
    }

    SECTION("serialize tct1_struct")
    {
        auto tct1sobj = tct1_space3::static_construct<tct1_struct>(22, 23.1244);

        out << tct1sobj;

        REQUIRE(out.str() == std::string("{22, 23.1244}"));

        SECTION("deserialize tct1_struct")
        {
            std::istringstream in(out.str());

            auto deserialized = tct1_space3::static_construct<tct1_struct>();

            in >> deserialized;

            CHECK(tct1_space3::get_held_value<tct1_struct>(deserialized).i ==
                  22);
            CHECK(tct1_space3::get_held_value<tct1_struct>(deserialized).d ==
                  Approx(23.1244));
        }
    }


    SECTION("serialize tct1_struct2")
    {
        auto tct1s2obj = tct1_space3::static_construct<tct1_struct2>(
            101ul, tct1_struct{12, 432.54});

        out << tct1s2obj;

        REQUIRE(out.str() == std::string("{101, {12, 432.54}}"));

        SECTION("deserialize tct1_struct2")
        {
            std::istringstream in(out.str());

            auto deserialized = tct1_space3::static_construct<tct1_struct2>();

            in >> deserialized;

            CHECK(
                tct1_space3::get_held_value<tct1_struct2>(deserialized).index ==
                101);

            CHECK(tct1_space3::get_held_value<tct1_struct2>(deserialized)
                      .the_struct.i == 12);

            CHECK(tct1_space3::get_held_value<tct1_struct2>(deserialized)
                      .the_struct.d == Approx(432.54));
        }
    }

    SECTION("serialize a series of objects")
    {
        std::vector<shadow::object> objects;
        objects.push_back(tct1_space3::static_construct<int>(22));
        objects.push_back(tct1_space3::static_construct<tct1_struct>(10, 3.2));
        objects.push_back(tct1_space3::static_construct<float>(42.5f));
        objects.push_back(tct1_space3::static_construct<tct1_struct2>(
            44ul, tct1_struct{63, 23.5}));

        std::ostringstream out;

        std::for_each(objects.cbegin(),
                      objects.cend(),
                      [&out](const auto& obj) { out << obj << ' '; });


        SECTION("deserialize objects")
        {
            std::istringstream in(out.str());

            std::vector<shadow::object> deserialized_objects;
            deserialized_objects.push_back(
                tct1_space3::static_construct<int>());
            deserialized_objects.push_back(
                tct1_space3::static_construct<tct1_struct>());
            deserialized_objects.push_back(
                tct1_space3::static_construct<float>());
            deserialized_objects.push_back(
                tct1_space3::static_construct<tct1_struct2>());

            std::for_each(deserialized_objects.begin(),
                          deserialized_objects.end(),
                          [&in](auto& obj) { in >> obj; });

            CHECK(tct1_space3::get_held_value<int>(deserialized_objects[0]) ==
                  22);
            CHECK(tct1_space3::get_held_value<tct1_struct>(
                      deserialized_objects[1])
                      .i == 10);
            CHECK(tct1_space3::get_held_value<tct1_struct>(
                      deserialized_objects[1])
                      .d == Approx(3.2));
            CHECK(tct1_space3::get_held_value<float>(deserialized_objects[2]) ==
                  Approx(42.5f));
            CHECK(tct1_space3::get_held_value<tct1_struct2>(
                      deserialized_objects[3])
                      .index == 44ul);
            CHECK(tct1_space3::get_held_value<tct1_struct2>(
                      deserialized_objects[3])
                      .the_struct.i == 63);
            CHECK(tct1_space3::get_held_value<tct1_struct2>(
                      deserialized_objects[3])
                      .the_struct.d == Approx(23.5));
        }
    }
}
