#ifndef COMPILE_TIME_HPP
#define COMPILE_TIME_HPP


#include <string>

#include <function_deduction.hpp>
#include <integer_sequence.hpp>
#include <sfinae.hpp>
#include <type_list.hpp>

#include "reflection_info.hpp"
#include "reflection_manager.hpp"
#include "exceptions.hpp"


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
    metamusil::t_list::explicit_value_transform<TypeListOfCompileTimeTypeInfo,
                                                extract_name,
                                                const char*>;

// extract type info and make array of type_info at compile time
template <class CompileTimeTypeInfo>
struct extract_type_info
{
    static constexpr type_info value = {
        static_cast<const char*>(CompileTimeTypeInfo::name),
        CompileTimeTypeInfo::size,
        &pointer_detail::generic_address_of_bind_point<
            typename CompileTimeTypeInfo::type>,
        &pointer_detail::generic_dereference_bind_point<
            typename CompileTimeTypeInfo::type>};
};

template <class TypeListOfCompileTimeTypeInfo>
using generate_array_of_type_info =
    metamusil::t_list::explicit_value_transform<TypeListOfCompileTimeTypeInfo,
                                                extract_type_info,
                                                type_info>;


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
        static_cast<const std::size_t*>(
            CTCI::parameter_type_indices_holder::value),
        static_cast<const bool*>(CTCI::parameter_pointer_flags_holder::value),
        CTCI::bind_point};
};

template <class TypeListOfCompileTimeConstructorInfo>
using generate_array_of_constructor_info =
    metamusil::t_list::explicit_value_transform<
        TypeListOfCompileTimeConstructorInfo,
        extract_constructor_info,
        constructor_info>;


template <class ResultType, class ParamTypeList>
struct constructor_bind_point_from_type_list;

template <class ResultType, class... ParamTypes>
struct constructor_bind_point_from_type_list<
    ResultType,
    metamusil::t_list::type_list<ParamTypes...>>
{
    static constexpr shadow::constructor_binding_signature value =
        &shadow::constructor_detail::
            generic_constructor_bind_point<ResultType, ParamTypes...>;
};

template <class ResultType, class ParamTypeList>
constexpr shadow::constructor_binding_signature
    constructor_bind_point_from_type_list_v =
        constructor_bind_point_from_type_list<ResultType, ParamTypeList>::value;


// template for holding type combinations
template <class T1, class T2>
struct type_pair
{
};

// filter for valid conversion combinations
template <class CombinationList>
struct filter_valid_combinations
{
    template <class Pair>
    struct is_defined_predicate;

    template <class A, class B>
    struct is_defined_predicate<type_pair<A, B>>
    {
        static const bool value = metamusil::specialization_defined_v<
            shadow::conversion_detail::conversion_specializer,
            A,
            B>;
    };

    typedef metamusil::t_list::filter_t<CombinationList, is_defined_predicate>
        type;
};

template <class CombinationList>
using filter_valid_combinations_t =
    typename filter_valid_combinations<CombinationList>::type;


// extract conversion_struct from type_pair
template <class TypePair, class TypeUniverseList>
struct extract_conversion_info;

template <class To, class From, class AllTypesList>
struct extract_conversion_info<type_pair<To, From>, AllTypesList>
{
    static constexpr conversion_info value = {
        metamusil::t_list::index_of_type_v<AllTypesList, From>,
        metamusil::t_list::index_of_type_v<AllTypesList, To>,
        &shadow::conversion_detail::generic_conversion_bind_point<To, From>};
};

template <class To, class From, class AllTypesList>
constexpr conversion_info
    extract_conversion_info<type_pair<To, From>, AllTypesList>::value;


// generate type_description from type
template <class T, class AllTypesList>
struct generate_type_description
{
    typedef metamusil::t_descriptor::decompose_t<T> decomposed_type;

    typedef metamusil::t_descriptor::array_from_descriptor<decomposed_type>
        descriptor_array_holder;

    static constexpr type_description value = {
        metamusil::t_list::index_of_type_v<
            AllTypesList,
            metamusil::t_descriptor::base_type_t<decomposed_type>>,
        std::extent<decltype(descriptor_array_holder::value)>::value,
        static_cast<const type_attribute*>(descriptor_array_holder::value)};
};

template <class T, class AllTypesList>
constexpr type_description generate_type_description<T, AllTypesList>::value;

template <class T, class AllTypesList>
constexpr type_description generate_type_description_v =
    generate_type_description<T, AllTypesList>::value;


// extract free_function_info from compile_time_ff_info
template <class CTFFI>
struct extract_free_function_info
{
    static constexpr shadow::free_function_info value = {
        CTFFI::name,
        CTFFI::return_type_index,
        metamusil::t_list::length_v<typename CTFFI::parameter_list>,
        CTFFI::parameter_type_indices_holder::value,
        CTFFI::parameter_pointer_flags_holder::value,
        CTFFI::bind_point};
};

template <class CompileTimeFfInfoList>
using generate_array_of_ff_info =
    metamusil::t_list::explicit_value_transform<CompileTimeFfInfoList,
                                                extract_free_function_info,
                                                free_function_info>;


template <class CTMFI>
struct extract_member_function_info
{
    static constexpr shadow::member_function_info value = {
        CTMFI::name,
        CTMFI::object_type_index,
        CTMFI::return_type_index,
        CTMFI::num_parameters,
        CTMFI::parameter_type_indices_holder::value,
        CTMFI::parameter_pointer_flags_holder::value,
        CTMFI::bind_point};
};

template <class CompileTimeMfInfoList>
using generate_array_of_mf_info =
    metamusil::t_list::explicit_value_transform<CompileTimeMfInfoList,
                                                extract_member_function_info,
                                                member_function_info>;

template <class CTMVI>
struct extract_member_variable_info
{
    static constexpr member_variable_info value = {CTMVI::name,
                                                   CTMVI::object_type_index,
                                                   CTMVI::type_index,
                                                   CTMVI::offset,
                                                   CTMVI::get_bind_point,
                                                   CTMVI::set_bind_point};
};

template <class CompileTimeMvInfoList>
using generate_array_of_mv_info =
    metamusil::t_list::explicit_value_transform<CompileTimeMvInfoList,
                                                extract_member_variable_info,
                                                member_variable_info>;


template <class Types>
struct generate_array_of_serialization_info
{
    // filter for types that has serialization_type_selector defined
    template <class T>
    using is_defined_predicate = metamusil::specialization_defined<
        serialization_detail::serialization_type_selector,
        T>;

    typedef metamusil::t_list::filter_t<Types, is_defined_predicate>
        serializable_types;

    template <class T>
    struct extract_serialization_info
    {
        static constexpr shadow::serialization_info value = {
            "default",
            metamusil::t_list::index_of_type_v<Types, T>,
            &serialization_detail::generic_serialization_bind_point<T>,
            &serialization_detail::generic_deserialization_bind_point<T>};
    };


    typedef metamusil::t_list::explicit_value_transform<
        serializable_types,
        extract_serialization_info,
        serialization_info>
        type;
};

template <class Types>
using generate_array_of_serialization_info_t =
    typename generate_array_of_serialization_info<Types>::type;


} // namespace shadow


////////////////////////////////////////////////////////////////////////////////
// Type registration
#define REGISTER_TYPE_BEGIN()                                                  \
    /* set beginning line number to search from */                             \
    constexpr std::size_t type_line_begin = __LINE__;                          \
    /* declaration of compile time info later specialized for each line item*/ \
    template <std::size_t LineNum>                                             \
    struct compile_time_type_info;


#define REGISTER_TYPE(type_name)                                               \
    /* specialization contains type information */                             \
    template <>                                                                \
    struct compile_time_type_info<__LINE__>                                    \
    {                                                                          \
        typedef type_name type;                                                \
        static constexpr char name[] = #type_name;                             \
        static const std::size_t size = sizeof(type_name);                     \
    };                                                                         \
                                                                               \
    /* definition required for static member */                                \
    constexpr char compile_time_type_info<__LINE__>::name[];


// used internally to register fundamental types
#define REGISTER_FUNDAMENTAL_BEGIN()                                           \
    template <class T>                                                         \
    struct fundamental_compile_time_info;                                      \
                                                                               \
    typedef metamusil::t_list::type_list<void,                                 \
                                         std::nullptr_t,                       \
                                         bool,                                 \
                                         signed char,                          \
                                         unsigned char,                        \
                                         char,                                 \
                                         wchar_t,                              \
                                         char16_t,                             \
                                         char32_t,                             \
                                         short int,                            \
                                         unsigned short int,                   \
                                         int,                                  \
                                         unsigned int,                         \
                                         long int,                             \
                                         unsigned long int,                    \
                                         long long int,                        \
                                         unsigned long long int,               \
                                         float,                                \
                                         double,                               \
                                         long double,                          \
                                         std::string>                          \
        fundamental_type_universe;                                             \
                                                                               \
    template <>                                                                \
    struct fundamental_compile_time_info<void>                                 \
    {                                                                          \
        typedef void type;                                                     \
        static constexpr char name[] = "void";                                 \
        static const std::size_t size = 0;                                     \
    };                                                                         \
    constexpr char fundamental_compile_time_info<void>::name[];


#define REGISTER_FUNDAMENTAL(type_name)                                        \
    template <>                                                                \
    struct fundamental_compile_time_info<type_name>                            \
    {                                                                          \
        typedef type_name type;                                                \
        static constexpr char name[] = #type_name;                             \
        static const std::size_t size = sizeof(type_name);                     \
    };                                                                         \
                                                                               \
    constexpr char fundamental_compile_time_info<type_name>::name[];


#define REGISTER_FUNDAMENTAL_END()                                             \
    typedef metamusil::t_list::type_transform_t<fundamental_type_universe,     \
                                                fundamental_compile_time_info> \
        instantiated_fundamental_compile_time_infos;


#define REGISTER_TYPE_END()                                                    \
    /* register fundamental types */                                           \
    REGISTER_FUNDAMENTAL_BEGIN()                                               \
    REGISTER_FUNDAMENTAL(std::nullptr_t)                                       \
    REGISTER_FUNDAMENTAL(bool)                                                 \
    REGISTER_FUNDAMENTAL(signed char)                                          \
    REGISTER_FUNDAMENTAL(unsigned char)                                        \
    REGISTER_FUNDAMENTAL(char)                                                 \
    REGISTER_FUNDAMENTAL(wchar_t)                                              \
    REGISTER_FUNDAMENTAL(char16_t)                                             \
    REGISTER_FUNDAMENTAL(char32_t)                                             \
    REGISTER_FUNDAMENTAL(short int)                                            \
    REGISTER_FUNDAMENTAL(unsigned short int)                                   \
    REGISTER_FUNDAMENTAL(int)                                                  \
    REGISTER_FUNDAMENTAL(unsigned int)                                         \
    REGISTER_FUNDAMENTAL(long int)                                             \
    REGISTER_FUNDAMENTAL(unsigned long int)                                    \
    REGISTER_FUNDAMENTAL(long long int)                                        \
    REGISTER_FUNDAMENTAL(unsigned long long int)                               \
    REGISTER_FUNDAMENTAL(float)                                                \
    REGISTER_FUNDAMENTAL(double)                                               \
    REGISTER_FUNDAMENTAL(long double)                                          \
    REGISTER_FUNDAMENTAL(std::string)                                          \
    REGISTER_FUNDAMENTAL_END()                                                 \
                                                                               \
    /* set ending line number to stop search at */                             \
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
    typedef metamusil::t_list::concat_t<                                       \
        instantiated_fundamental_compile_time_infos,                           \
        instantiated_compile_time_infos>                                       \
        all_compile_time_infos;                                                \
                                                                               \
    typedef shadow::generate_array_of_strings<all_compile_time_infos>          \
        type_name_array_holder;                                                \
                                                                               \
    typedef shadow::generate_array_of_type_info<all_compile_time_infos>        \
        type_info_array_holder;                                                \
                                                                               \
    typedef metamusil::t_list::type_transform_t<all_compile_time_infos,        \
                                                shadow::extract_type>          \
        type_universe;                                                         \
                                                                               \
    REGISTER_CONSTRUCTOR_BEGIN()                                               \
    REGISTER_FREE_FUNCTION_BEGIN()                                             \
    REGISTER_MEMBER_FUNCTION_BEGIN()                                           \
    REGISTER_MEMBER_VARIABLE_BEGIN()


////////////////////////////////////////////////////////////////////////////////
// Constructor registration
#define REGISTER_CONSTRUCTOR_BEGIN()                                           \
    constexpr std::size_t constructor_line_begin = __LINE__;                   \
                                                                               \
    template <std::size_t>                                                     \
    struct compile_time_constructor_info;


#define REGISTER_FUNDAMENTAL_CONSTRUCTORS()                                    \
    template <class ResultType, class... ParamTypes>                           \
    struct fundamental_compile_time_constructor_info                           \
    {                                                                          \
        static const std::size_t type_index =                                  \
            metamusil::t_list::index_of_type_v<type_universe, ResultType>;     \
                                                                               \
        typedef metamusil::t_list::type_list<ParamTypes...>                    \
            parameter_type_list;                                               \
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
        typedef metamusil::t_list::value_transform<parameter_type_list,        \
                                                   std::is_pointer>            \
            parameter_pointer_flags_holder;                                    \
                                                                               \
        static constexpr shadow::constructor_binding_signature bind_point =    \
            shadow::constructor_bind_point_from_type_list_v<                   \
                ResultType,                                                    \
                parameter_type_list>;                                          \
    };                                                                         \
                                                                               \
    typedef metamusil::t_list::type_list<                                      \
        fundamental_compile_time_constructor_info<std::nullptr_t>,             \
        fundamental_compile_time_constructor_info<bool>,                       \
        fundamental_compile_time_constructor_info<signed char>,                \
        fundamental_compile_time_constructor_info<unsigned char>,              \
        fundamental_compile_time_constructor_info<char>,                       \
        fundamental_compile_time_constructor_info<wchar_t>,                    \
        fundamental_compile_time_constructor_info<char16_t>,                   \
        fundamental_compile_time_constructor_info<char32_t>,                   \
        fundamental_compile_time_constructor_info<short int>,                  \
        fundamental_compile_time_constructor_info<unsigned short int>,         \
        fundamental_compile_time_constructor_info<int>,                        \
        fundamental_compile_time_constructor_info<unsigned int>,               \
        fundamental_compile_time_constructor_info<long int>,                   \
        fundamental_compile_time_constructor_info<unsigned long int>,          \
        fundamental_compile_time_constructor_info<long long int>,              \
        fundamental_compile_time_constructor_info<unsigned long long int>,     \
        fundamental_compile_time_constructor_info<float>,                      \
        fundamental_compile_time_constructor_info<double>,                     \
        fundamental_compile_time_constructor_info<long double>,                \
        fundamental_compile_time_constructor_info<std::string>,                \
                                                                               \
                                                                               \
        fundamental_compile_time_constructor_info<std::nullptr_t,              \
                                                  std::nullptr_t>,             \
        fundamental_compile_time_constructor_info<bool, bool>,                 \
        fundamental_compile_time_constructor_info<signed char, signed char>,   \
        fundamental_compile_time_constructor_info<unsigned char,               \
                                                  unsigned char>,              \
        fundamental_compile_time_constructor_info<char, char>,                 \
        fundamental_compile_time_constructor_info<wchar_t, wchar_t>,           \
        fundamental_compile_time_constructor_info<char16_t, char16_t>,         \
        fundamental_compile_time_constructor_info<char32_t, char32_t>,         \
        fundamental_compile_time_constructor_info<short int, short int>,       \
        fundamental_compile_time_constructor_info<unsigned short int,          \
                                                  unsigned short int>,         \
        fundamental_compile_time_constructor_info<int, int>,                   \
        fundamental_compile_time_constructor_info<unsigned int, unsigned int>, \
        fundamental_compile_time_constructor_info<long int, long int>,         \
        fundamental_compile_time_constructor_info<unsigned long int,           \
                                                  unsigned long int>,          \
        fundamental_compile_time_constructor_info<long long int,               \
                                                  long long int>,              \
        fundamental_compile_time_constructor_info<unsigned long long int,      \
                                                  unsigned long long int>,     \
        fundamental_compile_time_constructor_info<float, float>,               \
        fundamental_compile_time_constructor_info<double, double>,             \
        fundamental_compile_time_constructor_info<long double, long double>,   \
        fundamental_compile_time_constructor_info<std::string, std::string>>   \
        instantiated_fundamental_compile_time_constructor_infos;


#define REGISTER_CONSTRUCTOR(type_name, ...)                                   \
    template <>                                                                \
    struct compile_time_constructor_info<__LINE__>                             \
    {                                                                          \
        static const std::size_t type_index =                                  \
            metamusil::t_list::index_of_type_v<type_universe, type_name>;      \
                                                                               \
        typedef metamusil::t_list::type_list<__VA_ARGS__> parameter_type_list; \
                                                                               \
        typedef metamusil::t_list::type_transform_t<parameter_type_list,       \
                                                    metamusil::base_t>         \
            base_parameter_type_list;                                          \
                                                                               \
        static const std::size_t num_parameters =                              \
            metamusil::t_list::length_v<parameter_type_list>;                  \
                                                                               \
        typedef metamusil::t_list::order_t<base_parameter_type_list,           \
                                           type_universe>                      \
            parameter_index_sequence;                                          \
                                                                               \
        typedef metamusil::int_seq::integer_sequence_to_array<                 \
            parameter_index_sequence>                                          \
            parameter_type_indices_holder;                                     \
                                                                               \
        typedef metamusil::t_list::value_transform<parameter_type_list,        \
                                                   std::is_pointer>            \
            parameter_pointer_flags_holder;                                    \
                                                                               \
        static constexpr shadow::constructor_binding_signature bind_point =    \
            shadow::constructor_bind_point_from_type_list_v<                   \
                type_name,                                                     \
                parameter_type_list>;                                          \
    };


#define REGISTER_CONSTRUCTOR_END()                                             \
                                                                               \
    REGISTER_FUNDAMENTAL_CONSTRUCTORS()                                        \
                                                                               \
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
    typedef metamusil::t_list::concat_t<                                       \
        instantiated_fundamental_compile_time_constructor_infos,               \
        instantiated_compile_time_constructor_infos>                           \
        all_instantiated_compile_time_constructor_infos;                       \
                                                                               \
    typedef shadow::generate_array_of_constructor_info<                        \
        all_instantiated_compile_time_constructor_infos>                       \
        constructor_info_array_holder;


////////////////////////////////////////////////////////////////////////////////
// register free functions

#define REGISTER_FREE_FUNCTION_BEGIN()                                         \
    constexpr std::size_t ff_line_begin = __LINE__;                            \
                                                                               \
    template <std::size_t>                                                     \
    struct compile_time_ff_info;                                               \
                                                                               \
    template <std::size_t>                                                     \
    struct exp_compile_time_ff_info;


#define REGISTER_FREE_FUNCTION(function_name)                                  \
                                                                               \
    template <>                                                                \
    struct compile_time_ff_info<__LINE__>                                      \
    {                                                                          \
        static constexpr char name[] = #function_name;                         \
                                                                               \
        typedef decltype(&function_name) function_pointer_type;                \
                                                                               \
        static const std::size_t return_type_index =                           \
            metamusil::t_list::index_of_type_v<                                \
                type_universe,                                                 \
                metamusil::deduce_return_type_t<function_pointer_type>>;       \
                                                                               \
        typedef metamusil::deduce_parameter_types_t<function_pointer_type>     \
            parameter_list;                                                    \
                                                                               \
        typedef metamusil::t_list::type_transform_t<parameter_list,            \
                                                    metamusil::base_t>         \
            base_parameter_list;                                               \
                                                                               \
        typedef metamusil::t_list::order_t<base_parameter_list, type_universe> \
            parameter_index_sequence;                                          \
                                                                               \
        typedef metamusil::int_seq::integer_sequence_to_array<                 \
            parameter_index_sequence>                                          \
            parameter_type_indices_holder;                                     \
                                                                               \
        typedef metamusil::t_list::value_transform<parameter_list,             \
                                                   std::is_pointer>            \
            parameter_pointer_flags_holder;                                    \
                                                                               \
        static constexpr shadow::free_function_binding_signature bind_point =  \
            &shadow::free_function_detail::generic_free_function_bind_point<   \
                decltype(&function_name),                                      \
                &function_name>;                                               \
    };                                                                         \
                                                                               \
    constexpr char compile_time_ff_info<__LINE__>::name[];


#define REGISTER_FREE_FUNCTION_EXPLICIT(function_name, return_type, ...)       \
                                                                               \
    template <>                                                                \
    struct exp_compile_time_ff_info<__LINE__>                                  \
    {                                                                          \
        static constexpr char name[] = #function_name;                         \
                                                                               \
        typedef return_type (*function_pointer_type)(__VA_ARGS__);             \
                                                                               \
        static const std::size_t return_type_index =                           \
            metamusil::t_list::index_of_type_v<type_universe, return_type>;    \
                                                                               \
        typedef metamusil::t_list::type_list<__VA_ARGS__> parameter_list;      \
                                                                               \
        typedef metamusil::t_list::type_transform_t<parameter_list,            \
                                                    metamusil::base_t>         \
            base_parameter_list;                                               \
                                                                               \
        typedef metamusil::t_list::order_t<base_parameter_list, type_universe> \
            parameter_index_sequence;                                          \
                                                                               \
        typedef metamusil::int_seq::integer_sequence_to_array<                 \
            parameter_index_sequence>                                          \
            parameter_type_indices_holder;                                     \
                                                                               \
        typedef metamusil::t_list::value_transform<parameter_list,             \
                                                   std::is_pointer>            \
            parameter_pointer_flags_holder;                                    \
                                                                               \
        static constexpr shadow::free_function_binding_signature bind_point =  \
            &shadow::free_function_detail::generic_free_function_bind_point<   \
                function_pointer_type,                                         \
                &function_name>;                                               \
    };                                                                         \
                                                                               \
    constexpr char exp_compile_time_ff_info<__LINE__>::name[];


#define REGISTER_FREE_FUNCTION_END()                                           \
    constexpr std::size_t ff_line_end = __LINE__;                              \
                                                                               \
    typedef metamusil::int_seq::integer_sequence_from_range_t<std::size_t,     \
                                                              ff_line_begin +  \
                                                                  1,           \
                                                              ff_line_end>     \
        ff_line_range;                                                         \
                                                                               \
    typedef shadow::generate_valid_compile_time_infos_t<compile_time_ff_info,  \
                                                        ff_line_range>         \
        instantiated_compile_time_ff_infos;                                    \
                                                                               \
    typedef shadow::generate_valid_compile_time_infos_t<                       \
        exp_compile_time_ff_info,                                              \
        ff_line_range>                                                         \
        instantiated_exp_compile_time_ff_infos;                                \
                                                                               \
    typedef metamusil::t_list::concat_t<                                       \
        instantiated_compile_time_ff_infos,                                    \
        instantiated_exp_compile_time_ff_infos>                                \
        all_instantiated_compile_time_ff_infos;                                \
                                                                               \
    typedef shadow::generate_array_of_ff_info<                                 \
        all_instantiated_compile_time_ff_infos>                                \
        free_function_info_array_holder;


////////////////////////////////////////////////////////////////////////////////
// member function registration

#define REGISTER_MEMBER_FUNCTION_BEGIN()                                       \
    constexpr std::size_t mf_line_begin = __LINE__;                            \
                                                                               \
    template <std::size_t>                                                     \
    struct compile_time_mf_info;                                               \
                                                                               \
    template <std::size_t>                                                     \
    struct exp_compile_time_mf_info;


#define REGISTER_MEMBER_FUNCTION(class_name, function_name)                    \
                                                                               \
    template <>                                                                \
    struct compile_time_mf_info<__LINE__>                                      \
    {                                                                          \
        static constexpr char name[] = #function_name;                         \
                                                                               \
        typedef decltype(                                                      \
            &class_name::function_name) member_function_signature_type;        \
                                                                               \
        static const std::size_t object_type_index =                           \
            metamusil::t_list::index_of_type_v<type_universe, class_name>;     \
                                                                               \
        static const std::size_t return_type_index =                           \
            metamusil::t_list::index_of_type_v<                                \
                type_universe,                                                 \
                metamusil::deduce_return_type_t<                               \
                    member_function_signature_type>>;                          \
                                                                               \
        typedef metamusil::deduce_parameter_types_t<                           \
            member_function_signature_type>                                    \
            parameter_type_list;                                               \
                                                                               \
        typedef metamusil::t_list::type_transform_t<parameter_type_list,       \
                                                    metamusil::base_t>         \
            base_parameter_type_list;                                          \
                                                                               \
        static const std::size_t num_parameters =                              \
            metamusil::t_list::length_v<parameter_type_list>;                  \
                                                                               \
        typedef metamusil::t_list::order_t<base_parameter_type_list,           \
                                           type_universe>                      \
            parameter_index_sequence;                                          \
                                                                               \
        typedef metamusil::int_seq::integer_sequence_to_array<                 \
            parameter_index_sequence>                                          \
            parameter_type_indices_holder;                                     \
                                                                               \
        typedef metamusil::t_list::value_transform<parameter_type_list,        \
                                                   std::is_pointer>            \
            parameter_pointer_flags_holder;                                    \
                                                                               \
        static constexpr shadow::member_function_binding_signature             \
            bind_point = &shadow::member_function_detail::                     \
                             generic_member_function_bind_point<               \
                                 member_function_signature_type,               \
                                 &class_name::function_name>;                  \
    };                                                                         \
                                                                               \
                                                                               \
    constexpr char compile_time_mf_info<__LINE__>::name[];


#define REGISTER_MEMBER_FUNCTION_EXPLICIT(                                     \
    class_name, function_name, return_type, ...)                               \
                                                                               \
    template <>                                                                \
    struct exp_compile_time_mf_info<__LINE__>                                  \
    {                                                                          \
        static constexpr char name[] = #function_name;                         \
                                                                               \
        typedef return_type (class_name::*member_function_signature_type)(     \
            __VA_ARGS__);                                                      \
                                                                               \
        static const std::size_t object_type_index =                           \
            metamusil::t_list::index_of_type_v<type_universe, class_name>;     \
                                                                               \
        static const std::size_t return_type_index =                           \
            metamusil::t_list::index_of_type_v<                                \
                type_universe,                                                 \
                metamusil::deduce_return_type_t<                               \
                    member_function_signature_type>>;                          \
                                                                               \
        typedef metamusil::deduce_parameter_types_t<                           \
            member_function_signature_type>                                    \
            parameter_type_list;                                               \
                                                                               \
        typedef metamusil::t_list::type_transform_t<parameter_type_list,       \
                                                    metamusil::base_t>         \
            base_parameter_type_list;                                          \
                                                                               \
        static const std::size_t num_parameters =                              \
            metamusil::t_list::length_v<parameter_type_list>;                  \
                                                                               \
        typedef metamusil::t_list::order_t<base_parameter_type_list,           \
                                           type_universe>                      \
            parameter_index_sequence;                                          \
                                                                               \
        typedef metamusil::int_seq::integer_sequence_to_array<                 \
            parameter_index_sequence>                                          \
            parameter_type_indices_holder;                                     \
                                                                               \
        typedef metamusil::t_list::value_transform<parameter_type_list,        \
                                                   std::is_pointer>            \
            parameter_pointer_flags_holder;                                    \
                                                                               \
        static constexpr shadow::member_function_binding_signature             \
            bind_point = &shadow::member_function_detail::                     \
                             generic_member_function_bind_point<               \
                                 member_function_signature_type,               \
                                 &class_name::function_name>;                  \
    };                                                                         \
                                                                               \
    constexpr char exp_compile_time_mf_info<__LINE__>::name[];


#define REGISTER_MEMBER_FUNCTION_END()                                         \
    constexpr std::size_t mf_line_end = __LINE__;                              \
                                                                               \
    typedef metamusil::int_seq::integer_sequence_from_range_t<std::size_t,     \
                                                              mf_line_begin +  \
                                                                  1,           \
                                                              mf_line_end>     \
        mf_line_range;                                                         \
                                                                               \
    typedef shadow::generate_valid_compile_time_infos_t<compile_time_mf_info,  \
                                                        mf_line_range>         \
        valid_compile_time_mf_infos;                                           \
                                                                               \
    typedef shadow::generate_valid_compile_time_infos_t<                       \
        exp_compile_time_mf_info,                                              \
        mf_line_range>                                                         \
        valid_exp_compile_time_mf_infos;                                       \
                                                                               \
    typedef metamusil::t_list::concat_t<valid_compile_time_mf_infos,           \
                                        valid_exp_compile_time_mf_infos>       \
        all_compile_time_mf_infos;                                             \
                                                                               \
    typedef shadow::generate_array_of_mf_info<all_compile_time_mf_infos>       \
        member_function_info_array_holder;


#define REGISTER_MEMBER_VARIABLE_BEGIN()                                       \
    constexpr std::size_t mv_line_begin = __LINE__;                            \
                                                                               \
    template <std::size_t>                                                     \
    struct compile_time_mv_info;


#define REGISTER_MEMBER_VARIABLE(class_name, variable_name)                    \
                                                                               \
    template <>                                                                \
    struct compile_time_mv_info<__LINE__>                                      \
    {                                                                          \
        static constexpr char name[] = #variable_name;                         \
                                                                               \
        static const std::size_t object_type_index =                           \
            metamusil::t_list::index_of_type_v<type_universe, class_name>;     \
                                                                               \
        static const std::size_t offset = offsetof(class_name, variable_name); \
                                                                               \
        typedef metamusil::deduce_member_variable_type_t<decltype(             \
            &class_name::variable_name)>                                       \
            type_type;                                                         \
                                                                               \
        static const std::size_t type_index =                                  \
            metamusil::t_list::index_of_type_v<type_universe, type_type>;      \
                                                                               \
        static constexpr shadow::member_variable_get_binding_signature         \
            get_bind_point = &shadow::member_variable_detail::                 \
                                 generic_member_variable_get_bind_point<       \
                                     decltype(&class_name::variable_name),     \
                                     &class_name::variable_name>;              \
                                                                               \
        static constexpr shadow::member_variable_set_binding_signature         \
            set_bind_point = &shadow::member_variable_detail::                 \
                                 generic_member_variable_set_bind_point<       \
                                     decltype(&class_name::variable_name),     \
                                     &class_name::variable_name>;              \
    };                                                                         \
                                                                               \
    constexpr char compile_time_mv_info<__LINE__>::name[];


#define REGISTER_MEMBER_VARIABLE_END()                                         \
    constexpr std::size_t mv_line_end = __LINE__;                              \
                                                                               \
    typedef metamusil::int_seq::                                               \
        integer_sequence_from_range_t<std::size_t, mv_line_begin, mv_line_end> \
            mv_line_range;                                                     \
                                                                               \
    typedef shadow::generate_valid_compile_time_infos_t<compile_time_mv_info,  \
                                                        mv_line_range>         \
        valid_compile_time_mv_infos;                                           \
                                                                               \
    typedef shadow::generate_array_of_mv_info<valid_compile_time_mv_infos>     \
        member_variable_info_array_holder;


////////////////////////////////////////////////////////////////////////////////
// Initialize Shadow reflection library
#define SHADOW_INIT()                                                          \
    REGISTER_CONSTRUCTOR_END()                                                 \
    REGISTER_FREE_FUNCTION_END()                                               \
    REGISTER_MEMBER_FUNCTION_END()                                             \
    REGISTER_MEMBER_VARIABLE_END()                                             \
                                                                               \
    typedef metamusil::t_list::for_each_combination_t<type_universe,           \
                                                      shadow::type_pair>       \
        type_combinations;                                                     \
                                                                               \
    typedef shadow::filter_valid_combinations_t<type_combinations>             \
        valid_conversion_combinations;                                         \
                                                                               \
    template <class TypePair>                                                  \
    using bind_extract_conversion_info =                                       \
        shadow::extract_conversion_info<TypePair, type_universe>;              \
                                                                               \
    typedef metamusil::t_list::explicit_value_transform<                       \
        valid_conversion_combinations,                                         \
        bind_extract_conversion_info,                                          \
        shadow::conversion_info>                                               \
        conversion_info_array_holder;                                          \
                                                                               \
    typedef shadow::generate_array_of_serialization_info_t<type_universe>      \
        default_serialization_info_array_holder;                               \
                                                                               \
                                                                               \
    const shadow::reflection_manager manager{                                  \
        type_info_array_holder::value,                                         \
        constructor_info_array_holder::value,                                  \
        conversion_info_array_holder::value,                                   \
        free_function_info_array_holder::value,                                \
        member_function_info_array_holder::value,                              \
        member_variable_info_array_holder::value,                              \
        default_serialization_info_array_holder::value};                       \
                                                                               \
    template <class T, class... Args>                                          \
    shadow::object static_construct(Args&&... args)                            \
    {                                                                          \
        constexpr auto t_index =                                               \
            metamusil::t_list::index_of_type_v<type_universe, T>;              \
        constexpr const shadow::type_info* t_info =                            \
            type_info_array_holder::value + t_index;                           \
        return shadow::object(                                                 \
            shadow::any(T{std::forward<Args>(args)...}), t_info, &manager);    \
    }                                                                          \
                                                                               \
    template <class T>                                                         \
    shadow::object static_make_object(T&& value)                               \
    {                                                                          \
        constexpr auto t_index =                                               \
            metamusil::t_list::index_of_type_v<type_universe,                  \
                                               metamusil::base_t<T>>;          \
        constexpr const shadow::type_info* t_info =                            \
            type_info_array_holder::value + t_index;                           \
        return shadow::object(                                                 \
            shadow::any(std::forward<T>(value)), t_info, &manager);            \
    }                                                                          \
                                                                               \
    template <class T>                                                         \
    T& get_held_value(shadow::object& obj)                                     \
    {                                                                          \
        constexpr auto T_index =                                               \
            metamusil::t_list::index_of_type_v<type_universe, T>;              \
                                                                               \
        if(obj.type() !=                                                       \
           shadow::type_tag(type_info_array_holder::value[T_index]))           \
        {                                                                      \
            throw shadow::type_error(                                          \
                "attempt to get value of wrong underlying type");              \
        }                                                                      \
                                                                               \
        return manager.get<T>(obj);                                            \
    }                                                                          \
                                                                               \
    template <class T>                                                         \
    const T& get_held_value(const shadow::object& obj)                         \
    {                                                                          \
        constexpr auto T_index =                                               \
            metamusil::t_list::index_of_type_v<type_universe, T>;              \
                                                                               \
        if(obj.type() !=                                                       \
           shadow::type_tag(type_info_array_holder::value[T_index]))           \
        {                                                                      \
            throw shadow::type_error(                                          \
                "attempt to get value of wrong underlying type");              \
        }                                                                      \
                                                                               \
        return manager.get<T>(obj);                                            \
    }


#endif
