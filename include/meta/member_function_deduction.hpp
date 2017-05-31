#pragma once

#include "meta/type_list.hpp"


namespace shadow
{
template <class MemFunPointerType>
struct member_function_return_type;

template <class ReturnType, class ObjectType, class... ParamTypes>
struct member_function_return_type<ReturnType (ObjectType::*)(ParamTypes...)>
{
    typedef ReturnType type;
};

template <class MemFunPointerType>
using member_function_return_type_t =
    typename member_function_return_type<MemFunPointerType>::type;


template <class MemFunPointerType>
struct member_function_parameter_types;

template <class ReturnType, class ObjectType, class... ParamTypes>
struct member_function_parameter_types<ReturnType (ObjectType::*)(
    ParamTypes...)>
{
    typedef t_list::type_list<ParamTypes...> type;
};

template <class MemFunPointerType>
using member_function_parameter_types_t =
    typename member_function_parameter_types<MemFunPointerType>::type;


template <class MemFunPointerType>
struct member_function_object_type;

template <class ReturnType, class ObjectType, class... ParamTypes>
struct member_function_object_type<ReturnType (ObjectType::*)(ParamTypes...)>
{
    typedef ObjectType type;
};

template <class MemFunPointerType>
using member_function_object_type_t =
    typename member_function_object_type<MemFunPointerType>::type;
}
