#pragma once

#include <utility>

#include <array_view.hpp>
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
    typedef info_iterator_<const type_info, type_id> type_id_iterator;
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
              array_specializer<MemberVariableInfoArray>::initialize(mv_arr))
    {
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

private:
    helene::array_view<const type_info> type_info_view_;
    helene::array_view<const constructor_info> constructor_info_view_;
    helene::array_view<const conversion_info> conversion_info_view_;
    helene::array_view<const free_function_info> free_function_info_view_;
    helene::array_view<const member_function_info> member_function_info_view_;
    helene::array_view<const member_variable_info> member_variable_info_view_;
};
}


namespace shadow
{
inline std::pair<reflection_manager::type_id_iterator,
                 reflection_manager::type_id_iterator>
reflection_manager::types() const
{
    return std::make_pair(
        type_id_iterator(type_info_view_.data()),
        type_id_iterator(type_info_view_.data() + type_info_view_.size()));
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
}
