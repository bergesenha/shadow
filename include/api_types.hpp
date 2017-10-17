#pragma once


#include <string>
#include <cstddef>
#include <cstring>

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

public:
    info_type_aggregate() = default;

    info_type_aggregate(const InfoType& info) : info_ptr_(&info)
    {
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


typedef info_type_aggregate<type_info, name_policy, size_policy> type_tag;

class object
{
public:
private:
    any value_;
};
}


namespace shadow
{
}
