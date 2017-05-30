#pragma once

#include <cstddef>
#include "reflection_binding.hpp"


namespace shadow
{
struct type_info
{
    const char* name;
    std::size_t size;
    std::size_t type_index;
};

struct free_function_info
{
    const char* name;              // name of function
    std::size_t return_type_index; // type index of return type
    std::size_t num_parameters;    // number of parameters to function
    const std::size_t*
        argument_type_indices; // array of type indices of parameters
    free_function_binding_signature
        bind_point; // generic bind point for invoking function
};
}
