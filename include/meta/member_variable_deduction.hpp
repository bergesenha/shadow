#pragma once


namespace shadow
{
template <class MemVarPointerType>
struct member_variable_object_type;

template <class ObjectType, class MemVarType>
struct member_variable_object_type<MemVarType ObjectType::*>
{
    typedef ObjectType type;
};

template <class MemVarPointerType>
using member_variable_object_type_t =
    typename member_variable_object_type<MemVarPointerType>::type;


template <class MemVarPointerType>
struct member_variable_type;

template <class ObjectType, class MemVarType>
struct member_variable_type<MemVarType ObjectType::*>
{
    typedef MemVarType type;
};

template <class MemVarPointerType>
using member_variable_type_t =
    typename member_variable_type<MemVarPointerType>::type;
}
