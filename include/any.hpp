#pragma once


namespace shadow
{


// abstract base class providing interface to held value
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
}
