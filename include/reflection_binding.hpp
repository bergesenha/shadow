#pragma once

#include <utility>
#include <type_traits>

#include "any.hpp"
#include "meta/type_list.hpp"
#include "meta/free_function_deduction.hpp"
#include "meta/member_function_deduction.hpp"


namespace shadow
{
typedef any (*free_function_binding_signature)(any*);
typedef any (*member_function_binding_signature)(any&, any*);


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
             t_list::type_list<ArgTypes...>,
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
             t_list::type_list<ArgTypes...>,
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
    typedef t_list::integer_sequence_from_type_list_t<parameter_types>
        parameter_sequence;

    return return_type_specializer<return_type>::
        template dispatch<FunctionPointerType, FunctionPointerValue>(
            argument_array, parameter_types(), parameter_sequence());
}
}


namespace member_function_detail
{


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
    // deduce object type
}
}
}
