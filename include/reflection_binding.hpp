#pragma once

#include "any.hpp"

namespace shadow
{
typedef any (*free_function_binding_signature)(any*);


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
};


template <>
struct return_type_specializer<void>
{
};
}


// the value of the function pointer is stored at runtime in the template
// overload set
// this enables any function to be wrapped into uniform function signature that
// can be stored homogenously at runtime
template <class FunctionPointerType, FunctionPointerType FunctionPointerValue>
any
generic_free_function_bind_point(any* argument_array)
{
    // TODO: implementation
}
}
