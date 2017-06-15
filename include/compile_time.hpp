#pragma once

#include <type_list.hpp>
#include <integer_sequence.hpp>
#include <sfinae.hpp>


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


template <template <std::size_t> class CTI, class IntSeqRange>
struct generate_valid_compile_time_type_infos
{
    template <typename IntSeqRange::value_type LineNum>
    using is_defined_predicate =
        metamusil::integral_specialization_defined<std::size_t, CTI, LineNum>;

    typedef metamusil::int_seq::filter_t<IntSeqRange, is_defined_predicate>
        filtered_line_range;

    typedef metamusil::t_list::from_integer_template_instantiations_t<
        std::size_t,
        CTI,
        filtered_line_range>
        type;
};

template <template <std::size_t> class CTI, class IntSeqRange>
using generate_valid_compile_time_type_infos_t =
    typename generate_valid_compile_time_type_infos<CTI, IntSeqRange>::type;
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
    typedef shadow::generate_valid_compile_time_type_infos_t<                  \
        compile_time_type_info,                                                \
        type_line_range>                                                       \
        instantiated_compile_time_infos;

