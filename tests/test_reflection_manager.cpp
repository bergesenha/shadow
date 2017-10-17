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
}
