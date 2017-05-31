#pragma once


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
}
