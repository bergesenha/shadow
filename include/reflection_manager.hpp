#pragma once

#include <array_view.hpp>
#include "reflection_info.hpp"

namespace shadow
{
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
    reflection_manager(TypeInfoArray&,
                       ConstructorInfoArray&,
                       ConversionInfoArray&,
                       FreeFunctionArray&,
                       MemberFunctionArray&,
                       MemberVariableArray&,
                       StringSerializationArray&);

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
