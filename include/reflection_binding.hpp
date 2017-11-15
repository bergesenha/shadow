#ifndef REFLECTION_BINDING_HPP
#define REFLECTION_BINDING_HPP


#include <string>
#include <type_traits>
#include <utility>
#include <istream>
#include <ostream>
#include <limits>

#include "any.hpp"
#include <function_deduction.hpp>
#include <member_variable_deduction.hpp>
#include <type_list.hpp>
#include <void_t.hpp>


namespace shadow
{
// free function signature
typedef any (*free_function_binding_signature)(any*);
// member function signature
typedef any (*member_function_binding_signature)(any&, any*);
// member variable getter
typedef any (*member_variable_get_binding_signature)(const any&);
// member variable setter
typedef void (*member_variable_set_binding_signature)(any&, const any&);
// constructor signature
typedef any (*constructor_binding_signature)(any*);
// conversion signature
typedef any (*conversion_binding_signature)(const any&);
// address of signature
typedef any (*address_of_signature)(any&);
// dereference signature
typedef any (*dereference_signature)(any&);
// serialization signature
typedef std::ostream& (*serialization_signature)(std::ostream&, const any&);
// deserialization signature
typedef std::istream& (*deserialization_signature)(std::istream&, any&);


////////////////////////////////////////////////////////////////////////////////
// generic bind point for free functions
// has the same signature as function pointer free_function_binding_signature
// which can be stored in the free_function_info struct


namespace free_function_detail
{
// necessary to handle void as return type differently when calling underlying
// free function
template <class ReturnType>
struct return_type_specializer
{
    // dispatch: unpacks argument type list and sequence to correctly index into
    // argument array and call get with the right types to retrieve raw values
    // from the anys
    template <class FunctionPointerType,
              FunctionPointerType FunctionPointerValue,
              class... ArgTypes,
              std::size_t... ArgSeq>
    static any
    dispatch(any* argument_array,
             metamusil::t_list::type_list<ArgTypes...>,
             std::index_sequence<ArgSeq...>)
    {
        // necessary to remove reference from types as any only stores
        // unqualified types, ie values only
        return FunctionPointerValue(argument_array[ArgSeq].get<ArgTypes>()...);
    }
};


template <>
struct return_type_specializer<void>
{
    // dispatch: unpacks argument type list and sequence to correctly index into
    // argument array and call get with the right types to retrieve raw values
    // from the anys
    template <class FunctionPointerType,
              FunctionPointerType FunctionPointerValue,
              class... ArgTypes,
              std::size_t... ArgSeq>
    static any
    dispatch(any* argument_array,
             metamusil::t_list::type_list<ArgTypes...>,
             std::index_sequence<ArgSeq...>)
    {
        // necessary to remove reference from types as any only stores
        // unqualified types, ie values only
        FunctionPointerValue(argument_array[ArgSeq].get<ArgTypes>()...);

        // return empty any, ie 'void'
        return any();
    }
};


// the value of the function pointer is stored at runtime in the template
// overload set
// this enables any function to be wrapped into uniform function signature that
// can be stored homogenously at runtime
template <class FunctionPointerType, FunctionPointerType FunctionPointerValue>
any
generic_free_function_bind_point(any* argument_array)
{
    typedef metamusil::deduce_return_type_t<FunctionPointerType> return_type;
    typedef metamusil::deduce_parameter_types_t<FunctionPointerType>
        parameter_types;
    // make integer sequence from type list
    typedef metamusil::t_list::index_sequence_for_t<parameter_types>
        parameter_sequence;

    return return_type_specializer<return_type>::
        template dispatch<FunctionPointerType, FunctionPointerValue>(
            argument_array, parameter_types(), parameter_sequence());
}
} // namespace free_function_detail


namespace member_function_detail
{

template <class ReturnType>
struct return_type_specializer
{
    template <class MemFunPointerType,
              MemFunPointerType MemFunPointerValue,
              class ObjectType,
              class... ParamTypes,
              std::size_t... ParamSequence>
    static any
    dispatch(any& object,
             any* argument_array,
             metamusil::t_list::type_list<ParamTypes...>,
             std::index_sequence<ParamSequence...>)
    {
        return (object.get<ObjectType>().*MemFunPointerValue)(
            argument_array[ParamSequence]
                .get<std::remove_reference_t<ParamTypes>>()...);
    }
};


template <>
struct return_type_specializer<void>
{
    template <class MemFunPointerType,
              MemFunPointerType MemFunPointerValue,
              class ObjectType,
              class... ParamTypes,
              std::size_t... ParamSequence>
    static any
    dispatch(any& object,
             any* argument_array,
             metamusil::t_list::type_list<ParamTypes...>,
             std::index_sequence<ParamSequence...>)
    {
        (object.get<ObjectType>().*MemFunPointerValue)(
            argument_array[ParamSequence]
                .get<std::remove_reference_t<ParamTypes>>()...);

        return any();
    }
};


template <class MemFunPointerType, MemFunPointerType MemFunPointerValue>
any
generic_member_function_bind_point(any& object, any* argument_array)
{
    // deduce return type
    typedef metamusil::deduce_return_type_t<MemFunPointerType> return_type;

    // deduce parameter types
    typedef metamusil::deduce_parameter_types_t<MemFunPointerType>
        parameter_types;

    // make integer sequence from parameter type list
    typedef metamusil::t_list::index_sequence_for_t<parameter_types>
        parameter_sequence;

    // deduce object type
    typedef metamusil::deduce_object_type_t<MemFunPointerType> object_type;

    return return_type_specializer<return_type>::
        template dispatch<MemFunPointerType, MemFunPointerValue, object_type>(
            object, argument_array, parameter_types(), parameter_sequence());
}
} // namespace member_function_detail


namespace member_variable_detail
{

template <class MemVarPointerType,
          MemVarPointerType MemVarPointerValue,
          class ObjectType,
          class MemVarType>
any
get_dispatch(const any& object)
{
    return (object.get<ObjectType>().*MemVarPointerValue);
}

template <class MemVarPointerType,
          MemVarPointerType MemVarPointerValue,
          class ObjectType,
          class MemVarType>
void
set_dispatch(any& object, const any& value)
{
    (object.get<ObjectType>().*MemVarPointerValue) = value.get<MemVarType>();
}


template <class MemVarPointerType, MemVarPointerType MemVarPointerValue>
any
generic_member_variable_get_bind_point(const any& object)
{
    typedef metamusil::deduce_member_variable_object_type_t<MemVarPointerType>
        object_type;
    typedef metamusil::deduce_member_variable_type_t<MemVarPointerType>
        member_variable_type;

    return get_dispatch<MemVarPointerType,
                        MemVarPointerValue,
                        object_type,
                        member_variable_type>(object);
}


template <class MemVarPointerType, MemVarPointerType MemVarPointerValue>
void
generic_member_variable_set_bind_point(any& object, const any& value)
{
    typedef metamusil::deduce_member_variable_object_type_t<MemVarPointerType>
        object_type;
    typedef metamusil::deduce_member_variable_type_t<MemVarPointerType>
        member_variable_type;

    return set_dispatch<MemVarPointerType,
                        MemVarPointerValue,
                        object_type,
                        member_variable_type>(object, value);
}
} // namespace member_variable_detail


namespace constructor_detail
{

// purpose of braced_init_selector is to attempt to use constructor T(...) if
// available, otherwise fall back to braced init list initialization
template <class, class T, class... ParamTypes>
struct braced_init_selector_impl
{
    template <std::size_t... Seq>
    static any
    constructor_dispatch(any* argument_array, std::index_sequence<Seq...>)
    {
        any out =
            T{argument_array[Seq]
                  .get<typename std::remove_reference<ParamTypes>::type>()...};
        return out;
    }
};

template <class T, class... ParamTypes>
struct braced_init_selector_impl<
    metamusil::void_t<decltype(T(std::declval<ParamTypes>()...))>,
    T,
    ParamTypes...>
{
    template <std::size_t... Seq>
    static any
    constructor_dispatch(any* argument_array, std::index_sequence<Seq...>)
    {
        any out =
            T(argument_array[Seq]
                  .get<typename std::remove_reference<ParamTypes>::type>()...);
        return out;
    }
};

template <class T, class... ParamTypes>
using braced_init_selector = braced_init_selector_impl<void, T, ParamTypes...>;


template <class T, class... ParamTypes>
any
generic_constructor_bind_point(any* argument_array)
{
    typedef std::index_sequence_for<ParamTypes...> param_sequence;

    return braced_init_selector<T, ParamTypes...>::constructor_dispatch(
        argument_array, param_sequence());
}
} // namespace constructor_detail


namespace conversion_detail
{
template <class TargetType, class SourceType, class = void>
struct conversion_specializer;

template <>
struct conversion_specializer<void, void>;

template <class TargetType, class SourceType>
struct conversion_specializer<
    TargetType,
    SourceType,
    std::enable_if_t<std::is_convertible<SourceType, TargetType>::value>>
{
    static any
    dispatch(const any& src)
    {
        TargetType temp = static_cast<TargetType>(src.get<SourceType>());
        return temp;
    }
};

template <class TargetType, class SourceType>
any
generic_conversion_bind_point(const any& src)
{
    return conversion_specializer<TargetType, SourceType>::dispatch(src);
}
} // namespace conversion_detail


namespace pointer_detail
{
template <class T>
inline any
generic_address_of_bind_point(any& value)
{
    return any(&(value.get<T>()));
}

template <>
inline any
generic_address_of_bind_point<void>(any&)
{
    return any(nullptr);
}

template <class T>
inline any
generic_dereference_bind_point(any& value)
{
    return any(*(value.get<T*>()));
}

template <>
inline any
generic_dereference_bind_point<void>(any&)
{
    return any();
}

} // namespace pointer_detail

namespace serialization_detail
{


template <class T, class = void>
struct serialization_type_selector;

template <>
struct serialization_type_selector<wchar_t>;

template <>
struct serialization_type_selector<char16_t>;

template <>
struct serialization_type_selector<char32_t>;


template <class T>
struct serialization_type_selector<
    T,
    std::enable_if_t<std::is_arithmetic<T>::value>>
{
    static std::ostream&
    serialize_dispatch(std::ostream& out, const any& value)
    {
        out << value.get<T>();
        return out;
    }

    static std::istream&
    deserialize_dispatch(std::istream& in, any& value)
    {
        in >> value.get<T>();
        return in;
    }
};


template <>
struct serialization_type_selector<std::string>
{
    static std::ostream&
    serialize_dispatch(std::ostream& out, const any& value)
    {
        out << '"' << value.get<std::string>() << '"';
        return out;
    }

    static std::istream&
    deserialize_dispatch(std::istream& in, any& value)
    {
        in.ignore(std::numeric_limits<std::streamsize>::max(), '"');

        std::getline(in, value.get<std::string>(), '"');

        return in;
    }
};


template <>
struct serialization_type_selector<bool>
{
    static std::ostream&
    serialize_dispatch(std::ostream& out, const any& value)
    {
        if(value.get<bool>())
        {
            out << "true";
        }
        else
        {
            out << "false";
        }

        return out;
    }

    static std::istream&
    deserialize_dispatch(std::istream& in, any& value)
    {
        std::string boolstring;

        in >> boolstring;

        if(boolstring == std::string("true"))
        {
            value.get<bool>() = true;
        }
        else
        {
            value.get<bool>() = false;
        }

        return in;
    }
};


template <class T>
std::ostream&
generic_serialization_bind_point(std::ostream& out, const any& value)
{
    return serialization_type_selector<T>::serialize_dispatch(out, value);
}


template <class T>
std::istream&
generic_deserialization_bind_point(std::istream& in, any& value)
{
    return serialization_type_selector<T>::deserialize_dispatch(in, value);
}
}

} // namespace shadow

#endif
