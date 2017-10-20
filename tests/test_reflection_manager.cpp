#include "catch.hpp"

#include <shadow.hpp>


TEST_CASE(
    "instantiate reflection_manager with empty information arrays, ie nullptrs",
    "[reflection_manager]")
{
    const shadow::type_info* ti_arr = nullptr;
    const shadow::constructor_info* ci_arr = nullptr;
    const shadow::conversion_info* cvi_arr = nullptr;
    const shadow::free_function_info* ffi_arr = nullptr;
    const shadow::member_function_info* mfi_arr = nullptr;
    const shadow::member_variable_info* mvi_arr = nullptr;
    const shadow::string_serialization_info* ssi_arr = nullptr;

    shadow::reflection_manager man(
        ti_arr, ci_arr, cvi_arr, ffi_arr, mfi_arr, mvi_arr, ssi_arr);


    SECTION("retrieve types")
    {
        auto types_pair = man.types();

        REQUIRE(types_pair.first == types_pair.second);
    }
}


TEST_CASE("instantiate reflection_manager with only type_info",
          "[reflection_manager]")
{
    const shadow::type_info ti_arr[] = {{"void", 0, nullptr, nullptr},
                                        {"int", sizeof(int), nullptr, nullptr}};
    const shadow::constructor_info* ci_arr = nullptr;
    const shadow::conversion_info* cvi_arr = nullptr;
    const shadow::free_function_info* ffi_arr = nullptr;
    const shadow::member_function_info* mfi_arr = nullptr;
    const shadow::member_variable_info* mvi_arr = nullptr;
    const shadow::string_serialization_info* ssi_arr = nullptr;

    shadow::reflection_manager man(
        ti_arr, ci_arr, cvi_arr, ffi_arr, mfi_arr, mvi_arr, ssi_arr);

    SECTION("retrieve types")
    {
        auto types_pair = man.types();

        REQUIRE(std::distance(types_pair.first, types_pair.second) == 2);
        REQUIRE(types_pair.first->name() == std::string("void"));
        REQUIRE(types_pair.first->size() == 0);
    }
}


TEST_CASE("instantiate reflection_manager with type info and constructor info",
          "[reflection_manager]")
{
    const shadow::type_info ti_arr[] = {{"void", 0, nullptr, nullptr},
                                        {"int", sizeof(int), nullptr, nullptr}};

    const std::size_t constr_params[] = {1};
    const bool pointer_flags[] = {false};

    const shadow::constructor_info ci_arr[] = {
        {1,
         0,
         nullptr,
         nullptr,
         &shadow::constructor_detail::generic_constructor_bind_point<int>},
        {1,
         1,
         static_cast<const std::size_t*>(constr_params),
         static_cast<const bool*>(pointer_flags),
         &shadow::constructor_detail::generic_constructor_bind_point<int,
                                                                     int>}};

    const shadow::conversion_info* cvi_arr = nullptr;
    const shadow::free_function_info* ffi_arr = nullptr;
    const shadow::member_function_info* mfi_arr = nullptr;
    const shadow::member_variable_info* mvi_arr = nullptr;
    const shadow::string_serialization_info* ssi_arr = nullptr;

    shadow::reflection_manager man(
        ti_arr, ci_arr, cvi_arr, ffi_arr, mfi_arr, mvi_arr, ssi_arr);

    SECTION("get constructor tags")
    {
        auto constr_tag_pair = man.constructors();

        REQUIRE(std::distance(constr_tag_pair.first, constr_tag_pair.second) ==
                2);

        SECTION("get target types of constructors")
        {
            auto tt1 = man.constructor_type(*constr_tag_pair.first);
            ++constr_tag_pair.first;
            auto tt2 = man.constructor_type(*constr_tag_pair.first);

            REQUIRE(tt1.name() == std::string("int"));
            REQUIRE(tt2.name() == std::string("int"));
        }

        SECTION("get constructor parameter types")
        {
            auto param_pair1 =
                man.constructor_parameter_types(*constr_tag_pair.first);
            ++constr_tag_pair.first;
            auto param_pair2 =
                man.constructor_parameter_types(*constr_tag_pair.first);

            auto num_params1 =
                std::distance(param_pair1.first, param_pair1.second);
            auto num_params2 =
                std::distance(param_pair2.first, param_pair2.second);


            if(num_params1 == 1)
            {
                REQUIRE(num_params2 == 0);
                REQUIRE((*param_pair1.first).name() == std::string("int"));
            }
            else
            {
                REQUIRE(num_params1 == 0);
                REQUIRE((*param_pair2.first).name() == std::string("int"));
            }
        }

        SECTION("find constructor with 0 parameters")
        {
            auto found =
                std::find_if(constr_tag_pair.first,
                             constr_tag_pair.second,
                             [&man](const auto& ct) {
                                 auto param_pair1 =
                                     man.constructor_parameter_types(ct);

                                 return std::distance(param_pair1.first,
                                                      param_pair1.second) == 0;
                             });

            REQUIRE(found != constr_tag_pair.second);

            SECTION("default construct an int object through type_manager")
            {
                std::vector<shadow::object> args;
                auto res =
                    man.construct_object(*found, args.begin(), args.end());

                REQUIRE(res.type().name() == std::string("int"));
            }
        }

        SECTION("find constructor with 1 parameter")
        {
            auto found =
                std::find_if(constr_tag_pair.first,
                             constr_tag_pair.second,
                             [&man](const auto& ct) {
                                 auto param_pair1 =
                                     man.constructor_parameter_types(ct);

                                 return std::distance(param_pair1.first,
                                                      param_pair1.second) == 1;
                             });

            REQUIRE(found != constr_tag_pair.second);

            SECTION("construct an int object with a value")
            {
                std::vector<shadow::object> args{
                    shadow::object(shadow::any(23), ti_arr + 1, &man)};

                auto res =
                    man.construct_object(*found, args.begin(), args.end());

                REQUIRE(res.type().name() == std::string("int"));
            }
        }
    }
}


TEST_CASE(
    "instantiate reflection_manager with some type_info and free_function_info",
    "[reflection_manager]")
{
    const shadow::type_info ti_arr[] = {{"void", 0, nullptr, nullptr},
                                        {"int", sizeof(int), nullptr, nullptr}};

    const std::size_t constr_params[] = {1};
    const bool pointer_flags[] = {false};

    const shadow::constructor_info ci_arr[] = {
        {1,
         0,
         nullptr,
         nullptr,
         &shadow::constructor_detail::generic_constructor_bind_point<int>},
        {1,
         1,
         static_cast<const std::size_t*>(constr_params),
         static_cast<const bool*>(pointer_flags),
         &shadow::constructor_detail::generic_constructor_bind_point<int,
                                                                     int>}};

    const std::size_t param_arr[] = {1};
    const bool ptr_arr[] = {false};

    const shadow::free_function_info ff_arr[] = {
        {"fun1", 0, 0, nullptr, nullptr},
        {"fun2",
         1,
         1,
         static_cast<const std::size_t*>(param_arr),
         static_cast<const bool*>(ptr_arr)}};

    const shadow::conversion_info* cvi_arr = nullptr;
    const shadow::member_function_info* mfi_arr = nullptr;
    const shadow::member_variable_info* mvi_arr = nullptr;
    const shadow::string_serialization_info* ssi_arr = nullptr;

    shadow::reflection_manager man(
        ti_arr, ci_arr, cvi_arr, ff_arr, mfi_arr, mvi_arr, ssi_arr);


    SECTION("get all free functions")
    {
        auto ffs = man.free_functions();

        REQUIRE(std::distance(ffs.first, ffs.second) == 2);

        SECTION("find fun1")
        {
            auto found =
                std::find_if(ffs.first, ffs.second, [](const auto& ff) {
                    return ff.name() == std::string("fun1");
                });

            REQUIRE(found != ffs.second);
            REQUIRE(found->name() == std::string("fun1"));
        }
    }
}
