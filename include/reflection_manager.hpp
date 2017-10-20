#pragma once

#include <utility>
#include <stdexcept>
#include <vector>

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
template <class T>
struct array_selector
{
    static helene::array_view<std::remove_pointer_t<std::remove_const_t<T>>>
        initialize(T)
    {
        return helene::array_view<
            std::remove_pointer_t<std::remove_const_t<T>>>();
    }
};

template <class I, std::size_t N>
struct array_selector<I[N]>
{
    static helene::array_view<I> initialize(I (&arr)[N])
    {
        return helene::make_array_view(arr);
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

    typedef info_iterator_<const conversion_info, conversion_tag>
        const_conversion_iterator;

    typedef info_iterator_<const free_function_info, free_function_tag>
        const_free_function_iterator;

    typedef info_iterator_<const member_function_info, member_function_tag>
        const_member_function_iterator;

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

    template <class Iterator>
    object construct_object(const constructor_tag& tag,
                            Iterator first,
                            Iterator last) const;


    // type conversion functions
    std::pair<const_conversion_iterator, const_conversion_iterator>
    conversions() const;

    std::pair<type_tag, type_tag>
    conversion_types(const conversion_tag& tag) const;

    object convert(const conversion_tag& tag, const object& val) const;


    // returns range of tags to all free functions available
    std::pair<const_free_function_iterator, const_free_function_iterator>
    free_functions() const;

    // returns name of function associated with the free_function_tag
    std::string free_function_name(const free_function_tag& tag) const;

    // returns return type of function associated with the free_function_tag
    type_tag free_function_return_type(const free_function_tag& tag) const;

    // return range of type tags of parameters of free function associated with
    // the tag
    std::pair<const_indexed_type_iterator, const_indexed_type_iterator>
    free_function_parameter_types(const free_function_tag& tag) const;

    template <class Iterator>
    object call_free_function(const free_function_tag& tag,
                              Iterator first,
                              Iterator last) const;


    std::pair<const_member_function_iterator, const_member_function_iterator>
    member_functions() const;

    type_tag member_function_class(const member_function_tag& tag) const;

    std::string member_function_name(const member_function_tag& tag) const;

public:
    // unchecked operations
    template <class T>
    T& get(object& obj) const;

    template <class T>
    const T& get(const object& obj) const;

private:
    template <class Iterator, class InfoType>
    bool
    check_arguments(Iterator first, Iterator last, const InfoType& info) const;

    bool compare_type(const type_tag& tag, std::size_t index) const;

    template <class Iterator, class OutputIterator, class InfoType>
    void construct_argument_array(Iterator first,
                                  Iterator last,
                                  OutputIterator out,
                                  const InfoType& info) const;

    template <class Iterator, class OutputIterator, class InfoType>
    void pass_parameters_out(Iterator first,
                             Iterator last,
                             OutputIterator out,
                             const InfoType& info) const;

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

inline std::pair<typename reflection_manager::const_constructor_iterator,
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

template <class Iterator, class InfoType>
inline bool
reflection_manager::check_arguments(Iterator first,
                                    Iterator last,
                                    const InfoType& info) const
{
    if(info.num_parameters != std::distance(first, last))
    {
        return false;
    }

    for(auto index_ptr = info.parameter_type_indices; first != last;
        ++index_ptr, ++first)
    {
        if(compare_type(first->type(), *index_ptr) == false)
        {
            return false;
        }
    }

    return true;
}


template <class Iterator, class OutputIterator, class InfoType>
inline void
reflection_manager::construct_argument_array(Iterator first,
                                             Iterator last,
                                             OutputIterator out,
                                             const InfoType& info) const
{
    for(auto pointer_flag_ptr = info.parameter_pointer_flags; first != last;
        ++pointer_flag_ptr, ++first, ++out)
    {
        if(*pointer_flag_ptr)
        {
            // construct any with pointer to value
            auto address_bind_point = first->type_info_->address_of_bind_point;

            out = address_bind_point(first->value_);
        }
        else
        {
            out = first->value_;
        }
    }
}

template <class Iterator, class OutputIterator, class InfoType>
inline void
reflection_manager::pass_parameters_out(Iterator first,
                                        Iterator last,
                                        OutputIterator out,
                                        const InfoType& info) const
{
    for(auto ptr_flags = info.parameter_pointer_flags; first != last;
        ++first, ++out, ++ptr_flags)
    {
        if(*ptr_flags)
        {
            auto dereference_binding = out->type_info_->dereference_bind_point;
            out->value_ = dereference_binding(*first);
        }
        else
        {
            out->value_ = *first;
        }
    }
}

template <class Iterator>
inline object
reflection_manager::construct_object(const constructor_tag& tag,
                                     Iterator first,
                                     Iterator last) const
{
    if(check_arguments(first, last, *tag.info_ptr_) == false)
    {
        throw std::runtime_error("wrong argument types");
    }

    std::vector<any> arg_vec;
    arg_vec.reserve(std::distance(first, last));

    construct_argument_array(
        first, last, std::back_inserter(arg_vec), *tag.info_ptr_);

    auto return_value = tag.info_ptr_->bind_point(arg_vec.data());

    return object(
        return_value, type_info_view_.data() + tag.info_ptr_->type_index, this);
}

inline std::pair<reflection_manager::const_conversion_iterator,
                 reflection_manager::const_conversion_iterator>
reflection_manager::conversions() const
{
    return std::make_pair(
        const_conversion_iterator(conversion_info_view_.cbegin()),
        const_conversion_iterator(conversion_info_view_.cend()));
}

inline std::pair<type_tag, type_tag>
reflection_manager::conversion_types(const conversion_tag& tag) const
{
    return std::make_pair(
        type_tag(type_info_view_[tag.info_ptr_->from_type_index]),
        type_tag(type_info_view_[tag.info_ptr_->to_type_index]));
}

inline object
reflection_manager::convert(const conversion_tag& tag, const object& val) const
{
    if(val.type() != type_tag(type_info_view_[tag.info_ptr_->from_type_index]))
    {
        throw std::runtime_error(
            "type of object doesn't match conversion binding");
    }

    return object(tag.info_ptr_->bind_point(val.value_),
                  type_info_view_.data() + tag.info_ptr_->to_type_index,
                  this);
}
template <class T>
inline T&
reflection_manager::get(object& obj) const
{
    return obj.value_.get<T>();
}

template <class T>
inline const T&
reflection_manager::get(const object& obj) const
{
    return obj.value_.get<T>();
}


inline std::pair<reflection_manager::const_free_function_iterator,
                 reflection_manager::const_free_function_iterator>
reflection_manager::free_functions() const
{
    return std::make_pair(
        const_free_function_iterator(free_function_info_view_.cbegin()),
        const_free_function_iterator(free_function_info_view_.cend()));
}

inline std::string
reflection_manager::free_function_name(const free_function_tag& tag) const
{
    return std::string(tag.info_ptr_->name);
}

inline type_tag
reflection_manager::free_function_return_type(
    const free_function_tag& tag) const
{
    return type_tag(type_info_view_[tag.info_ptr_->return_type_index]);
}


inline std::pair<reflection_manager::const_indexed_type_iterator,
                 reflection_manager::const_indexed_type_iterator>
reflection_manager::free_function_parameter_types(
    const free_function_tag& tag) const
{
    return std::make_pair(
        const_indexed_type_iterator(
            0, tag.info_ptr_->parameter_type_indices, type_info_view_.data()),
        const_indexed_type_iterator(tag.info_ptr_->num_parameters,
                                    tag.info_ptr_->parameter_type_indices,
                                    type_info_view_.data()));
}


template <class Iterator>
inline object
reflection_manager::call_free_function(const free_function_tag& tag,
                                       Iterator first,
                                       Iterator last) const
{
    if(!check_arguments(first, last, *tag.info_ptr_))
    {
        throw std::runtime_error(
            "attempting to call free function with arguments of wrong type");
    }

    std::vector<shadow::any> args;
    args.reserve(std::distance(first, last));

    construct_argument_array(
        first, last, std::back_inserter(args), *tag.info_ptr_);

    auto return_value = tag.info_ptr_->bind_point(args.data());

    pass_parameters_out(args.begin(), args.end(), first, *tag.info_ptr_);

    return object(return_value,
                  type_info_view_.data() + tag.info_ptr_->return_type_index,
                  this);
}


inline std::pair<reflection_manager::const_member_function_iterator,
                 reflection_manager::const_member_function_iterator>
reflection_manager::member_functions() const
{
    return std::make_pair(
        const_member_function_iterator(member_function_info_view_.cbegin()),
        const_member_function_iterator(member_function_info_view_.cend()));
}


inline type_tag
reflection_manager::member_function_class(const member_function_tag& tag) const
{
    return type_tag(type_info_view_[tag.info_ptr_->object_type_index]);
}


inline std::string
reflection_manager::member_function_name(const member_function_tag& tag) const
{
    return std::string(tag.info_ptr_->name);
}
} // namespace shadow
