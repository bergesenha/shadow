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
    std::pair<type_id_iterator, type_id_iterator> types() const;
    std::pair<constructor_id_iterator, constructor_id_iterator>
    constructors() const;
    std::pair<conversion_id_iterator, conversion_id_iterator>
    conversions() const;
    std::pair<free_function_id_iterator, free_function_id_iterator>
    free_functions() const;
    std::pair<member_function_id_iterator, member_function_id_iterator>
    member_functions() const;
    std::pair<member_variable_id_iterator, member_variable_id_iterator>
    member_variables() const;


    std::string type_name(const type_id& id) const;
    type_id base_type(const type_id& id) const;

    type_id constructor_type(const constructor_id& id) const;

    std::pair<type_id_iterator, type_id_iterator>
    constructor_parameter_types(const constructor_id& id) const;

    std::string free_function_name(const free_function_id& id) const;

    type_id free_function_return_type(const free_function_id& id) const;

    std::pair<type_id_iterator, type_id_iterator>
    free_function_parameter_types(const free_function_id& id) const;

    std::string member_function_name(const member_function_id& id) const;

    type_id member_function_return_type(const member_function_id& id) const;
    type_id member_function_object_type(const member_function_id& id) const;

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
inline std::pair<reflection_manager::type_id_iterator,
                 reflection_manager::type_id_iterator>
reflection_manager::types() const
{
    return std::make_pair(type_id_iterator(base_type_descriptions_.data()),
                          type_id_iterator(base_type_descriptions_.data() +
                                           base_type_descriptions_.size()));
}

inline std::pair<reflection_manager::constructor_id_iterator,
                 reflection_manager::constructor_id_iterator>
reflection_manager::constructors() const
{
    return std::make_pair(
        constructor_id_iterator(constructor_info_view_.data()),
        constructor_id_iterator(constructor_info_view_.data() +
                                constructor_info_view_.size()));
}

inline std::pair<reflection_manager::conversion_id_iterator,
                 reflection_manager::conversion_id_iterator>
reflection_manager::conversions() const
{
    return std::make_pair(conversion_id_iterator(conversion_info_view_.data()),
                          conversion_id_iterator(conversion_info_view_.data() +
                                                 conversion_info_view_.size()));
}

inline std::pair<reflection_manager::free_function_id_iterator,
                 reflection_manager::free_function_id_iterator>
reflection_manager::free_functions() const
{
    return std::make_pair(
        free_function_id_iterator(free_function_info_view_.data()),
        free_function_id_iterator(free_function_info_view_.data() +
                                  free_function_info_view_.size()));
}

inline std::pair<reflection_manager::member_function_id_iterator,
                 reflection_manager::member_function_id_iterator>
reflection_manager::member_functions() const
{
    return std::make_pair(
        member_function_id_iterator(member_function_info_view_.data()),
        member_function_id_iterator(member_function_info_view_.data() +
                                    member_function_info_view_.size()));
}

inline std::pair<reflection_manager::member_variable_id_iterator,
                 reflection_manager::member_variable_id_iterator>
reflection_manager::member_variables() const
{
    return std::make_pair(
        member_variable_id_iterator(member_variable_info_view_.data()),
        member_variable_id_iterator(member_variable_info_view_.data() +
                                    member_variable_info_view_.size()));
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
    return type_id(base_type_descriptions_[id.info_ptr_->type_index]);
}

inline type_id
reflection_manager::constructor_type(const constructor_id& id) const
{
    return type_id(*id.info_ptr_->type);
}

inline std::pair<reflection_manager::type_id_iterator,
                 reflection_manager::type_id_iterator>
reflection_manager::constructor_parameter_types(const constructor_id& id) const
{
    return std::make_pair(type_id_iterator(id.info_ptr_->parameter_types),
                          type_id_iterator(id.info_ptr_->parameter_types +
                                           id.info_ptr_->num_parameters));
}

inline std::string
reflection_manager::free_function_name(const free_function_id& id) const
{
    return id.info_ptr_->name;
}

inline type_id
reflection_manager::free_function_return_type(const free_function_id& id) const
{
    return type_id(*id.info_ptr_->return_type);
}

inline std::pair<reflection_manager::type_id_iterator,
                 reflection_manager::type_id_iterator>
reflection_manager::free_function_parameter_types(
    const free_function_id& id) const
{
    return std::make_pair(type_id_iterator(id.info_ptr_->parameter_types),
                          type_id_iterator(id.info_ptr_->parameter_types +
                                           id.info_ptr_->num_parameters));
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
    return type_id(*id.info_ptr_->return_type);
}

inline type_id
reflection_manager::member_function_object_type(
    const member_function_id& id) const
{
    return type_id(*id.info_ptr_->object_type);
}
}
