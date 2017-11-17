#ifndef REFLECTION_INFO_HPP
#define REFLECTION_INFO_HPP


#include <cstddef>
#include <string>

#include <type_descriptor.hpp>

#include "reflection_binding.hpp"


namespace shadow
{
// info about the base type
struct type_info
{
    const char* name;
    std::size_t size;
    address_of_signature address_of_bind_point;
    dereference_signature dereference_bind_point;
};


typedef metamusil::t_descriptor::type_tag type_attribute;

// info about of type and its qualifiers and modifiers
struct type_description
{
    std::size_t type_index;
    std::size_t num_attributes;
    const type_attribute* attributes;
};


struct constructor_info
{
    const type_description* type;
    std::size_t num_parameters;
    const type_description* parameter_types;
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
    const type_description* return_type;
    std::size_t num_parameters;
    const type_description* parameter_types;
    free_function_binding_signature bind_point;
};


struct member_function_info
{
    const char* name;
    const type_description* object_type;
    const type_description* return_type;
    std::size_t num_parameters;
    const type_description* parameter_types;
    member_function_binding_signature bind_point;
};


struct member_variable_info
{
    const char* name;
    std::size_t object_type_index;
    std::size_t type_index;
    std::size_t offset;
    member_variable_get_binding_signature get_bind_point;
    member_variable_set_binding_signature set_bind_point;
};


struct serialization_info
{
    const char* name;
    std::size_t type_index;
    serialization_signature serialization_bind_point;
    deserialization_signature deserialization_bind_point;
};
} // namespace shadow

#endif
