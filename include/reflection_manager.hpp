#pragma once

#include <array_view.hpp>
#include "reflection_info.hpp"

namespace shadow
{


namespace reflection_initialization_detail
{
template <class ArrayType>
struct array_selector;

template <class T, std::size_t N>
struct array_selector<T[N]>
{
    static helene::array_view<T> initialize(T (&arr)[N])
    {
        return helene::make_array_view(arr);
    }
};

template <class T>
struct array_selector<T*>
{
    static helene::array_view<T>
    initialize(T* ptr)
    {
        return helene::array_view<T>();
    }
};
}


class reflection_manager
{
public:
    template <class TypeInfoArray,
              class ConstructorInfoArray,
              class ConversionInfoArray,
              class FreeFunctionArray,
              class MemberFunctionArray,
              class MemberVariableArray,
              class StringSerializationArray>
    reflection_manager(TypeInfoArray& ti_arr,
                       ConstructorInfoArray& ci_arr,
                       ConversionInfoArray& cv_arr,
                       FreeFunctionArray& ff_arr,
                       MemberFunctionArray& mf_arr,
                       MemberVariableArray& mv_arr,
                       StringSerializationArray& ss_arr);

private:
    helene::array_view<const type_info> type_info_view_;
    helene::array_view<const constructor_info> constructor_info_view_;
    helene::array_view<const conversion_info> conversion_info_view_;
    helene::array_view<const free_function_info> free_function_info_view_;
    helene::array_view<const member_function_info> member_function_info_view_;
    helene::array_view<const member_variable_info> member_variable_info_view_;
    helene::array_view<const string_serialization_info>
        string_serialization_info_view_;
};
}
