#pragma once

#include <utility>
#include <vector>
#include <algorithm>
#include <ostream>
#include <istream>

#include "reflection_info.hpp"
#include "info_iterator.hpp"
#include "api_info_types.hpp"

namespace shadow
{


class reflection_manager
{
public:
    template <class Derived>
    friend class get_type_policy;

    template <class Derived>
    friend class get_parameter_types_policy;

    template <class Derived>
    friend class get_from_type_policy;

    template <class Derived>
    friend class get_to_type_policy;

    template <class Derived>
    friend class get_return_type_policy;

    template <class Derived>
    friend class get_object_type_policy;

    template <class Derived>
    friend class call_free_function;

    friend class variable;

    friend std::ostream& operator<<(std::ostream&, const variable&);
    friend std::istream& operator>>(std::istream&, variable&);

    typedef type_ type;
    typedef info_iterator_<const type_info, const type> const_type_iterator;

    typedef constructor_ constructor;
    typedef info_iterator_<const constructor_info, const constructor>
        const_constructor_iterator;
    typedef indexed_info_iterator_<const constructor_info, const constructor>
        const_indexed_constructor_iterator;

    typedef type_conversion_ type_conversion;
    typedef info_iterator_<const conversion_info, const type_conversion>
        const_conversion_iterator;
    typedef indexed_info_iterator_<const conversion_info, const type_conversion>
        const_indexed_conversion_iterator;

    typedef free_function_ free_function;
    typedef info_iterator_<const free_function_info, const free_function>
        const_free_function_iterator;

    typedef member_function_ member_function;
    typedef info_iterator_<const member_function_info, const member_function>
        const_member_function_iterator;
    typedef indexed_info_iterator_<const member_function_info,
                                   const member_function>
        const_indexed_member_function_iterator;

    typedef member_variable_ member_variable;
    typedef info_iterator_<const member_variable_info, const member_variable>
        const_member_variable_iterator;
    typedef indexed_info_iterator_<const member_variable_info,
                                   const member_variable>
        const_indexed_member_variable_iterator;

    typedef string_serializer_ string_serializer;
    typedef info_iterator_<const string_serialization_info,
                           const string_serializer>
        const_string_serializer_iterator;

public:
    reflection_manager();

    template <class TypeInfoArrayHolder,
              class ConstructorInfoArrayHolder,
              class ConversionInfoArrayHolder,
              class FreeFunctionInfoArrayHolder,
              class MemberFunctionInfoArrayHolder,
              class MemberVariableInfoArrayHolder,
              class StringSerializationInfoArrayHolder>
    reflection_manager(TypeInfoArrayHolder,
                       ConstructorInfoArrayHolder,
                       ConversionInfoArrayHolder,
                       FreeFunctionInfoArrayHolder,
                       MemberFunctionInfoArrayHolder,
                       MemberVariableInfoArrayHolder,
                       StringSerializationInfoArrayHolder);

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

    template <class ValueType, class TypeInfoArrayHolder, class Fun>
    std::vector<std::vector<std::size_t>>
    indices_by_type(const std::pair<const ValueType*, const ValueType*>& range,
                    TypeInfoArrayHolder,
                    Fun type_index_extractor);


    type type_by_index(std::size_t index) const;


public:
    ////////////////////////////////////////////////////////////////////////////
    // main api interface for interacting with the reflection system
    std::pair<const_type_iterator, const_type_iterator> types() const;

    std::pair<const_constructor_iterator, const_constructor_iterator>
    constructors() const;

    std::pair<const_indexed_constructor_iterator,
              const_indexed_constructor_iterator>
    constructors_by_type(const type& tp) const;

    std::pair<const_conversion_iterator, const_conversion_iterator>
    type_conversions() const;

    std::pair<const_indexed_conversion_iterator,
              const_indexed_conversion_iterator>
    type_conversions_by_type(const type& tp) const;

    std::pair<const_free_function_iterator, const_free_function_iterator>
    free_functions() const;

    std::pair<const_member_function_iterator, const_member_function_iterator>
    member_functions() const;

    std::pair<const_indexed_member_function_iterator,
              const_indexed_member_function_iterator>
    member_functions_by_type(const type& tp) const;

    std::pair<const_member_variable_iterator, const_member_variable_iterator>
    member_variables() const;

    std::pair<const_indexed_member_variable_iterator,
              const_indexed_member_variable_iterator>
    member_variables_by_type(const type& tp) const;

    std::pair<const_string_serializer_iterator,
              const_string_serializer_iterator>
    string_serializers() const;

    template <class TypeUniverseList, class T>
    variable static_create(const T& value) const;

private:
    // pairs hold iterators to beginning and end of arrays of information
    // generated at compile time
    std::pair<const type_info*, const type_info*> type_info_range_;
    std::pair<const constructor_info*, const constructor_info*>
        constructor_info_range_;
    std::pair<const conversion_info*, const conversion_info*>
        conversion_info_range_;
    std::pair<const free_function_info*, const free_function_info*>
        free_function_info_range_;
    std::pair<const member_function_info*, const member_function_info*>
        member_function_info_range_;
    std::pair<const member_variable_info*, const member_variable_info*>
        member_variable_info_range_;
    std::pair<const string_serialization_info*,
              const string_serialization_info*>
        string_serialization_info_range_;


    // sorted index information
    std::vector<std::vector<std::size_t>> constructor_info_indices_by_type_;
    std::vector<std::vector<std::size_t>> conversion_info_indices_by_type_;
    std::vector<std::vector<std::size_t>> member_function_info_indices_by_type_;
    std::vector<std::vector<std::size_t>> member_variable_info_indices_by_type_;
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
          class FreeFunctionInfoArrayHolder,
          class MemberFunctionInfoArrayHolder,
          class MemberVariableInfoArrayHolder,
          class StringSerializationInfoArrayHolder>
inline reflection_manager::reflection_manager(
    TypeInfoArrayHolder,
    ConstructorInfoArrayHolder,
    ConversionInfoArrayHolder,
    FreeFunctionInfoArrayHolder,
    MemberFunctionInfoArrayHolder,
    MemberVariableInfoArrayHolder,
    StringSerializationInfoArrayHolder)
    : type_info_range_(initialize_range(TypeInfoArrayHolder())),
      constructor_info_range_(initialize_range(ConstructorInfoArrayHolder())),
      conversion_info_range_(initialize_range(ConversionInfoArrayHolder())),
      free_function_info_range_(
          initialize_range(FreeFunctionInfoArrayHolder())),
      member_function_info_range_(
          initialize_range(MemberFunctionInfoArrayHolder())),
      member_variable_info_range_(
          initialize_range(MemberVariableInfoArrayHolder())),
      string_serialization_info_range_(
          initialize_range(StringSerializationInfoArrayHolder())),
      constructor_info_indices_by_type_(
          indices_by_type(constructor_info_range_,
                          TypeInfoArrayHolder(),
                          [](const auto& ci) { return ci.type_index; })),
      conversion_info_indices_by_type_(
          indices_by_type(conversion_info_range_,
                          TypeInfoArrayHolder(),
                          [](const auto& ci) { return ci.from_type_index; })),
      member_function_info_indices_by_type_(indices_by_type(
          member_function_info_range_,
          TypeInfoArrayHolder(),
          [](const auto& mfi) { return mfi.object_type_index; })),
      member_variable_info_indices_by_type_(indices_by_type(
          member_variable_info_range_,
          TypeInfoArrayHolder(),
          [](const auto& mvi) { return mvi.object_type_index; }))
{
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


template <class ValueType, class TypeInfoArrayHolder, class Fun>
inline std::vector<std::vector<std::size_t>>
reflection_manager::indices_by_type(
    const std::pair<const ValueType*, const ValueType*>& range,
    TypeInfoArrayHolder,
    Fun type_index_extractor)
{
    if(range.first != nullptr && range.second != nullptr)
    {
        std::vector<std::vector<std::size_t>> out(
            array_size(TypeInfoArrayHolder()));

        for(auto i = 0ul; i < range.second - range.first; ++i)
        {
            out[type_index_extractor(range.first[i])].push_back(i);
        }

        return out;
    }
    else
    {
        return std::vector<std::vector<std::size_t>>();
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


inline std::pair<reflection_manager::const_indexed_constructor_iterator,
                 reflection_manager::const_indexed_constructor_iterator>
reflection_manager::constructors_by_type(const type& tp) const
{
    const auto index_of_type = tp.info_ - type_info_range_.first;
    return std::make_pair(
        const_indexed_constructor_iterator(
            0,
            constructor_info_indices_by_type_[index_of_type].data(),
            constructor_info_range_.first,
            this),
        const_indexed_constructor_iterator(
            constructor_info_indices_by_type_[index_of_type].size(),
            constructor_info_indices_by_type_[index_of_type].data(),
            constructor_info_range_.first,
            this));
}


inline std::pair<reflection_manager::const_conversion_iterator,
                 reflection_manager::const_conversion_iterator>
reflection_manager::type_conversions() const
{
    return std::make_pair(
        const_conversion_iterator(conversion_info_range_.first, this),
        const_conversion_iterator(conversion_info_range_.second, this));
}


inline std::pair<reflection_manager::const_indexed_conversion_iterator,
                 reflection_manager::const_indexed_conversion_iterator>
reflection_manager::type_conversions_by_type(const type& tp) const
{
    const auto index_of_type = tp.info_ - type_info_range_.first;
    return std::make_pair(
        const_indexed_conversion_iterator(
            0,
            conversion_info_indices_by_type_[index_of_type].data(),
            conversion_info_range_.first,
            this),
        const_indexed_conversion_iterator(
            conversion_info_indices_by_type_[index_of_type].size(),
            conversion_info_indices_by_type_[index_of_type].data(),
            conversion_info_range_.first,
            this));
}


inline std::pair<reflection_manager::const_free_function_iterator,
                 reflection_manager::const_free_function_iterator>
reflection_manager::free_functions() const
{
    return std::make_pair(
        const_free_function_iterator(free_function_info_range_.first, this),
        const_free_function_iterator(free_function_info_range_.second, this));
}


inline std::pair<reflection_manager::const_member_function_iterator,
                 reflection_manager::const_member_function_iterator>
reflection_manager::member_functions() const
{
    return std::make_pair(
        const_member_function_iterator(member_function_info_range_.first, this),
        const_member_function_iterator(member_function_info_range_.second,
                                       this));
}


inline std::pair<reflection_manager::const_indexed_member_function_iterator,
                 reflection_manager::const_indexed_member_function_iterator>
reflection_manager::member_functions_by_type(const type& tp) const
{
    const auto index_of_type = tp.info_ - type_info_range_.first;
    return std::make_pair(
        const_indexed_member_function_iterator(
            0,
            member_function_info_indices_by_type_[index_of_type].data(),
            member_function_info_range_.first,
            this),
        const_indexed_member_function_iterator(
            member_function_info_indices_by_type_[index_of_type].size(),
            member_function_info_indices_by_type_[index_of_type].data(),
            member_function_info_range_.first,
            this));
}

inline std::pair<reflection_manager::const_member_variable_iterator,
                 reflection_manager::const_member_variable_iterator>
reflection_manager::member_variables() const
{
    return std::make_pair(
        const_member_variable_iterator(member_variable_info_range_.first, this),
        const_member_variable_iterator(member_variable_info_range_.second,
                                       this));
}


inline std::pair<reflection_manager::const_indexed_member_variable_iterator,
                 reflection_manager::const_indexed_member_variable_iterator>
reflection_manager::member_variables_by_type(const type& tp) const
{
    const auto index_of_type = tp.info_ - type_info_range_.first;
    return std::make_pair(
        const_indexed_member_variable_iterator(
            0,
            member_variable_info_indices_by_type_[index_of_type].data(),
            member_variable_info_range_.first,
            this),
        const_indexed_member_variable_iterator(
            member_variable_info_indices_by_type_[index_of_type].size(),
            member_variable_info_indices_by_type_[index_of_type].data(),
            member_variable_info_range_.first,
            this));
}

inline std::pair<reflection_manager::const_string_serializer_iterator,
                 reflection_manager::const_string_serializer_iterator>
reflection_manager::string_serializers() const
{
    return std::make_pair(const_string_serializer_iterator(
                              string_serialization_info_range_.first, this),
                          const_string_serializer_iterator(
                              string_serialization_info_range_.second, this));
}


template <class TypeUniverseList, class T>
inline variable
reflection_manager::static_create(const T& value) const
{
    const auto type_index =
        metamusil::t_list::index_of_type_v<TypeUniverseList, T>;

    return variable(value, type_index, this);
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


////////////////////////////////////////////////////////////////////////////////
// DEFINITIONS for variable
inline type_
variable::type() const
{
    return manager_->type_by_index(type_index_);
}

inline std::pair<variable::member_function_iterator,
                 variable::member_function_iterator>
variable::member_functions() const
{
    return std::make_pair(
        member_function_iterator(
            0,
            manager_->member_function_info_indices_by_type_[type_index_].data(),
            manager_->member_function_info_range_.first,
            manager_),
        member_function_iterator(
            manager_->member_function_info_indices_by_type_[type_index_].size(),
            manager_->member_function_info_indices_by_type_[type_index_].data(),
            manager_->member_function_info_range_.first,
            manager_));
}


inline std::pair<variable::member_variable_iterator,
                 variable::member_variable_iterator>
variable::member_variables() const
{
    return std::make_pair(
        member_variable_iterator(
            0,
            manager_->member_variable_info_indices_by_type_[type_index_].data(),
            manager_->member_variable_info_range_.first,
            manager_),
        member_variable_iterator(
            manager_->member_variable_info_indices_by_type_[type_index_].size(),
            manager_->member_variable_info_indices_by_type_[type_index_].data(),
            manager_->member_variable_info_range_.first,
            manager_));
}


inline std::ostream&
operator<<(std::ostream& out, const variable& var)
{
    if(!var.manager_)
    {
        out << "empty";
        return out;
    }
    // try to find a string serializer in the reflection_manager of var
    auto ssi_range = var.manager_->string_serialization_info_range_;
    auto found = std::find_if(
        ssi_range.first, ssi_range.second, [&var](const auto& ssi) {
            return ssi.type_index == var.type_index_;
        });

    if(found != ssi_range.second)
    {
        out << found->serialize_bind_point(var.value_);
    }
    else
    {
        const auto& mem_var_info_indices =
            var.manager_
                ->member_variable_info_indices_by_type_[var.type_index_];
        const auto mem_var_info_buffer =
            var.manager_->member_variable_info_range_.first;

        out << "{ ";

        if(mem_var_info_indices.size() > 0)
        {

            auto index_begin = mem_var_info_indices.cbegin();
            auto index_end = mem_var_info_indices.cend();

            out << "\"" << mem_var_info_buffer[*index_begin].name << "\":"
                << variable(mem_var_info_buffer[*index_begin].get_bind_point(
                                var.value_),
                            mem_var_info_buffer[*index_begin].type_index,
                            var.manager_);

            ++index_begin;

            std::for_each(
                index_begin,
                index_end,
                [&out, mem_var_info_buffer, &var](const auto index) {
                    out << ", \"" << mem_var_info_buffer[index].name << "\":";
                    out << variable(
                        mem_var_info_buffer[index].get_bind_point(var.value_),
                        mem_var_info_buffer[index].type_index,
                        var.manager_);
                });
        }
        out << " }";
    }

    return out;
}

inline std::istream&
operator>>(std::istream& in, variable& var)
{

    auto ssi_pair = var.manager_->string_serialization_info_range_;

    // attempt to find fundamental type string serializer
    auto found =
        std::find_if(ssi_pair.first, ssi_pair.second, [&var](const auto& ssi) {
            return ssi.type_index == var.type_index_;
        });

    if(found != ssi_pair.second)
    {
        std::string instring;
        in >> instring;

        var.value_ = found->deserialize_bind_point(instring);
    }
    else
    {
    }

    return in;
}
}
