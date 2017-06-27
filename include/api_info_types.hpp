#pragma once

#include <string>
#include <utility>
#include <ostream>

#include "reflection_info.hpp"
#include "info_iterator.hpp"

namespace shadow
{
class reflection_manager;


// holds one value with type/reflection information
class variable
{
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

private:
    // holds type erased value
    any value_;
    // these identify the type erased value
    std::size_t type_index_;
    const reflection_manager* manager_;
};


template <class InfoType, template <class> class... Policies>
class api_type_aggregator
    : public Policies<api_type_aggregator<InfoType, Policies...>>...
{
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

typedef api_type_aggregator<free_function_info,
                            get_name_policy,
                            get_num_parameters_policy,
                            get_parameter_types_policy,
                            get_return_type_policy>
    free_function_;
}
