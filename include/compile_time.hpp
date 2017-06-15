#pragma once

#include <type_list.hpp>
#include <integer_sequence.hpp>

namespace shadow
{

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

#define REGISTER_TYPE_BEGIN()                                                  \
    constexpr std::size_t type_line_begin = __LINE__;                          \
    template <std::size_t LineNum>                                             \
    struct compile_time_type_info;


#define REGISTER_TYPE(type_name)                                               \
    template <>                                                                \
    struct compile_time_type_info<__LINE__>                                    \
    {                                                                          \
        typedef type_name type;                                                \
        static constexpr char name[] = #type_name;                             \
    };                                                                         \
                                                                               \
    constexpr char compile_time_type_info<__LINE__>::name[];


#define REGISTER_TYPE_END()                                                    \
    constexpr std::size_t type_line_end = __LINE__;                            \
                                                                               \
    typedef metamusil::int_seq::integer_sequence_from_range_t<                 \
        std::size_t,                                                           \
        type_line_begin + 1,                                                   \
        type_line_end>                                                         \
        type_line_range;                                                       \
    \
