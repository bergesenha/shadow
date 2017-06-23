#pragma once

#include <utility>

#include "reflection_info.hpp"


namespace shadow
{
class reflection_manager
{
public:
    template <class TypeInfoArrayHolder,
              class ConstructorInfoArrayHolder,
              class ConversionInfoArrayHolder,
              class StringSerializationInfoArrayHolder,
              class FreeFunctionInfoArrayHolder,
              class MemberFunctionInfoArrayHolder,
              class MemberVariableInfoArrayHolder>
    reflection_manager(TypeInfoArrayHolder,
                       ConstructorInfoArrayHolder,
                       ConversionInfoArrayHolder,
                       StringSerializationInfoArrayHolder,
                       FreeFunctionInfoArrayHolder,
                       MemberFunctionInfoArrayHolder,
                       MemberVariableInfoArrayHolder)
        : type_info_range_(initialize_range(TypeInfoArrayHolder())),
          constructor_info_range_(
              initialize_range(ConstructorInfoArrayHolder())),
          conversion_info_range_(initialize_range(ConversionInfoArrayHolder())),
          string_serialization_info_range_(
              initialize_range(StringSerializationInfoArrayHolder())),
          free_function_info_range_(
              initialize_range(FreeFunctionInfoArrayHolder())),
          member_function_info_range_(
              initialize_range(MemberFunctionInfoArrayHolder())),
          member_variable_info_range_(
              initialize_range(MemberVariableInfoArrayHolder()))
    {
    }

private:
    template <class ArrayHolderType>
    std::pair<const typename ArrayHolderType::type*,
              const typename ArrayHolderType::type*>
        initialize_range(ArrayHolderType)
    {
// clang complains here due to the comparison of decayed array and
// nullptr, since in the case ArrayHolderType::value was an array, this
// would always evaluate to true. The fact is that
// ArrayHolderType::value may be a pointer to nullptr for some cases of
// ArrayHolderType.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wtautological-pointer-compare"
        if(ArrayHolderType::value != nullptr)
        {
            return std::make_pair(std::begin(ArrayHolderType::value),
                                  std::end(ArrayHolderType::value));
        }
        else
        {
            std::pair<const typename ArrayHolderType::type*,
                      const typename ArrayHolderType::type*>
                out(nullptr, nullptr);

            return out;
        }
    }
#pragma clang diagnostic pop

private:
    // pairs hold iterators to beginning and end of arrays of information
    // generated at compile time
    std::pair<const type_info*, const type_info*> type_info_range_;
    std::pair<const constructor_info*, const constructor_info*>
        constructor_info_range_;
    std::pair<const conversion_info*, const conversion_info*>
        conversion_info_range_;
    std::pair<const string_serialization_info*,
              const string_serialization_info*>
        string_serialization_info_range_;
    std::pair<const free_function_info*, const free_function_info*>
        free_function_info_range_;
    std::pair<const member_function_info*, const member_function_info*>
        member_function_info_range_;
    std::pair<const member_variable_info*, const member_variable_info*>
        member_variable_info_range_;
};
}
