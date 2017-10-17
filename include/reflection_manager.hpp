#pragma once

#include <array_view.hpp>
#include "reflection_info.hpp"

namespace shadow
{


namespace reflection_initialization_detail
{
// used to handle cases where reflection_manager is initialized with nullptrs
// (ie empty) instead of arrays
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
} // namespace reflection_initialization_detail


// point of interaction with the reflection system
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
    // array_views of reflection information generated at compile time
    helene::array_view<const type_info> type_info_view_;
    helene::array_view<const constructor_info> constructor_info_view_;
    helene::array_view<const conversion_info> conversion_info_view_;
    helene::array_view<const free_function_info> free_function_info_view_;
    helene::array_view<const member_function_info> member_function_info_view_;
    helene::array_view<const member_variable_info> member_variable_info_view_;
    helene::array_view<const string_serialization_info>
        string_serialization_info_view_;
};
} // namespace shadow

namespace shadow
{
template <class TypeInfoArray,
          class ConstructorInfoArray,
          class ConversionInfoArray,
          class FreeFunctionArray,
          class MemberFunctionArray,
          class MemberVariableArray,
          class StringSerializationArray>
inline reflection_manager::reflection_manager(TypeInfoArray& ti_arr,
                                              ConstructorInfoArray& ci_arr,
                                              ConversionInfoArray& cv_arr,
                                              FreeFunctionArray& ff_arr,
                                              MemberFunctionArray& mf_arr,
                                              MemberVariableArray& mv_arr,
                                              StringSerializationArray& ss_arr)
    : type_info_view_(reflection_initialization_detail::array_selector<
                      TypeInfoArray>::initialize(ti_arr)),
      constructor_info_view_(reflection_initialization_detail::array_selector<
                             ConstructorInfoArray>::initialize(ci_arr)),
      conversion_info_view_(reflection_initialization_detail::array_selector<
                            ConversionInfoArray>::initialize(cv_arr)),
      free_function_info_view_(reflection_initialization_detail::array_selector<
                               FreeFunctionArray>::initialize(ff_arr)),
      member_function_info_view_(
          reflection_initialization_detail::array_selector<
              MemberFunctionArray>::initialize(mf_arr)),
      member_variable_info_view_(
          reflection_initialization_detail::array_selector<
              MemberVariableArray>::initialize(mv_arr)),
      string_serialization_info_view_(
          reflection_initialization_detail::array_selector<
              StringSerializationArray>::initialize(ss_arr))
{
}
} // namespace shadow
