#pragma once

#include <memory>

#include <any.hpp>

namespace shadow
{

class ptr_holder_base
{
public:
    virtual ptr_holder_base* copy() const = 0;

    virtual holder_base* make_heap_value() const = 0;
    virtual void make_stack_value(
        std::aligned_storage_t<sizeof(holder_base*)>* buffer) const = 0;
    virtual bool fits_on_stack() const = 0;
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

    virtual holder_base*
    make_heap_value() const override
    {
        return new holder<T>(*ptr_value_);
    }

    virtual void
    make_stack_value(
        std::aligned_storage_t<sizeof(holder_base*)>* buffer) const override
    {
        new(buffer) holder<T>(*ptr_value_);
    }

    virtual bool
    fits_on_stack() const override
    {
        return is_small_buffer_type<T>::value;
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

    any_reference(any_reference& other) : reference_()
    {
        if(other.has_reference())
        {
            reference_.reset(other.reference_->copy());
        }
    }

    any_reference(const any_reference& other)
        : reference_(other.reference_->copy())
    {
    }

    any_reference(any_reference&& other) = default;

    any_reference&
    operator=(any_reference other)
    {
        std::swap(reference_, other.reference_);
        return *this;
    }

    operator any()
    {
        any out;

        if(has_reference())
        {
            if(reference_->fits_on_stack())
            {
                reference_->make_stack_value(&out.stack);
                out.on_heap_ = false;
            }
            else
            {
                out.heap = reference_->make_heap_value();
                out.on_heap_ = true;
            }
        }

        return out;
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

    bool
    has_reference() const
    {
        return bool(reference_);
    }

private:
    std::unique_ptr<ptr_holder_base> reference_;
};
}


namespace shadow
{
}
