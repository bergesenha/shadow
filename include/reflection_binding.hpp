#pragma once

#include "any.hpp"

namespace shadow
{
typedef any (*free_function_binding_signature)(any*);


template <class FunctionPointerType, FunctionPointerType FunctionPointerValue>
any
generic_free_function_bind_point(any* argument_array)
{
    // TODO: implementation
}
}
