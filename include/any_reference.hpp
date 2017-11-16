#pragma once

#include <any.hpp>

namespace shadow
{
class any_reference
{
public:
    template <class T>
    struct get_specializer
    {
        typedef std::decay_t<T>& return_type;
    };

    template <class T>
    struct get_specializer<T&&>
    {
        typedef std::decay_t<T>&& return_type;
    };

public:
    template <class T>
    any_reference(T& value);

    template <class T>
    any_reference(const T& value);

    any_reference(const any_reference&) = default;
    any_reference(any_reference&&) = default;
    any_reference& operator=(const any_reference&) = default;
    any_reference& operator=(any_reference&&) = default;
    ~any_reference() = default;

public:
    template <class T>
    typename get_specializer<T>::return_type get();

private:
    any value_ptr_;
};
}


namespace shadow
{
template <class T>
any_reference::any_reference(T& value) : value_ptr_(&value)
{
}

template <class T>
any_reference::any_reference(const T& value) : value_ptr_(&value)
{
}
}
