#pragma once

#include <array_view.hpp>
#include "reflection_info.hpp"
#include "api_types.hpp"


namespace shadow
{

template <class T>
struct array_specializer;

template <class T>
struct array_specializer<T* const>
{
    static helene::array_view<T>
    initialize(T* const)
    {
        return helene::array_view<T>();
    }
};

template <class T, std::size_t N>
struct array_specializer<const T[N]>
{
    static helene::array_view<const T>
    initialize(const T (&arr)[N])
    {
        return helene::make_array_view(arr);
    }
};

class reflection_manager
{
public:
    reflection_manager() = default;

    template <class TypeInfoArray,
              class ConstructorInfoArray,
              class ConversionInfoArray,
              class FreeFunctionInfoArray,
              class MemberFunctionInfoArray,
              class MemberVariableInfoArray>
    reflection_manager(TypeInfoArray& ti_arr,
                       ConstructorInfoArray& ctr_arr,
                       ConversionInfoArray& cv_arr,
                       FreeFunctionInfoArray& ff_arr,
                       MemberFunctionInfoArray& mf_arr,
                       MemberVariableInfoArray& mv_arr)
        : type_info_view_(array_specializer<TypeInfoArray>::initialize(ti_arr)),
          constructor_info_view_(
              array_specializer<ConstructorInfoArray>::initialize(ctr_arr)),
          conversion_info_view_(
              array_specializer<ConversionInfoArray>::initialize(cv_arr)),
          free_function_info_view_(
              array_specializer<FreeFunctionInfoArray>::initialize(ff_arr)),
          member_function_info_view_(
              array_specializer<MemberFunctionInfoArray>::initialize(mf_arr)),
          member_variable_info_view_(array_specializer<MemberVariableInfoArray>::initialize(mv_arr))
    {
    }


public:

private:
    helene::array_view<const type_info> type_info_view_;
    helene::array_view<const constructor_info> constructor_info_view_;
    helene::array_view<const conversion_info> conversion_info_view_;
    helene::array_view<const free_function_info> free_function_info_view_;
    helene::array_view<const member_function_info> member_function_info_view_;
    helene::array_view<const member_variable_info> member_variable_info_view_;
};
}
