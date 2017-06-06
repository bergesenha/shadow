#pragma once

#include <type_list.hpp>

namespace shadow
{
// Deduce return type of pointer to free function.
template <class FunctionPointerType>
struct free_function_return_type;


template <class ReturnType, class... ArgTypes>
struct free_function_return_type<ReturnType (*)(ArgTypes...)>
{
    typedef ReturnType type;
};


template <class FunctionPointerType>
using free_function_return_type_t =
    typename free_function_return_type<FunctionPointerType>::type;


// Deduce parameter types of pointer to free function.
template <class FunctionPointerType>
struct free_function_parameter_types;


template <class ReturnType, class... ParamTypes>
struct free_function_parameter_types<ReturnType (*)(ParamTypes...)>
{
    typedef metamusil::t_list::type_list<ParamTypes...> type;
};

template <class FunctionPointerType>
using free_function_parameter_types_t =
    typename free_function_parameter_types<FunctionPointerType>::type;
}
