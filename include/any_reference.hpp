#pragma once

#include <any.hpp>

namespace shadow
{
class any_reference
{
public:
    any_reference() : ref_(nullptr)
    {
    }

    any_reference(any& value) : ref_(&value)
    {
    }

    any_reference&
    operator=(const any& value)
    {
        *ref_ = value;
        return *this;
    }

    operator any()
    {
        return *ref_;
    }

public:
    template <class T>
    std::decay_t<T>&
    get()
    {
        return ref_->get<T>();
    }

    template <class T>
    const std::decay_t<T>&
    get() const
    {
        return ref_->get<T>();
    }

private:
    any* ref_;
};
}


namespace shadow
{
}
