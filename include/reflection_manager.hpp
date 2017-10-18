#pragma once

#include <utility>

#include <array_view.hpp>
#include "reflection_info.hpp"
#include "api_types.hpp"
#include "info_iterators.hpp"

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
    typedef info_iterator_<const type_info, type_tag> const_type_iterator;
    typedef indexed_info_iterator_<const type_info, type_tag>
        const_indexed_type_iterator;
    typedef info_iterator_<const constructor_info, constructor_tag>
        const_constructor_iterator;

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


    // queries on types
    std::pair<const_type_iterator, const_type_iterator> types() const;

    std::string type_name(const type_tag& tag) const;

    std::size_t type_size(const type_tag& tag) const;


    // returns range of all constructors available
    std::pair<const_constructor_iterator, const_constructor_iterator>
    constructors() const;

    // returns the type that the given constructor belongs to
    type_tag constructor_type(const constructor_tag& tag) const;

    // returns range of the types of the parameters of the given constructor
    std::pair<const_indexed_type_iterator, const_indexed_type_iterator>
    constructor_parameter_types(const constructor_tag& tag) const;


    bool compare_type(const type_tag& tag, std::size_t index) const;

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

inline std::pair<typename reflection_manager::const_type_iterator,
                 typename reflection_manager::const_type_iterator>
reflection_manager::types() const
{
    return std::make_pair(const_type_iterator(type_info_view_.cbegin()),
                          const_type_iterator(type_info_view_.cend()));
}

inline std::string
reflection_manager::type_name(const type_tag& tag) const
{
    return tag.name();
}

inline std::size_t
reflection_manager::type_size(const type_tag& tag) const
{
    return tag.size();
}

std::pair<typename reflection_manager::const_constructor_iterator,
          typename reflection_manager::const_constructor_iterator>
reflection_manager::constructors() const
{
    return std::make_pair(
        const_constructor_iterator(constructor_info_view_.cbegin()),
        const_constructor_iterator(constructor_info_view_.cend()));
}

inline type_tag
reflection_manager::constructor_type(const constructor_tag& tag) const
{
    return type_tag(type_info_view_[tag.info_ptr_->type_index]);
}

inline std::pair<reflection_manager::const_indexed_type_iterator,
                 reflection_manager::const_indexed_type_iterator>
reflection_manager::constructor_parameter_types(
    const constructor_tag& tag) const
{
    auto index_buffer = tag.info_ptr_->parameter_type_indices;
    auto data_buffer = type_info_view_.data();

    return std::make_pair(
        const_indexed_type_iterator(0, index_buffer, data_buffer),
        const_indexed_type_iterator(
            tag.info_ptr_->num_parameters, index_buffer, data_buffer));
}

inline bool
reflection_manager::compare_type(const type_tag& tag, std::size_t index) const
{
    type_tag tag_from_index(type_info_view_[index]);

    return tag == tag_from_index;
}
} // namespace shadow
