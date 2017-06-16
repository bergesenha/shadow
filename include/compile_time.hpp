#pragma once

#include <type_list.hpp>
#include <integer_sequence.hpp>
#include <sfinae.hpp>

#include "reflection_info.hpp"
#include "director.hpp"


namespace shadow
{
// generate type_list of instantiated compile_time_type_info to feed
// generate_array_of_type_info and generate_array_of_strings
template <template <std::size_t> class CTI, class IntSeqRange>
struct generate_valid_compile_time_infos
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
using generate_valid_compile_time_infos_t =
    typename generate_valid_compile_time_infos<CTI, IntSeqRange>::type;

// extract name and make an array of type names at compile time
template <class CompileTimeTypeInfo>
struct extract_name
{
    static constexpr const char* value = CompileTimeTypeInfo::name;
};

template <class TypeListOfCompileTimeTypeInfo>
using generate_array_of_strings =
    metamusil::t_list::value_transform<TypeListOfCompileTimeTypeInfo,
                                       extract_name>;

// extract type info and make array of type_info at compile time
template <class CompileTimeTypeInfo>
struct extract_type_info
{
    static constexpr type_info value = {CompileTimeTypeInfo::name,
                                        CompileTimeTypeInfo::size};
};

template <class TypeListOfCompileTimeTypeInfo>
using generate_array_of_type_info =
    metamusil::t_list::value_transform<TypeListOfCompileTimeTypeInfo,
                                       extract_type_info>;


// extract type from compile_time_info
template <class CTI>
using extract_type = typename CTI::type;


// extract dynamic constructor_info from compile_time_constructor info template
// instantiations
template <class CTCI>
struct extract_constructor_info
{
    static constexpr constructor_info value = {
        CTCI::type_index,
        CTCI::num_parameters,
        CTCI::parameter_type_indices_holder::value,
        CTCI::bind_point};
};

template <class TypeListOfCompileTimeConstructorInfo>
using generate_array_of_constructor_info =
    metamusil::t_list::value_transform<TypeListOfCompileTimeConstructorInfo,
                                       extract_constructor_info>;
}


////////////////////////////////////////////////////////////////////////////////
// Type registration
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
        static const std::size_t size = sizeof(type_name);                     \
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
    typedef shadow::generate_valid_compile_time_infos_t<                       \
        compile_time_type_info,                                                \
        type_line_range>                                                       \
        instantiated_compile_time_infos;                                       \
                                                                               \
    typedef shadow::generate_array_of_strings<instantiated_compile_time_infos> \
        type_name_array_holder;                                                \
                                                                               \
    typedef shadow::generate_array_of_type_info<                               \
        instantiated_compile_time_infos>                                       \
        type_info_array_holder;                                                \
                                                                               \
    typedef metamusil::t_list::type_transform_t<                               \
        instantiated_compile_time_infos,                                       \
        shadow::extract_type>                                                  \
        type_universe;


////////////////////////////////////////////////////////////////////////////////
// Constructor registration
#define REGISTER_CONSTRUCTOR_BEGIN()                                           \
    constexpr std::size_t constructor_line_begin = __LINE__;                   \
                                                                               \
    template <std::size_t>                                                     \
    struct compile_time_constructor_info;


#define REGISTER_CONSTRUCTOR(type_name, ...)                                   \
    template <>                                                                \
    struct compile_time_constructor_info<__LINE__>                             \
    {                                                                          \
        static const std::size_t type_index =                                  \
            metamusil::t_list::index_of_type_v<type_universe, type_name>;      \
                                                                               \
        typedef metamusil::t_list::type_list<__VA_ARGS__> parameter_type_list; \
                                                                               \
        static const std::size_t num_parameters =                              \
            metamusil::t_list::length_v<parameter_type_list>;                  \
                                                                               \
        typedef metamusil::t_list::order_t<parameter_type_list, type_universe> \
            parameter_index_sequence;                                          \
                                                                               \
        typedef metamusil::int_seq::integer_sequence_to_array<                 \
            parameter_index_sequence>                                          \
            parameter_type_indices_holder;                                     \
                                                                               \
        static constexpr shadow::constructor_binding_signature bind_point =    \
            &shadow::constructor_detail::                                      \
                generic_constructor_bind_point<type_name, __VA_ARGS__>;        \
    };


#define REGISTER_CONSTRUCTOR_END()                                             \
    constexpr std::size_t constructor_line_end = __LINE__;                     \
                                                                               \
    typedef metamusil::int_seq::integer_sequence_from_range_t<                 \
        std::size_t,                                                           \
        constructor_line_begin + 1,                                            \
        constructor_line_end>                                                  \
        constructor_line_range;                                                \
                                                                               \
    typedef shadow::generate_valid_compile_time_infos_t<                       \
        compile_time_constructor_info,                                         \
        constructor_line_range>                                                \
        instantiated_compile_time_constructor_infos;                           \
                                                                               \
    typedef shadow::generate_array_of_constructor_info<                        \
        instantiated_compile_time_constructor_infos>                           \
        constructor_info_array_holder;


////////////////////////////////////////////////////////////////////////////////
// Initialize Shadow reflection library
#define SHADOW_INIT()                                                          \
    static const shadow::director reflection_director{                         \
        type_name_array_holder(),                                              \
        type_info_array_holder(),                                              \
        constructor_info_array_holder()};
