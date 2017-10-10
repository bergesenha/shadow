#ifndef ANY_HPP
#define ANY_HPP


#include <memory>


namespace shadow
{


// abstract base class providing interface to held value
// holds no type information, intended as building block for a more complex type
// that holds type information through a reflection mechanism
class holder_base
{
public:
    virtual holder_base* clone() const = 0;
    virtual ~holder_base() = default;
};


// concrete subclass holding value of any type
template <class T>
class holder : public holder_base
{
public:
    // this is necessary for any to access raw value
    friend class any;

public:
    holder(const T& value) : value_(value)
    {
    }

    virtual holder_base*
    clone() const override
    {
        return new holder(value_);
    }

private:
    T value_;
};


// specialization for type void, represents empty value
template <>
class holder<void> : public holder_base
{
public:
    virtual holder_base*
    clone() const override
    {
        return new holder();
    }
};


// type erasure wrapper for value of any type
class any
{
public:
    // construct empty any (holds 'void')
    any() : holder_(std::make_unique<holder<void>>())
    {
    }


    // construct empty with a held value of type T
    template <class T>
    any(const T& value) : holder_(std::make_unique<holder<T>>(value))
    {
    }

    // copy construct, relies on holder implementation of clone
    any(const any& other) : holder_(other.holder_->clone())
    {
    }

    any(any&&) = default;

    any&
    operator=(any other)
    {
        holder_ = std::move(other.holder_);
        return *this;
    }


public:
    // unsafe interaction with underlying value, intended to be used by higher
    // level reflection mechanism that holds information about the type of held
    // value

    template <class T>
    T&
    get()
    {
        return static_cast<holder<T>*>(holder_.get())->value_;
    }

    template <class T>
    const T&
    get() const
    {
        return static_cast<holder<T>*>(holder_.get())->value_;
    }

private:
    std::unique_ptr<holder_base> holder_;
};
} // namespace shadow

#endif
