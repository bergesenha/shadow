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

        static return_type
        get(any_reference& ar)
        {
            return *(ar.value_ptr_.get<T*>());
        }
    };

    template <class T>
    struct get_specializer<const T>
    {
        typedef const std::decay_t<T>& return_type;

        static return_type get(any_reference& ar)
        {
            return *(ar.value_ptr_.get<const T*>());
        }
    };

    template <class T>
    struct get_specializer<T&&>
    {
        typedef std::decay_t<T>&& return_type;

        static return_type
        get(any_reference& ar)
        {
            return std::move(*(ar.value_ptr_.get<T*>()));
        }
    };

public:

    template <class T>
    any_reference(T& value);

    template <class T>
    any_reference(const T& value);

    any_reference() = default;
    any_reference(const any_reference&) = default;
    any_reference(any_reference&&) = default;
    any_reference& operator=(const any_reference&) = default;
    any_reference& operator=(any_reference&&) = default;
    ~any_reference() = default;

public:
    template <class T>
    typename get_specializer<T>::return_type get();

    template <class T>
    const typename get_specializer<T>::return_type get() const;

private:
    any value_ptr_;
};
}


namespace shadow
{
template <class T>
inline any_reference::any_reference(T& value) : value_ptr_(&value)
{
}

template <class T>
inline any_reference::any_reference(const T& value) : value_ptr_(&value)
{
}

template <class T>
inline typename any_reference::get_specializer<T>::return_type
any_reference::get()
{
    return std::forward<typename get_specializer<T>::return_type>(
        get_specializer<T>::get(*this));
}

template <class T>
inline const typename any_reference::get_specializer<T>::return_type
any_reference::get() const
{
    return *(value_ptr_.get<T*>());
}
}
