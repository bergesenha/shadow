#pragma once

#include <string>

#include "reflection_info.hpp"

namespace shadow
{
class reflection_manager;


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

typedef api_type_aggregator<constructor_info,
                            get_type_policy,
                            get_num_parameters_policy>
    constructor_;
};
