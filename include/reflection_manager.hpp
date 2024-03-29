#pragma once

#include <utility>
#include <stdexcept>
#include <vector>
#include <algorithm>

#include <array_view.hpp>
#include "reflection_info.hpp"
#include "api_types.hpp"
#include "info_iterators.hpp"
#include "exceptions.hpp"

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
    friend std::ostream& operator<<(std::ostream&, const object&);
    friend std::istream& operator>>(std::istream&, object&);

public:
    typedef info_iterator_<const type_info, type_tag> const_type_iterator;
    typedef indexed_info_iterator_<const type_info, type_tag>
        const_indexed_type_iterator;

    typedef info_iterator_<const constructor_info, constructor_tag>
        const_constructor_iterator;
    typedef indexed_info_iterator_<const constructor_info, constructor_tag>
        const_indexed_constructor_iterator;

    typedef info_iterator_<const conversion_info, conversion_tag>
        const_conversion_iterator;
    typedef indexed_info_iterator_<const conversion_info, conversion_tag>
        const_indexed_conversion_iterator;

    typedef info_iterator_<const free_function_info, free_function_tag>
        const_free_function_iterator;

    typedef info_iterator_<const member_function_info, member_function_tag>
        const_member_function_iterator;
    typedef indexed_info_iterator_<const member_function_info,
                                   member_function_tag>
        const_indexed_member_function_iterator;

    typedef info_iterator_<const member_variable_info, member_variable_tag>
        const_member_variable_iterator;
    typedef indexed_info_iterator_<const member_variable_info,
                                   member_variable_tag>
        const_indexed_member_variable_iterator;

public:
    template <class TypeInfoArray,
              class ConstructorInfoArray,
              class ConversionInfoArray,
              class FreeFunctionArray,
              class MemberFunctionArray,
              class MemberVariableArray,
              class SerializationInfoArray>
    reflection_manager(TypeInfoArray& ti_arr,
                       ConstructorInfoArray& ci_arr,
                       ConversionInfoArray& cv_arr,
                       FreeFunctionArray& ff_arr,
                       MemberFunctionArray& mf_arr,
                       MemberVariableArray& mv_arr,
                       SerializationInfoArray& si_arr);


    // queries on types
    std::pair<const_type_iterator, const_type_iterator> types() const;

    std::string type_name(const type_tag& tag) const;

    std::size_t type_size(const type_tag& tag) const;


    // returns range of all constructors available
    std::pair<const_constructor_iterator, const_constructor_iterator>
    constructors() const;

    // returns range of all constructors for given type
    std::pair<const_indexed_constructor_iterator,
              const_indexed_constructor_iterator>
    constructors_by_type(const type_tag& tag) const;

    // returns the type that the given constructor belongs to
    type_tag constructor_type(const constructor_tag& tag) const;

    // returns range of the types of the parameters of the given constructor
    std::pair<const_indexed_type_iterator, const_indexed_type_iterator>
    constructor_parameter_types(const constructor_tag& tag) const;

    template <class Iterator>
    object construct_object(const constructor_tag& tag,
                            Iterator first,
                            Iterator last) const;

    object construct_object(const constructor_tag& tag) const;


    // type conversion functions
    std::pair<const_conversion_iterator, const_conversion_iterator>
    conversions() const;

    std::pair<const_indexed_conversion_iterator,
              const_indexed_conversion_iterator>
    conversions_by_from_type(const type_tag& tag) const;

    // return pair of types of a conversion, first: from_type, second: to_type
    std::pair<type_tag, type_tag>
    conversion_types(const conversion_tag& tag) const;

    // convert an object to another type according to the given conversion
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

    object call_free_function(const free_function_tag& tag) const;


    // return all available member functions
    std::pair<const_member_function_iterator, const_member_function_iterator>
    member_functions() const;

    std::pair<const_indexed_member_function_iterator,
              const_indexed_member_function_iterator>
    member_functions_by_class_type(const type_tag& tag) const;

    type_tag member_function_class_type(const member_function_tag& tag) const;

    std::string member_function_name(const member_function_tag& tag) const;

    std::pair<const_indexed_type_iterator, const_indexed_type_iterator>
    member_function_parameter_types(const member_function_tag& tag) const;

    template <class Iterator>
    object call_member_function(object& obj,
                                const member_function_tag& tag,
                                Iterator first,
                                Iterator last) const;

    object call_member_function(object& obj,
                                const member_function_tag& tag) const;


    std::pair<const_member_variable_iterator, const_member_variable_iterator>
    member_variables() const;

    std::pair<const_indexed_member_variable_iterator,
              const_indexed_member_variable_iterator>
    member_variables_by_class_type(const type_tag& tag) const;

    type_tag member_variable_type(const member_variable_tag& tag) const;

    type_tag member_variable_class_type(const member_variable_tag& tag) const;

    std::string member_variable_name(const member_variable_tag& tag) const;

    void set_member_variable(object& obj,
                             const member_variable_tag& tag,
                             const object& val) const;

    object get_member_variable(const object& obj,
                               const member_variable_tag& tag) const;

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

    template <class InfoType>
    bool check_member_class_type(const object& obj, const InfoType& info) const;

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

    template <class Iterator, class InfoExtractor>
    std::vector<std::vector<std::size_t>>
    indices_by_type(Iterator first,
                    Iterator last,
                    std::size_t num_types,
                    InfoExtractor&& extract_info) const;

    std::size_t index_of_type(const type_tag& tag) const;
    std::size_t index_of_object(const object& obj) const;

private:
    // array_views of reflection information generated at compile time
    helene::array_view<const type_info> type_info_view_;
    helene::array_view<const constructor_info> constructor_info_view_;
    helene::array_view<const conversion_info> conversion_info_view_;
    helene::array_view<const free_function_info> free_function_info_view_;
    helene::array_view<const member_function_info> member_function_info_view_;
    helene::array_view<const member_variable_info> member_variable_info_view_;
    helene::array_view<const serialization_info> serialization_info_view_;

    std::vector<std::vector<std::size_t>> constructor_indices_by_type_;
    std::vector<std::vector<std::size_t>> conversion_indices_by_type_;
    std::vector<std::vector<std::size_t>> member_function_indices_by_type_;
    std::vector<std::vector<std::size_t>> member_variable_indices_by_type_;
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
          class SerializationInfoArray>
inline reflection_manager::reflection_manager(TypeInfoArray& ti_arr,
                                              ConstructorInfoArray& ci_arr,
                                              ConversionInfoArray& cv_arr,
                                              FreeFunctionArray& ff_arr,
                                              MemberFunctionArray& mf_arr,
                                              MemberVariableArray& mv_arr,
                                              SerializationInfoArray& si_arr)
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
      serialization_info_view_(reflection_initialization_detail::array_selector<
                               SerializationInfoArray>::initialize(si_arr)),
      constructor_indices_by_type_(indices_by_type(
          constructor_info_view_.cbegin(),
          constructor_info_view_.cend(),
          type_info_view_.size(),
          [](const constructor_info& info) { return info.type_index; })),
      conversion_indices_by_type_(indices_by_type(
          conversion_info_view_.cbegin(),
          conversion_info_view_.cend(),
          type_info_view_.size(),
          [](const conversion_info& info) { return info.from_type_index; })),
      member_function_indices_by_type_(
          indices_by_type(member_function_info_view_.cbegin(),
                          member_function_info_view_.cend(),
                          type_info_view_.size(),
                          [](const member_function_info& info) {
                              return info.object_type_index;
                          })),
      member_variable_indices_by_type_(
          indices_by_type(member_variable_info_view_.cbegin(),
                          member_variable_info_view_.cend(),
                          type_info_view_.size(),
                          [](const member_variable_info& info) {
                              return info.object_type_index;
                          }))
{
    // sort member variables by offset
    std::for_each(member_variable_indices_by_type_.begin(),
                  member_variable_indices_by_type_.end(),
                  [this](auto& mv_vec) {
                      std::sort(
                          mv_vec.begin(),
                          mv_vec.end(),
                          [this](std::size_t less, std::size_t more) {
                              return member_variable_info_view_[less].offset <
                                     member_variable_info_view_[more].offset;
                          });
                  });
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


template <class InfoType>
inline bool
reflection_manager::check_member_class_type(const object& obj,
                                            const InfoType& info) const
{
    return index_of_object(obj) == info.object_type_index;
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


template <class Iterator, class InfoExtractor>
inline std::vector<std::vector<std::size_t>>
reflection_manager::indices_by_type(Iterator first,
                                    Iterator last,
                                    std::size_t num_types,
                                    InfoExtractor&& extract_info) const
{

    std::vector<std::vector<std::size_t>> out(num_types);
    if(num_types == 0)
    {
        return out;
    }

    for(std::size_t index = 0ul; first != last; ++index, ++first)
    {
        out[extract_info(*first)].push_back(index);
    }

    return out;
}


template <class Iterator>
inline object
reflection_manager::construct_object(const constructor_tag& tag,
                                     Iterator first,
                                     Iterator last) const
{
    if(check_arguments(first, last, *tag.info_ptr_) == false)
    {
        throw argument_error("wrong argument types");
    }

    std::vector<any> arg_vec;
    arg_vec.reserve(std::distance(first, last));

    construct_argument_array(
        first, last, std::back_inserter(arg_vec), *tag.info_ptr_);

    auto return_value = tag.info_ptr_->bind_point(arg_vec.data());

    return object(
        return_value, type_info_view_.data() + tag.info_ptr_->type_index, this);
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


template <class Iterator>
inline object
reflection_manager::call_free_function(const free_function_tag& tag,
                                       Iterator first,
                                       Iterator last) const
{
    if(!check_arguments(first, last, *tag.info_ptr_))
    {
        throw argument_error(
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


template <class Iterator>
inline object
reflection_manager::call_member_function(object& obj,
                                         const member_function_tag& tag,
                                         Iterator first,
                                         Iterator last) const
{
    if(!check_arguments(first, last, *tag.info_ptr_))
    {
        throw argument_error(
            "attempting to call member function with arguments of wrong type");
    }

    std::vector<shadow::any> args;
    args.reserve(std::distance(first, last));

    construct_argument_array(
        first, last, std::back_inserter(args), *tag.info_ptr_);

    auto return_value = tag.info_ptr_->bind_point(obj.value_, args.data());

    pass_parameters_out(args.begin(), args.end(), first, *tag.info_ptr_);

    return object(return_value,
                  type_info_view_.data() + tag.info_ptr_->return_type_index,
                  this);
}
} // namespace shadow
