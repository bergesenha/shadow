#pragma once

#include <utility>
#include <vector>
#include <numeric>
#include <algorithm>
#include <sstream>

#include <array_view.hpp>
#include <member_iterator.hpp>
#include "reflection_info.hpp"
#include "api_types.hpp"
#include "info_iterators.hpp"
#include "exceptions.hpp"


namespace shadow
{

// array_specializer: handles the case where when T is a pointer instead of an
// array, indicating that the held static member is 'empty', ie. nullptr value.
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


// reflection_manager: handles interaction with the registered reflection
// information.
class reflection_manager
{
    template <class Derived>
    friend struct type_name_policy;

public:
    // member types
    typedef info_iterator_<const type_description, type_id> type_id_iterator;
    typedef info_iterator_<const constructor_info, constructor_id>
        constructor_id_iterator;
    typedef info_iterator_<const conversion_info, conversion_id>
        conversion_id_iterator;
    typedef info_iterator_<const free_function_info, free_function_id>
        free_function_id_iterator;
    typedef info_iterator_<const member_function_info, member_function_id>
        member_function_id_iterator;
    typedef info_iterator_<const member_variable_info, member_variable_id>
        member_variable_id_iterator;

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
                       MemberVariableInfoArray& mv_arr) noexcept
        : type_info_view_(array_specializer<TypeInfoArray>::initialize(ti_arr)),
          constructor_info_view_(
              array_specializer<ConstructorInfoArray>::initialize(ctr_arr)),
          conversion_info_view_(
              array_specializer<ConversionInfoArray>::initialize(cv_arr)),
          free_function_info_view_(
              array_specializer<FreeFunctionInfoArray>::initialize(ff_arr)),
          member_function_info_view_(
              array_specializer<MemberFunctionInfoArray>::initialize(mf_arr)),
          member_variable_info_view_(
              array_specializer<MemberVariableInfoArray>::initialize(mv_arr)),
          base_type_descriptions_(type_info_view_.size(), {0, 0, nullptr})
    {
        typedef helene::member_iterator<std::size_t,
                                        type_description,
                                        std::vector<type_description>::iterator,
                                        &type_description::type_index>
            td_index_iter;

        auto first_td = td_index_iter(base_type_descriptions_.begin());
        auto last_td = td_index_iter(base_type_descriptions_.end());

        std::iota(first_td, last_td, 0);
    }


public:
    // returns a range of iterators to type_id's representing all types
    // registered
    std::pair<type_id_iterator, type_id_iterator> types() const;

    // returns a range of iterators to constructor_id's representing all
    // constructors registered
    std::pair<constructor_id_iterator, constructor_id_iterator>
    constructors() const;

    // returns a range of iterators to conversion_id's representing all implicit
    // conversions between types registered
    std::pair<conversion_id_iterator, conversion_id_iterator>
    conversions() const;

    // returns a range of iterators to free_function_id's representing all
    // registered free functions
    std::pair<free_function_id_iterator, free_function_id_iterator>
    free_functions() const;

    // returns a range of iterators to member_function_id's representing all
    // member functions of all types registered
    std::pair<member_function_id_iterator, member_function_id_iterator>
    member_functions() const;

    // returns a range of iterators to member_variable_id's representing all
    // member variables of all types registered
    std::pair<member_variable_id_iterator, member_variable_id_iterator>
    member_variables() const;

    // returns a type_id representing the type of the given object obj
    type_id object_type(const object& obj) const;

    // returns a string of the name of the type represented by the type_id id
    std::string type_name(const type_id& id) const;

    // returns a type_id representing the base type (the type of id, with all
    // qualifiers and attributes removed)
    type_id base_type(const type_id& id) const;

    // returns a type_id representing the type of an object that would be
    // constructed by the constructor id
    type_id constructor_type(const constructor_id& id) const;

    // returns a range of iterators to type_id's representing the types of the
    // parameters of the given constructor id
    std::pair<type_id_iterator, type_id_iterator>
    constructor_parameter_types(const constructor_id& id) const;

    // returns an object holding an instance of a type that is constructed by
    // the given constructor id, given the range to arguments (of type object)
    template <class Iterator>
    object construct_object(const constructor_id& id,
                            Iterator arg_first,
                            Iterator arg_last) const;

    // returns an object holding an instance of a type that is constructed by
    // the given constructor that takes no arguments
    object construct_object(const constructor_id& id) const;

    // returns a string of the name of the free function represented by the
    // given free_function_id id
    std::string free_function_name(const free_function_id& id) const;

    // returns a type_id representing the return type of the given free function
    // id
    type_id free_function_return_type(const free_function_id& id) const;

    // returns a range to type_id's representing the types of parameters of the
    // given free function id
    std::pair<type_id_iterator, type_id_iterator>
    free_function_parameter_types(const free_function_id& id) const;

    // returns a string of the name of the given member function id
    std::string member_function_name(const member_function_id& id) const;

    // returns a type_id representing the return type of the given member
    // function id
    type_id member_function_return_type(const member_function_id& id) const;

    // returns a type_id representing the type of the class that the given
    // member function id is a member of
    type_id member_function_object_type(const member_function_id& id) const;

    // returns a range of iterators to type_id's representing the parameter
    // types of the given member function id
    std::pair<type_id_iterator, type_id_iterator>
    member_function_parameter_types(const member_function_id& id) const;

    // returns true if the given member function is const
    bool member_function_is_const(const member_function_id& id) const;

    // returns a string of the name of the given member variable
    std::string member_variable_name(const member_variable_id& id) const;

    // returns a type_id representing the type of the given member variable id
    type_id member_variable_type(const member_variable_id& id) const;

    // returns a type_id representing the type of the class that the given
    // member variable id is a member of
    type_id member_variable_object_type(const member_variable_id& id) const;

    // returns the offset of a member variable within its class, equivalent to
    // offsetof
    std::size_t member_variable_offset(const member_variable_id& id) const;

private:
    // views of raw compile time generated information
    helene::array_view<const type_info> type_info_view_;
    helene::array_view<const constructor_info> constructor_info_view_;
    helene::array_view<const conversion_info> conversion_info_view_;
    helene::array_view<const free_function_info> free_function_info_view_;
    helene::array_view<const member_function_info> member_function_info_view_;
    helene::array_view<const member_variable_info> member_variable_info_view_;

    // processed information
    std::vector<type_description> base_type_descriptions_;
};
}

namespace shadow
{
}


namespace shadow
{
inline std::pair<reflection_manager::type_id_iterator,
                 reflection_manager::type_id_iterator>
reflection_manager::types() const
{
    return std::make_pair(
        type_id_iterator(base_type_descriptions_.data(), this),
        type_id_iterator(base_type_descriptions_.data() +
                             base_type_descriptions_.size(),
                         this));
}

inline std::pair<reflection_manager::constructor_id_iterator,
                 reflection_manager::constructor_id_iterator>
reflection_manager::constructors() const
{
    return std::make_pair(
        constructor_id_iterator(constructor_info_view_.data(), this),
        constructor_id_iterator(constructor_info_view_.data() +
                                    constructor_info_view_.size(),
                                this));
}

inline std::pair<reflection_manager::conversion_id_iterator,
                 reflection_manager::conversion_id_iterator>
reflection_manager::conversions() const
{
    return std::make_pair(
        conversion_id_iterator(conversion_info_view_.data(), this),
        conversion_id_iterator(
            conversion_info_view_.data() + conversion_info_view_.size(), this));
}

inline std::pair<reflection_manager::free_function_id_iterator,
                 reflection_manager::free_function_id_iterator>
reflection_manager::free_functions() const
{
    return std::make_pair(
        free_function_id_iterator(free_function_info_view_.data(), this),
        free_function_id_iterator(free_function_info_view_.data() +
                                      free_function_info_view_.size(),
                                  this));
}

inline std::pair<reflection_manager::member_function_id_iterator,
                 reflection_manager::member_function_id_iterator>
reflection_manager::member_functions() const
{
    return std::make_pair(
        member_function_id_iterator(member_function_info_view_.data(), this),
        member_function_id_iterator(member_function_info_view_.data() +
                                        member_function_info_view_.size(),
                                    this));
}

inline std::pair<reflection_manager::member_variable_id_iterator,
                 reflection_manager::member_variable_id_iterator>
reflection_manager::member_variables() const
{
    return std::make_pair(
        member_variable_id_iterator(member_variable_info_view_.data(), this),
        member_variable_id_iterator(member_variable_info_view_.data() +
                                        member_variable_info_view_.size(),
                                    this));
}

inline type_id
reflection_manager::object_type(const object& obj) const
{
    if(obj.has_value())
    {
        return type_id(*obj.type_, *this);
    }
    else
    {
        return type_id(base_type_descriptions_[0], *this);
    }
}

inline std::string
reflection_manager::type_name(const type_id& id) const
{
    std::ostringstream sout;
    sout << type_info_view_[id.info_ptr_->type_index].name;
    std::for_each(id.info_ptr_->attributes,
                  id.info_ptr_->attributes + id.info_ptr_->num_attributes,
                  [&sout](const auto att) {

                      if(att == type_attribute::const_tag)
                      {
                          sout << " const";
                      }
                      if(att == type_attribute::pointer_tag)
                      {
                          sout << "*";
                      }
                      if(att == type_attribute::lreference_tag)
                      {
                          sout << "&";
                      }
                      if(att == type_attribute::rreference_tag)
                      {
                          sout << "&&";
                      }

                  });
    return sout.str();
}

inline type_id
reflection_manager::base_type(const type_id& id) const
{
    return type_id(base_type_descriptions_[id.info_ptr_->type_index], *this);
}

inline type_id
reflection_manager::constructor_type(const constructor_id& id) const
{
    return type_id(*id.info_ptr_->type, *this);
}

inline std::pair<reflection_manager::type_id_iterator,
                 reflection_manager::type_id_iterator>
reflection_manager::constructor_parameter_types(const constructor_id& id) const
{
    return std::make_pair(type_id_iterator(id.info_ptr_->parameter_types, this),
                          type_id_iterator(id.info_ptr_->parameter_types +
                                               id.info_ptr_->num_parameters,
                                           this));
}

template <class Iterator>
inline object
reflection_manager::construct_object(const constructor_id& id,
                                     Iterator arg_first,
                                     Iterator arg_last) const
{
    const auto bind_point = id.info_ptr_->bind_point;
    // TODO: check arguments
    // create argument array
    std::vector<any> args;
    args.reserve(std::distance(arg_first, arg_last));
    std::transform(arg_first,
                   arg_last,
                   std::back_inserter(args),
                   [](const auto& obj) { return obj.value_; });

    return object(bind_point(args.data()), id.info_ptr_->type);
}

inline object
reflection_manager::construct_object(const constructor_id& id) const
{
    if(id.info_ptr_->num_parameters != 0)
    {
        throw argument_error(
            "non-zero-parameter bind point called with no arguments");
    }

    return object(id.info_ptr_->bind_point(nullptr), id.info_ptr_->type);
}


inline std::string
reflection_manager::free_function_name(const free_function_id& id) const
{
    return id.info_ptr_->name;
}

inline type_id
reflection_manager::free_function_return_type(const free_function_id& id) const
{
    return type_id(*id.info_ptr_->return_type, *this);
}

inline std::pair<reflection_manager::type_id_iterator,
                 reflection_manager::type_id_iterator>
reflection_manager::free_function_parameter_types(
    const free_function_id& id) const
{
    return std::make_pair(type_id_iterator(id.info_ptr_->parameter_types, this),
                          type_id_iterator(id.info_ptr_->parameter_types +
                                               id.info_ptr_->num_parameters,
                                           this));
}


inline std::string
reflection_manager::member_function_name(const member_function_id& id) const
{
    return id.info_ptr_->name;
}

inline type_id
reflection_manager::member_function_return_type(
    const member_function_id& id) const
{
    return type_id(*id.info_ptr_->return_type, *this);
}

inline type_id
reflection_manager::member_function_object_type(
    const member_function_id& id) const
{
    return type_id(*id.info_ptr_->object_type, *this);
}

inline std::pair<reflection_manager::type_id_iterator,
                 reflection_manager::type_id_iterator>
reflection_manager::member_function_parameter_types(
    const member_function_id& id) const
{
    return std::make_pair(type_id_iterator(id.info_ptr_->parameter_types, this),
                          type_id_iterator(id.info_ptr_->parameter_types +
                                               id.info_ptr_->num_parameters,
                                           this));
}

inline bool
reflection_manager::member_function_is_const(const member_function_id& id) const
{
    return id.info_ptr_->constness;
}


inline std::string
reflection_manager::member_variable_name(const member_variable_id& id) const
{
    return id.info_ptr_->name;
}

inline type_id
reflection_manager::member_variable_type(const member_variable_id& id) const
{
    return type_id(*id.info_ptr_->type, *this);
}


inline type_id
reflection_manager::member_variable_object_type(
    const member_variable_id& id) const
{
    return type_id(*id.info_ptr_->object_type, *this);
}

inline std::size_t
reflection_manager::member_variable_offset(const member_variable_id& id) const
{
    return id.info_ptr_->offset;
}
}
