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

inline bool
operator==(const type_info& lhs, const type_info& rhs)
{
    return std::string(lhs.name) == std::string(rhs.name);
}

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
    std::size_t type_index;
    std::size_t num_parameters;
    const std::size_t* parameter_type_indices;
    const bool* parameter_pointer_flags;
    constructor_binding_signature bind_point;
};

inline bool
operator==(const constructor_info& lhs, const constructor_info& rhs)
{
    return lhs.bind_point == rhs.bind_point;
}


struct conversion_info
{
    std::size_t from_type_index;
    std::size_t to_type_index;
    conversion_binding_signature bind_point;
};

inline bool
operator==(const conversion_info& lhs, const conversion_info& rhs)
{
    return lhs.bind_point == rhs.bind_point;
}


struct free_function_info
{
    const char* name;
    std::size_t return_type_index;
    std::size_t num_parameters;
    const std::size_t* parameter_type_indices;
    const bool* parameter_pointer_flags;
    free_function_binding_signature bind_point;
};

inline bool
operator==(const free_function_info& lhs, const free_function_info& rhs)
{
    return lhs.bind_point == rhs.bind_point;
}


struct member_function_info
{
    const char* name;
    std::size_t object_type_index;
    std::size_t return_type_index;
    std::size_t num_parameters;
    const std::size_t* parameter_type_indices;
    const bool* parameter_pointer_flags;
    member_function_binding_signature bind_point;
};

inline bool
operator==(const member_function_info& lhs, const member_function_info& rhs)
{
    return lhs.bind_point == rhs.bind_point;
}


struct member_variable_info
{
    const char* name;
    std::size_t object_type_index;
    std::size_t type_index;
    std::size_t offset;
    member_variable_get_binding_signature get_bind_point;
    member_variable_set_binding_signature set_bind_point;
};

inline bool
operator==(const member_variable_info& lhs, const member_variable_info& rhs)
{
    return lhs.get_bind_point == rhs.get_bind_point &&
           lhs.set_bind_point == rhs.set_bind_point;
}


struct serialization_info
{
    const char* name;
    std::size_t type_index;
    serialization_signature serialization_bind_point;
    deserialization_signature deserialization_bind_point;
};


} // namespace shadow

#endif
