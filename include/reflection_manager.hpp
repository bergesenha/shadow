#pragma once

#include <utility>
#include <vector>
#include <algorithm>

#include "reflection_info.hpp"
#include "info_iterator.hpp"
#include "api_info_types.hpp"

namespace shadow
{
////////////////////////////////////////////////////////////////////////////////
// user facing API types carrying information and functionality for the
// reflection system

class reflection_manager;

// holds one value with type/reflection information
class variable
{
private:
    // holds type erased value
    any value_;
    // these identify the type erased value
    const reflection_manager* manager_;
    std::size_t type_index_;
};


class reflection_manager
{
public:
    template <class Derived>
    friend class get_type_policy;

    template <class Derived>
    friend class get_parameter_types_policy;

    typedef type_ type;
    typedef info_iterator_<const type_info, const type> const_type_iterator;

    typedef constructor_ constructor;
    typedef info_iterator_<const constructor_info, const constructor>
        const_constructor_iterator;

public:
    reflection_manager();

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
                       MemberVariableInfoArrayHolder);

private:
    template <class ArrayHolderType>
    std::pair<const typename ArrayHolderType::type*,
              const typename ArrayHolderType::type*>
        initialize_range(ArrayHolderType);


    template <class ArrayHolderType>
    std::size_t array_size(ArrayHolderType);


    template <class ValueType, class TypeInfoArrayHolder, class Fun>
    std::vector<std::vector<ValueType>>
    buckets_by_index(const std::pair<const ValueType*, const ValueType*>& range,
                     TypeInfoArrayHolder,
                     Fun index_extractor);


    type type_by_index(std::size_t index) const;


public:
    ////////////////////////////////////////////////////////////////////////////
    // main api interface for interacting with the reflection system
    std::pair<const_type_iterator, const_type_iterator> types() const;

    std::pair<const_constructor_iterator, const_constructor_iterator>
    constructors() const;

    std::pair<const_constructor_iterator, const_constructor_iterator>
    constructors(const type& tp) const;

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

    // sorted information
    std::vector<std::vector<constructor_info>> constructor_info_by_index_;
    std::vector<std::vector<conversion_info>> conversion_info_by_index_;
    std::vector<std::vector<member_function_info>>
        member_function_info_by_index_;
    std::vector<std::vector<member_variable_info>>
        member_variable_info_by_index_;
    std::vector<string_serialization_info> string_serializer_info_by_index_;
};
}

////////////////////////////////////////////////////////////////////////////////
// DEFINITIONS
namespace shadow
{
inline reflection_manager::reflection_manager() = default;


template <class TypeInfoArrayHolder,
          class ConstructorInfoArrayHolder,
          class ConversionInfoArrayHolder,
          class StringSerializationInfoArrayHolder,
          class FreeFunctionInfoArrayHolder,
          class MemberFunctionInfoArrayHolder,
          class MemberVariableInfoArrayHolder>
inline reflection_manager::reflection_manager(
    TypeInfoArrayHolder,
    ConstructorInfoArrayHolder,
    ConversionInfoArrayHolder,
    StringSerializationInfoArrayHolder,
    FreeFunctionInfoArrayHolder,
    MemberFunctionInfoArrayHolder,
    MemberVariableInfoArrayHolder)
    : type_info_range_(initialize_range(TypeInfoArrayHolder())),
      constructor_info_range_(initialize_range(ConstructorInfoArrayHolder())),
      conversion_info_range_(initialize_range(ConversionInfoArrayHolder())),
      string_serialization_info_range_(
          initialize_range(StringSerializationInfoArrayHolder())),
      free_function_info_range_(
          initialize_range(FreeFunctionInfoArrayHolder())),
      member_function_info_range_(
          initialize_range(MemberFunctionInfoArrayHolder())),
      member_variable_info_range_(
          initialize_range(MemberVariableInfoArrayHolder())),
      constructor_info_by_index_(
          buckets_by_index(constructor_info_range_,
                           TypeInfoArrayHolder(),
                           [](const auto& info) { return info.type_index; })),
      conversion_info_by_index_(buckets_by_index(
          conversion_info_range_,
          TypeInfoArrayHolder(),
          [](const auto& info) { return info.from_type_index; })),
      member_function_info_by_index_(buckets_by_index(
          member_function_info_range_,
          TypeInfoArrayHolder(),
          [](const auto& info) { return info.object_type_index; })),
      member_variable_info_by_index_(buckets_by_index(
          member_variable_info_range_,
          TypeInfoArrayHolder(),
          [](const auto& info) { return info.object_type_index; })),
      string_serializer_info_by_index_(array_size(TypeInfoArrayHolder()))
{
    std::for_each(string_serialization_info_range_.first,
                  string_serialization_info_range_.second,
                  [this](const auto& info) {
                      string_serializer_info_by_index_[info.type_index] = info;
                  });
}


// clang complains here due to the comparison of decayed array and
// nullptr, since in the case ArrayHolderType::value was an array, this
// would always evaluate to true. The fact is that
// ArrayHolderType::value may be a pointer to nullptr for some cases of
// ArrayHolderType.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wtautological-pointer-compare"
template <class ArrayHolderType>
inline std::pair<const typename ArrayHolderType::type*,
                 const typename ArrayHolderType::type*>
    reflection_manager::initialize_range(ArrayHolderType)
{
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


template <class ArrayHolderType>
inline std::size_t reflection_manager::array_size(ArrayHolderType)
{
    if(ArrayHolderType::value == nullptr)
    {
        return 0;
    }
    else
    {
        return std::extent<decltype(ArrayHolderType::value)>::value;
    }
}


template <class ValueType, class TypeInfoArrayHolder, class Fun>
inline std::vector<std::vector<ValueType>>
reflection_manager::buckets_by_index(
    const std::pair<const ValueType*, const ValueType*>& range,
    TypeInfoArrayHolder,
    Fun index_extractor)
{
    if(range.first != nullptr && range.second != nullptr)
    {
        std::vector<std::vector<ValueType>> out(
            array_size(TypeInfoArrayHolder()));

        std::for_each(range.first,
                      range.second,
                      [&out, index_extractor](const auto& info) {
                          out[index_extractor(info)].push_back(info);
                      });

        return out;
    }
    else
    {
        return std::vector<std::vector<ValueType>>();
    }
}
#pragma clang diagnostic pop


inline reflection_manager::type
reflection_manager::type_by_index(std::size_t index) const
{
    return type(type_info_range_.first + index, this);
}


inline std::pair<reflection_manager::const_type_iterator,
                 reflection_manager::const_type_iterator>
reflection_manager::types() const
{
    return std::make_pair(const_type_iterator(type_info_range_.first, this),
                          const_type_iterator(type_info_range_.second, this));
}

inline std::pair<reflection_manager::const_constructor_iterator,
                 reflection_manager::const_constructor_iterator>
reflection_manager::constructors() const
{
    return std::make_pair(
        const_constructor_iterator(constructor_info_range_.first, this),
        const_constructor_iterator(constructor_info_range_.second, this));
}

inline std::pair<reflection_manager::const_constructor_iterator,
                 reflection_manager::const_constructor_iterator>
reflection_manager::constructors(const type& tp) const
{
    const auto index_of_type = tp.info_ - type_info_range_.first;

    const auto& constr_vec = constructor_info_by_index_[index_of_type];

    return std::make_pair(const_constructor_iterator(constr_vec.data(), this),
                          const_constructor_iterator(
                              constr_vec.data() + constr_vec.size(), this));
}
}


////////////////////////////////////////////////////////////////////////////////
// DEFINITIONS for policies
namespace shadow
{
template <class Derived>
inline std::pair<
    typename get_parameter_types_policy<Derived>::const_parameter_type_iterator,
    typename get_parameter_types_policy<Derived>::const_parameter_type_iterator>
get_parameter_types_policy<Derived>::parameter_types() const
{
    const auto num_parameters =
        static_cast<const Derived*>(this)->info_->num_parameters;
    const std::size_t* param_type_index_buffer =
        static_cast<const Derived*>(this)->info_->parameter_type_indices;
    const reflection_manager* manager =
        static_cast<const Derived*>(this)->manager_;

    return std::make_pair(
        const_parameter_type_iterator(0,
                                      param_type_index_buffer,
                                      manager->type_info_range_.first,
                                      manager),
        const_parameter_type_iterator(num_parameters,
                                      param_type_index_buffer,
                                      manager->type_info_range_.first,
                                      manager));
}
}
