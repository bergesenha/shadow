#pragma once

#include <cstddef>

#include "reflection_binding.hpp"


namespace shadow
{
struct type_info
{
    const char* name;
    std::size_t size;
};

struct constructor_info
{
    std::size_t type_index;
    std::size_t num_parameters;
    const std::size_t* parameter_type_indices;
    constructor_binding_signature bind_point;
};

struct conversion_info
{
    std::size_t from_type_index;
    std::size_t to_type_index;
    conversion_binding_signature bind_point;
};

struct free_function_info
{
    const char* name;
    std::size_t return_type_index;
    free_function_binding_signature bind_point;
};

struct member_function_info
{
    const char* name;
    std::size_t object_type_index;
    std::size_t return_type_index;
    std::size_t num_parameters;
    std::size_t* parameter_type_indices;
    member_function_binding_signature bind_point;
};

struct member_variable_info
{
    const char* name;
    std::size_t object_type_index;
    std::size_t type_index;
    member_variable_get_binding_signature get_bind_point;
    member_variable_set_binding_signature set_bind_point;
};
}
