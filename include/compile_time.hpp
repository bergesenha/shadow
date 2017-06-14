#pragma once

#include <type_list.hpp>

namespace shadow
{
template <std::size_t LineNum>
struct compile_time_type_info;

template <class CompileTimeTypeInfo>
struct extract_name
{
    static constexpr const char* value = CompileTimeTypeInfo::name;
};

template <class TypeListOfCompileTimeTypeInfo>
using generate_vector_of_strings =
    metamusil::t_list::value_transform<TypeListOfCompileTimeTypeInfo,
                                       extract_name>;
}


#define REGISTER_TYPE(type_name)                                               \
    namespace shadow                                                           \
    {                                                                          \
    template <>                                                                \
    struct compile_time_type_info<__LINE__>                                    \
    {                                                                          \
        typedef type_name type;                                                \
        static constexpr char name[] = #type_name;                             \
    };                                                                         \
                                                                               \
    constexpr char compile_time_type_info<__LINE__>::name[];                   \
    }                                                                          \
    \
