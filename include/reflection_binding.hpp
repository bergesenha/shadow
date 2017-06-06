#pragma once

#include <utility>
#include <type_traits>

#include "any.hpp"
#include <type_list.hpp>
#include "meta/free_function_deduction.hpp"
#include "meta/member_function_deduction.hpp"
#include "meta/member_variable_deduction.hpp"


namespace shadow
{
// free function signature
typedef any (*free_function_binding_signature)(any*);
// member function signature
typedef any (*member_function_binding_signature)(any&, any*);
// member variable getter
typedef any (*member_variable_get_binding_signature)(const any&);
// member variable setter
typedef void (*member_variable_set_binding_signature)(any&, const any&);


////////////////////////////////////////////////////////////////////////////////
// generic bind point for free functions
// has the same signature as function pointer free_function_binding_signature
// which can be stored in the free_function_info struct


namespace free_function_detail
{
// necessary to handle void as return type differently when calling underlying
// free function
template <class ReturnType>
struct return_type_specializer
{
    // dispatch: unpacks argument type list and sequence to correctly index into
    // argument array and call get with the right types to retrieve raw values
    // from the anys
    template <class FunctionPointerType,
              FunctionPointerType FunctionPointerValue,
              class... ArgTypes,
              std::size_t... ArgSeq>
    static any
    dispatch(any* argument_array,
             metamusil::t_list::type_list<ArgTypes...>,
             std::index_sequence<ArgSeq...>)
    {
        // necessary to remove reference from types as any only stores
        // unqualified types, ie values only
        return FunctionPointerValue(
            argument_array[ArgSeq]
                .get<typename std::remove_reference_t<ArgTypes>>()...);
    }
};


template <>
struct return_type_specializer<void>
{
    // dispatch: unpacks argument type list and sequence to correctly index into
    // argument array and call get with the right types to retrieve raw values
    // from the anys
    template <class FunctionPointerType,
              FunctionPointerType FunctionPointerValue,
              class... ArgTypes,
              std::size_t... ArgSeq>
    static any
    dispatch(any* argument_array,
             metamusil::t_list::type_list<ArgTypes...>,
             std::index_sequence<ArgSeq...>)
    {
        // necessary to remove reference from types as any only stores
        // unqualified types, ie values only
        FunctionPointerValue(
            argument_array[ArgSeq]
                .get<typename std::remove_reference_t<ArgTypes>>()...);

        // return empty any, ie 'void'
        return any();
    }
};


// the value of the function pointer is stored at runtime in the template
// overload set
// this enables any function to be wrapped into uniform function signature that
// can be stored homogenously at runtime
template <class FunctionPointerType, FunctionPointerType FunctionPointerValue>
any
generic_free_function_bind_point(any* argument_array)
{
    typedef free_function_return_type_t<FunctionPointerType> return_type;
    typedef free_function_parameter_types_t<FunctionPointerType>
        parameter_types;
    // make integer sequence from type list
    typedef metamusil::t_list::index_sequence_for_t<parameter_types>
        parameter_sequence;

    return return_type_specializer<return_type>::
        template dispatch<FunctionPointerType, FunctionPointerValue>(
            argument_array, parameter_types(), parameter_sequence());
}
}


namespace member_function_detail
{

template <class ReturnType>
struct return_type_specializer
{
    template <class MemFunPointerType,
              MemFunPointerType MemFunPointerValue,
              class ObjectType,
              class... ParamTypes,
              std::size_t... ParamSequence>
    static any
    dispatch(any& object,
             any* argument_array,
             metamusil::t_list::type_list<ParamTypes...>,
             std::index_sequence<ParamSequence...>)
    {
        return (object.get<ObjectType>().*MemFunPointerValue)(
            argument_array[ParamSequence]
                .get<std::remove_reference_t<ParamTypes>>()...);
    }
};


template <>
struct return_type_specializer<void>
{
    template <class MemFunPointerType,
              MemFunPointerType MemFunPointerValue,
              class ObjectType,
              class... ParamTypes,
              std::size_t... ParamSequence>
    static any
    dispatch(any& object,
             any* argument_array,
             metamusil::t_list::type_list<ParamTypes...>,
             std::index_sequence<ParamSequence...>)
    {
        (object.get<ObjectType>().*MemFunPointerValue)(
            argument_array[ParamSequence]
                .get<std::remove_reference_t<ParamTypes>>()...);

        return any();
    }
};


template <class MemFunPointerType, MemFunPointerType MemFunPointerValue>
any
generic_member_function_bind_point(any& object, any* argument_array)
{
    // deduce return type
    typedef member_function_return_type_t<MemFunPointerType> return_type;

    // deduce parameter types
    typedef member_function_parameter_types_t<MemFunPointerType>
        parameter_types;

    // make integer sequence from parameter type list
    typedef metamusil::t_list::index_sequence_for_t<parameter_types>
        parameter_sequence;

    // deduce object type
    typedef member_function_object_type_t<MemFunPointerType> object_type;

    return return_type_specializer<return_type>::
        template dispatch<MemFunPointerType, MemFunPointerValue, object_type>(
            object, argument_array, parameter_types(), parameter_sequence());
}
}


namespace member_variable_detail
{

template <class MemVarPointerType,
          MemVarPointerType MemVarPointerValue,
          class ObjectType,
          class MemVarType>
any
get_dispatch(const any& object)
{
    return (object.get<ObjectType>().*MemVarPointerValue);
}

template <class MemVarPointerType,
          MemVarPointerType MemVarPointerValue,
          class ObjectType,
          class MemVarType>
void
set_dispatch(any& object, const any& value)
{
    (object.get<ObjectType>().*MemVarPointerValue) = value.get<MemVarType>();
}


template <class MemVarPointerType, MemVarPointerType MemVarPointerValue>
any
generic_member_variable_get_bind_point(const any& object)
{
    typedef member_variable_object_type_t<MemVarPointerType> object_type;
    typedef member_variable_type_t<MemVarPointerType> member_variable_type;

    return get_dispatch<MemVarPointerType,
                        MemVarPointerValue,
                        object_type,
                        member_variable_type>(object);
}


template <class MemVarPointerType, MemVarPointerType MemVarPointerValue>
void
generic_member_variable_set_bind_point(any& object, const any& value)
{
    typedef member_variable_object_type_t<MemVarPointerType> object_type;
    typedef member_variable_type_t<MemVarPointerType> member_variable_type;

    return set_dispatch<MemVarPointerType,
                        MemVarPointerValue,
                        object_type,
                        member_variable_type>(object, value);
}
}
}
