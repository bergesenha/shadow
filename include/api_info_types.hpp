#ifndef API_INFO_TYPES_HPP
#define API_INFO_TYPES_HPP


#include <algorithm>
#include <ostream>
#include <string>
#include <utility>
#include <vector>


#include <integer_sequence.hpp>

#include "exceptions.hpp"
#include "info_iterator.hpp"
#include "reflection_info.hpp"

namespace shadow
{
// forward declaration
class reflection_manager;


// class template for aggregating reflection information types from policies
// using CRTP
template <class InfoType, template <class> class... Policies>
class api_type_aggregator
    : public Policies<api_type_aggregator<InfoType, Policies...>>...
{
    friend class variable;

public:
    api_type_aggregator() : info_(nullptr), manager_(nullptr)
    {
    }

    api_type_aggregator(const InfoType* info, const reflection_manager* manager)
        : info_(info), manager_(manager)
    {
    }

    api_type_aggregator* operator->()
    {
        return this;
    }

    const api_type_aggregator* operator->() const
    {
        return this;
    }

public:
    const InfoType* info_;
    const reflection_manager* manager_;
};


// adds member function name, returning name of the information type where
// relevant
template <class Derived>
class get_name_policy
{
public:
    std::string
    name() const
    {
        return (static_cast<const Derived*>(this))->info_->name;
    }
};


// adds member function size, returning size of a type
template <class Derived>
class get_size_policy
{
public:
    std::size_t
    size() const
    {
        return static_cast<const Derived*>(this)->info_->size;
    }
};

template <class Derived>
class compare_policy
{
public:
    bool
    operator==(const Derived& other) const
    {
        return static_cast<const Derived*>(this)->info_ == other.info_;
    }
};


// aggregate type representing type information
typedef api_type_aggregator<type_info,
                            get_name_policy,
                            get_size_policy,
                            compare_policy>
    type_;

inline std::ostream&
operator<<(std::ostream& out, const type_& tp)
{
    out << tp.name();
    return out;
}


// adds member function get_type, returning type of an information object where
// relevant
template <class Derived>
class get_type_policy
{
public:
    type_
    type() const
    {
        // retrieve type index
        const auto type_index =
            static_cast<const Derived*>(this)->info_->type_index;

        return static_cast<const Derived*>(this)->manager_->type_by_index(
            type_index);
    }
};


// adds num_parameters member function, returning the number of parameters of a
// function, member_function, constructor etc...
template <class Derived>
class get_num_parameters_policy
{
public:
    std::size_t
    num_parameters() const
    {
        return static_cast<const Derived*>(this)->info_->num_parameters;
    }
};


// adds member function parameter_types, returning pair of iterators to type
// information objects representing the types of the parameters of a function,
// member function, constructor or other.
template <class Derived>
class get_parameter_types_policy
{
public:
    typedef indexed_info_iterator_<const type_info, const type_>
        const_parameter_type_iterator;

public:
    std::pair<const_parameter_type_iterator, const_parameter_type_iterator>
    parameter_types() const;
};


// aggregate type representing a constructor for a type.
typedef api_type_aggregator<constructor_info,
                            get_type_policy,
                            get_num_parameters_policy,
                            get_parameter_types_policy>
    constructor_;


inline std::ostream&
operator<<(std::ostream& out, const constructor_& con)
{
    out << con.type().name() << '(';

    const auto num_params = con.num_parameters();

    if(num_params > 0)
    {
        auto param_pair = con.parameter_types();
        out << *param_pair.first;

        for(++param_pair.first; param_pair.first != param_pair.second;
            ++param_pair.first)
        {
            out << ", " << *param_pair.first;
        }
    }

    out << ')';

    return out;
}


// adds member function from_type, returning a type information object
template <class Derived>
class get_from_type_policy
{
public:
    type_
    from_type() const
    {
        const auto from_type_index =
            static_cast<const Derived*>(this)->info_->from_type_index;

        return static_cast<const Derived*>(this)->manager_->type_by_index(
            from_type_index);
    }
};


// adds member function to_type, returning a type information object
template <class Derived>
class get_to_type_policy
{
public:
    type_
    to_type() const
    {
        const auto to_type_index =
            static_cast<const Derived*>(this)->info_->to_type_index;

        return static_cast<const Derived*>(this)->manager_->type_by_index(
            to_type_index);
    }
};


// aggregate type representing implicit conversions
typedef api_type_aggregator<conversion_info,
                            get_from_type_policy,
                            get_to_type_policy>
    type_conversion_;


// adds member function return_type, returning a type information object
// representing the return type of a function, member_function etc.
template <class Derived>
class get_return_type_policy
{
public:
    type_
    return_type() const
    {
        const auto ret_type_index =
            static_cast<const Derived*>(this)->info_->return_type_index;
        return static_cast<const Derived*>(this)->manager_->type_by_index(
            ret_type_index);
    }
};


// adds member function object_type, returning a type information object
// representing the object owning the member variable, member function etc...
template <class Derived>
class get_object_type_policy
{
public:
    type_
    object_type() const
    {
        const auto object_type_index =
            static_cast<const Derived*>(this)->info_->object_type_index;

        return static_cast<const Derived*>(this)->manager_->type_by_index(
            object_type_index);
    }
};


// aggregate type representing a member function
typedef api_type_aggregator<member_function_info,
                            get_name_policy,
                            get_num_parameters_policy,
                            get_parameter_types_policy,
                            get_return_type_policy,
                            get_object_type_policy>
    member_function_;

inline std::ostream&
operator<<(std::ostream& out, const member_function_& mf)
{
    out << mf.return_type() << " " << mf.object_type() << "::" << mf.name()
        << "(";

    if(mf.num_parameters() > 0)
    {
        auto param_pair = mf.parameter_types();
        out << *param_pair.first;

        for(++param_pair.first; param_pair.first != param_pair.second;
            ++param_pair.first)
        {
            out << ", " << *param_pair.first;
        }
    }

    out << ')';

    return out;
}


// aggregate type representing a member variable
typedef api_type_aggregator<member_variable_info,
                            get_name_policy,
                            get_object_type_policy,
                            get_type_policy>
    member_variable_;


inline std::ostream&
operator<<(std::ostream& out, const member_variable_& mv)
{
    out << mv.type() << " " << mv.object_type() << "::" << mv.name();

    return out;
}


// aggregate type representing a string serializer
typedef api_type_aggregator<string_serialization_info, get_type_policy>
    string_serializer_;


namespace call_utils
{
template <class ArgumentIterator, class OutputIterator, class InfoType>
void
construct_argument_values(ArgumentIterator first,
                          ArgumentIterator last,
                          OutputIterator arg_val_out,
                          const InfoType& info)
{
    for(auto ptr_flags = info.parameter_pointer_flags; first != last;
        ++first, ++arg_val_out, ++ptr_flags)
    {
        if(*ptr_flags)
        {
            *arg_val_out = first->address_of();
        }
        else
        {
            *arg_val_out = first->value_;
        }
    }
}


template <class ArgumentIterator, class InfoType>
void
check_parameter_types(ArgumentIterator first,
                      ArgumentIterator last,
                      const InfoType& info)
{
    if(std::distance(first, last) != info.num_parameters)
    {
        throw argument_error("wrong number of arguments provided");
    }

    for(auto prm_index = info.parameter_type_indices; first != last;
        ++first, ++prm_index)
    {
        if(*prm_index != first->type_index_)
        {
            throw argument_error("wrong argument type");
        }
    }
}


template <class ArgumentValueIterator, class OutputIterator, class InfoType>
void
pass_arguments_out(ArgumentValueIterator first,
                   ArgumentValueIterator last,
                   OutputIterator variable_out,
                   const InfoType& info)
{
    for(auto ptr_flags = info.parameter_pointer_flags; first != last;
        ++first, ++variable_out, ++ptr_flags)
    {
        if(*ptr_flags)
        {
            const auto type_index = variable_out->type_index_;
            const auto& t_info =
                variable_out->manager_->type_info_range_.first[type_index];
            variable_out->value_ = t_info.dereference_bind_point(*first);
        }
        else
        {
            variable_out->value_ = *first;
        }
    }
}
}


// holds one value with type/reflection information
class variable
{
    friend std::ostream& operator<<(std::ostream&, const variable&);
    friend std::istream& operator>>(std::istream&, variable&);

    template <class ArgumentIterator, class OutputIterator, class InfoType>
    friend void call_utils::construct_argument_values(ArgumentIterator,
                                                      ArgumentIterator,
                                                      OutputIterator,
                                                      const InfoType&);

    template <class ArgumentIterator, class InfoType>
    friend void call_utils::check_parameter_types(ArgumentIterator,
                                                  ArgumentIterator,
                                                  const InfoType&);

    template <class ArgumentValueIterator, class OutputIterator, class InfoType>
    friend void call_utils::pass_arguments_out(ArgumentValueIterator,
                                               ArgumentValueIterator,
                                               OutputIterator,
                                               const InfoType&);

    template <class Derived>
    friend class call_free_function;

    friend class reflection_manager;

    template <class T>
    friend T extract_value(const variable& var);

public:
    typedef member_function_ member_function;
    typedef indexed_info_iterator_<const member_function_info,
                                   const member_function>
        member_function_iterator;

    typedef member_variable_ member_variable;
    typedef indexed_info_iterator_<const member_variable_info,
                                   const member_variable>
        member_variable_iterator;

public:
    // default constructor, constructs an empty variable of type index 0
    // corresponding to a type of 'void'
    variable();

    // full constructor, typically invoked by reflection_manager or
    // static_create function template
    variable(any value,
             std::size_t type_index,
             const reflection_manager* manager);


public:
    // returns the type of the value held by the variable
    type_ type() const;

    // returns pair of iterators to all member functions of the type of value
    // held by variable
    std::pair<member_function_iterator, member_function_iterator>
    member_functions() const;

    // returns pair of iterators to all member variables of the type of value
    // held by the variable
    std::pair<member_variable_iterator, member_variable_iterator>
    member_variables() const;

    // return value of member variable represented by mv
    variable get_member_variable(const member_variable& mv) const;

    // overload returns member variable represented by mv_it, an iterator to a
    // member_variable
    variable get_member_variable(member_variable_iterator mv_it) const;

    // set the value of member variable referred to by mv to val
    void set_member_variable(const member_variable& mv, const variable& val);

    // overload sets the member variable referred to by mv_it, an iterator to a
    // member_variable
    void set_member_variable(member_variable_iterator mv_it,
                             const variable& val);

    // call member function identified by mf
    template <class Iterator>
    variable call_member_function(const member_function& mf,
                                  Iterator arg_begin,
                                  Iterator arg_end);

    // overload for member function taking no parameters
    variable call_member_function(const member_function& mf);

    // overload taking member_function_iterator as identifier
    template <class Iterator>
    variable call_member_function(member_function_iterator mf_it,
                                  Iterator arg_begin,
                                  Iterator arg_end);

    // overload taking member_function_iterator as identifier and calling member
    // function with no arguments
    variable call_member_function(member_function_iterator mf_it);

private:
    any address_of();

private:
    // holds type erased value
    any value_;
    // these identify the type erased value
    std::size_t type_index_;
    const reflection_manager* manager_;
};


// adds member functions for invoking a free function.
template <class Derived>
class call_free_function
{
public:
    // call specified at runtime, arguments checked for correctness,
    // arguments given by iterators to variable
    // argument_error thrown if not correct
    template <class Iterator>
    variable
    operator()(Iterator arg_begin, Iterator arg_end) const
    {

        const auto info = static_cast<const Derived*>(this)->info_;

        call_utils::check_parameter_types(arg_begin, arg_end, *info);

        // construct argument buffer
        std::vector<any> arg_values;
        arg_values.reserve(info->num_parameters);
        call_utils::construct_argument_values(
            arg_begin, arg_end, std::back_inserter(arg_values), *info);


        auto return_value = info->bind_point(arg_values.data());

        call_utils::pass_arguments_out(
            arg_values.begin(), arg_values.end(), arg_begin, *info);

        return variable(return_value,
                        info->return_type_index,
                        static_cast<const Derived*>(this)->manager_);
    }

    // overload for no parameters, if the function called does take parameters,
    // argument_error thrown
    variable
    operator()() const
    {
        const auto info = static_cast<const Derived*>(this)->info_;

        // check that function in fact takes no arguments
        if(info->num_parameters != 0)
        {
            throw argument_error("wrong number of arguments provided");
        }

        return variable(info->bind_point(nullptr),
                        info->return_type_index,
                        static_cast<const Derived*>(this)->manager_);
    }
};


// aggregate type representing a free function
typedef api_type_aggregator<free_function_info,
                            get_name_policy,
                            get_num_parameters_policy,
                            get_parameter_types_policy,
                            get_return_type_policy,
                            call_free_function>
    free_function_;

inline std::ostream&
operator<<(std::ostream& out, const free_function_& ff)
{
    out << ff.return_type() << ' ' << ff.name() << '(';

    if(ff.num_parameters() > 0)
    {
        auto param_pair = ff.parameter_types();
        out << *param_pair.first;

        for(++param_pair.first; param_pair.first != param_pair.second;
            ++param_pair.first)
        {
            out << ", " << *param_pair.first;
        }
    }

    out << ')';

    return out;
}
} // namespace shadow

////////////////////////////////////////////////////////////////////////////////
// DEFINITIONS
namespace shadow
{
inline variable::variable() : value_(), type_index_(0), manager_(nullptr)
{
}


inline variable::variable(any value,
                          std::size_t type_index,
                          const reflection_manager* manager)
    : value_(std::move(value)), type_index_(type_index), manager_(manager)
{
}


inline variable
variable::get_member_variable(const member_variable& mv) const
{
    auto bind_point = mv.info_->get_bind_point;

    return variable(bind_point(value_), mv.info_->type_index, manager_);
}


inline variable
variable::get_member_variable(member_variable_iterator mv_it) const
{
    auto bind_point = mv_it->info_->get_bind_point;

    return variable(bind_point(value_), mv_it->info_->type_index, manager_);
}


inline void
variable::set_member_variable(const member_variable& mv, const variable& val)
{
    if(val.type_index_ != mv.info_->type_index)
    {
        throw argument_error("wrong type setting member variable");
    }

    auto bind_point = mv.info_->set_bind_point;

    bind_point(value_, val.value_);
}


inline void
variable::set_member_variable(member_variable_iterator mv_it,
                              const variable& val)
{
    if(val.type_index_ != mv_it->info_->type_index)
    {
        throw argument_error("wrong type setting member variable");
    }

    auto bind_point = mv_it->info_->set_bind_point;

    bind_point(value_, val.value_);
}


template <class Iterator>
inline variable
variable::call_member_function(const member_function& mf,
                               Iterator arg_begin,
                               Iterator arg_end)
{
    auto bind_point = mf.info_->bind_point;

    call_utils::check_parameter_types(arg_begin, arg_end, *mf.info_);

    // construct arg buffer
    std::vector<any> arg_buffer;
    arg_buffer.reserve(mf.info_->num_parameters);

    call_utils::construct_argument_values(
        arg_begin, arg_end, std::back_inserter(arg_buffer), *mf.info_);

    auto return_value = bind_point(value_, arg_buffer.data());

    call_utils::pass_arguments_out(
        arg_buffer.begin(), arg_buffer.end(), arg_begin, *mf.info_);

    return variable(return_value, mf.info_->return_type_index, manager_);
}

inline variable
variable::call_member_function(const member_function& mf)
{
    if(mf.info_->num_parameters > 0)
    {
        throw argument_error("wrong number of arguments");
    }

    auto bind_point = mf.info_->bind_point;

    return variable(
        bind_point(value_, nullptr), mf.info_->return_type_index, manager_);
}


template <class Iterator>
inline variable
variable::call_member_function(member_function_iterator mf_it,
                               Iterator arg_begin,
                               Iterator arg_end)
{
    return call_member_function(*mf_it, arg_begin, arg_end);
}

inline variable
variable::call_member_function(member_function_iterator mf_it)
{
    return call_member_function(*mf_it);
}


} // namespace shadow

#endif
