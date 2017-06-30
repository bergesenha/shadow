#pragma once

#include <string>
#include <utility>
#include <ostream>
#include <vector>
#include <algorithm>


#include <integer_sequence.hpp>

#include "reflection_info.hpp"
#include "info_iterator.hpp"
#include "exceptions.hpp"

namespace shadow
{
class reflection_manager;


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


typedef api_type_aggregator<type_info, get_name_policy, get_size_policy> type_;

inline std::ostream&
operator<<(std::ostream& out, const type_& tp)
{
    out << tp.name();
    return out;
}

template <class Derived>
class get_type_policy
{
public:
    type_
    get_type() const
    {
        // retrieve type index
        const auto type_index =
            static_cast<const Derived*>(this)->info_->type_index;

        return static_cast<const Derived*>(this)->manager_->type_by_index(
            type_index);
    }
};


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

typedef api_type_aggregator<constructor_info,
                            get_type_policy,
                            get_num_parameters_policy,
                            get_parameter_types_policy>
    constructor_;

inline std::ostream&
operator<<(std::ostream& out, const constructor_& con)
{
    out << con.get_type().name() << '(';

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


typedef api_type_aggregator<conversion_info,
                            get_from_type_policy,
                            get_to_type_policy>
    type_conversion_;


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


typedef api_type_aggregator<member_variable_info,
                            get_name_policy,
                            get_object_type_policy,
                            get_type_policy>
    member_variable_;


inline std::ostream&
operator<<(std::ostream& out, const member_variable_& mv)
{
    out << mv.get_type() << " " << mv.object_type() << "::" << mv.name();

    return out;
}


typedef api_type_aggregator<string_serialization_info, get_type_policy>
    string_serializer_;


// holds one value with type/reflection information
class variable
{
    friend std::ostream& operator<<(std::ostream&, const variable&);
    friend std::istream& operator>>(std::istream&, variable&);

    template <class Derived>
    friend class call_free_function;

public:
    typedef indexed_info_iterator_<const member_function_info,
                                   const member_function_>
        member_function_iterator;

    typedef member_variable_ member_variable;
    typedef indexed_info_iterator_<const member_variable_info,
                                   const member_variable>
        member_variable_iterator;

public:
    variable() : value_(), type_index_(0), manager_(nullptr)
    {
    }

    variable(const any& value,
             std::size_t type_index,
             const reflection_manager* manager)
        : value_(value), type_index_(type_index), manager_(manager)
    {
    }


public:
    type_ type() const;

    std::pair<member_function_iterator, member_function_iterator>
    member_functions() const;

    std::pair<member_variable_iterator, member_variable_iterator>
    member_variables() const;

    variable
    get_member_variable(const member_variable& mv) const
    {
        auto bind_point = mv.info_->get_bind_point;

        return variable(bind_point(value_), mv.info_->type_index, manager_);
    }

    variable
    get_member_variable(member_variable_iterator mv_it) const
    {
        auto bind_point = mv_it->info_->get_bind_point;

        return variable(bind_point(value_), mv_it->info_->type_index, manager_);
    }

private:
    // holds type erased value
    any value_;
    // these identify the type erased value
    std::size_t type_index_;
    const reflection_manager* manager_;
};


template <class Derived>
class call_free_function
{
public:
    template <class... Args>
    variable
    call_static_unsafe(Args... args) const
    {
        const auto info = static_cast<const Derived*>(this)->info_;

        any arg_array[] = {args...};

        return variable(info->bind_point(arg_array),
                        info->return_type_index,
                        static_cast<const Derived*>(this)->manager_);
    }

    template <class TypeUniverseList, class... Args>
    variable
    call_static_safe(Args... args) const
    {
        const auto info = static_cast<const Derived*>(this)->info_;

        typedef metamusil::t_list::type_list<Args...> given_types;
        typedef metamusil::t_list::order_t<given_types, TypeUniverseList>
            given_types_indices;
        typedef metamusil::int_seq::integer_sequence_to_array<
            given_types_indices>
            given_types_index_array_holder;

        if(std::extent<decltype(
               given_types_index_array_holder::value)>::value !=
           info->num_parameters)
        {
            throw argument_error("wrong number of arguments");
        }

        for(auto i = 0; i < info->num_parameters; ++i)
        {
            if(given_types_index_array_holder::value[i] !=
               info->parameter_type_indices[i])
            {
                throw argument_error("wrong argument type");
            }
        }

        any arg_array[] = {args...};

        return variable(info->bind_point(arg_array),
                        info->return_type_index,
                        static_cast<const Derived*>(this)->manager_);
    }


    template <class Iterator>
    variable
    operator()(Iterator arg_begin, Iterator arg_end) const
    {

        const auto info = static_cast<const Derived*>(this)->info_;


        // construct argument buffer
        std::vector<any> arg_buffer;
        arg_buffer.reserve(info->num_parameters);

        std::transform(arg_begin,
                       arg_end,
                       std::back_inserter(arg_buffer),
                       [](const variable& var) { return var.value_; });

        // check arguments
        if(arg_buffer.size() != info->num_parameters)
        {
            throw argument_error("wrong number of arguments provided");
        }

        for(auto i = 0ul; i < info->num_parameters; ++i)
        {
            if(info->parameter_type_indices[i] != arg_begin->type_index_)
            {
                throw argument_error("wrong argument type");
            }

            ++arg_begin;
        }

        return variable(info->bind_point(arg_buffer.data()),
                        info->return_type_index,
                        static_cast<const Derived*>(this)->manager_);
    }

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

    template <class Iterator>
    variable
    call_unsafe(Iterator arg_begin, Iterator arg_end) const
    {
        const auto info = static_cast<const Derived*>(this)->info_;
        std::vector<any> arg_buffer;
        arg_buffer.reserve(info->num_parameters);
        std::transform(arg_begin,
                       arg_end,
                       std::back_inserter(arg_buffer),
                       [](const variable& var) { return var.value_; });

        return variable(info->bind_point(arg_buffer.data()),
                        info->return_type_index,
                        static_cast<const Derived*>(this)->manager_);
    }

    template <class Iterator>
    variable
    call_with_conversion(Iterator arg_begin, Iterator arg_end) const
    {
        const auto info = static_cast<const Derived*>(this)->info_;

        const auto num_params = info->num_parameters;

        if(num_params != std::distance(arg_begin, arg_end))
        {
            throw argument_error("wrong number of arguments provided");
        }

        std::vector<any> arg_buffer;
        arg_buffer.reserve(num_params);

        std::transform(
            arg_begin,
            arg_end,
            info->parameter_type_indices,
            std::back_inserter(arg_buffer),
            [](const auto& var, const auto to_index) {
                const auto from_index = var.type_index_;
                const auto manager = var.manager_;

                auto found = std::find_if(
                    manager->conversion_info_indices_by_type_[from_index]
                        .begin(),
                    manager->conversion_info_indices_by_type_[from_index].end(),
                    [to_index, manager](auto converter_index) {
                        return manager->conversion_info_range_
                                   .first[converter_index]
                                   .to_type_index == to_index;
                    });

                if(found ==
                   manager->conversion_info_indices_by_type_[from_index].end())
                {
                    throw argument_error("conversion of argument not possible");
                }

                const auto found_conversion_index = *found;

                return manager->conversion_info_range_
                    .first[found_conversion_index]
                    .bind_point(var.value_);

            });

        return variable(info->bind_point(arg_buffer.data()),
                        info->return_type_index,
                        static_cast<const Derived*>(this)->manager_);
    }
};


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
}
