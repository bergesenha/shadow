#pragma once

#include "meta/type_list.hpp"

namespace shadow
{
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
}
