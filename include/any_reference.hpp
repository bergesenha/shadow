#pragma once

#include <memory>

#include <any.hpp>

namespace shadow
{

class ptr_holder_base
{
public:
    virtual ptr_holder_base* copy() const = 0;
};

template <class T>
class ptr_holder : public ptr_holder_base
{
    friend class any_reference;

public:
    ptr_holder() : ptr_value_(nullptr)
    {
    }

    ptr_holder(T& value) : ptr_value_(&value)
    {
    }

    virtual ptr_holder_base*
    copy() const override
    {
        auto ptrh = new ptr_holder<T>;
        ptrh->ptr_value_ = ptr_value_;
        return ptrh;
    }

private:
    T* ptr_value_;
};


class any_reference
{
public:
    any_reference() = default;

    template <class T>
    any_reference(T& value) : reference_(new ptr_holder<std::decay_t<T>>(value))
    {
    }

    any_reference(any_reference& other) : reference_(other.reference_->copy())
    {
    }

    any_reference(const any_reference& other)
        : reference_(other.reference_->copy())
    {
    }


public:
    template <class T>
    std::decay_t<T>&
    get()
    {
        return *(static_cast<ptr_holder<T>*>(reference_.get())->ptr_value_);
    }

    template <class T>
    const std::decay_t<T>&
    get() const
    {
        return *(static_cast<ptr_holder<T>*>(reference_.get())->ptr_value_);
    }

private:
    std::unique_ptr<ptr_holder_base> reference_;
};
}


namespace shadow
{
}
