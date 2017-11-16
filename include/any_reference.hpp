#pragma once

#include <any.hpp>

namespace shadow
{
class any_reference
{
public:
    template <class T>
    any_reference(T& value) : value_ptr_(&value)
    {
    }

    template <class T>
    any_reference(const T& value) : value_ptr_(&value)
    {
    }

    any_reference(const any_reference&) = default;
    any_reference(any_reference&&) = default;
    any_reference& operator=(const any_reference&) = default;
    any_reference& operator=(any_reference&&) = default;
    ~any_reference() = default;

private:
    any value_ptr_;
};
}
