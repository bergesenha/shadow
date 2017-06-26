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
private:
    // holds type erased value
    any value_;
    // these identify the type erased value
    const reflection_manager* manager_;
    std::size_t type_index_;
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
};
