#pragma once


#include <string>
#include <cstddef>
#include <cstring>
#include <ostream>
#include <istream>

#include <any.hpp>
#include <reflection_info.hpp>


namespace shadow
{

template <class InfoType, template <class> class... Policies>
class info_type_aggregate
    : public Policies<info_type_aggregate<InfoType, Policies...>>...
{
    template <class Derived>
    friend class name_policy;

    template <class Derived>
    friend class size_policy;

    template <class Derived>
    friend class comparison_policy;

    friend class reflection_manager;

public:
    info_type_aggregate() = default;

    info_type_aggregate(const InfoType& info) : info_ptr_(&info)
    {
    }

    info_type_aggregate* operator->()
    {
        return this;
    }

    const info_type_aggregate* operator->() const
    {
        return this;
    }

private:
    const InfoType* info_ptr_;
};


template <class Derived>
class name_policy
{
public:
    std::string
    name() const
    {
        return std::string(static_cast<const Derived*>(this)->info_ptr_->name);
    }
};

template <class Derived>
class size_policy
{
public:
    std::size_t
    size() const
    {
        return static_cast<const Derived*>(this)->info_ptr_->size;
    }
};
}
