#pragma once

#include <iterator>
#include <cstddef>


namespace shadow
{
class reflection_manager;


template <class InfoType, class ProxyType>
class indexed_info_iterator_
{
public:
    typedef ProxyType value_type;
    typedef std::ptrdiff_t difference_type;
    typedef ProxyType reference;
    typedef ProxyType pointer;
    typedef std::random_access_iterator_tag iterator_category;

public:
    indexed_info_iterator_()
        : current_index_(0), index_buffer_(nullptr), data_buffer_(nullptr)
    {
    }

    indexed_info_iterator_(std::size_t current_index,
                           const std::size_t* index_buffer,
                           InfoType* data_buffer,
                           const reflection_manager* manager)
        : current_index_(current_index),
          index_buffer_(index_buffer),
          data_buffer_(data_buffer),
          manager_(manager)
    {
    }

public:
    ProxyType operator*() const
    {
        return ProxyType(data_buffer_ + index_buffer_[current_index_],
                         manager_);
    }

    ProxyType operator->() const
    {
        return ProxyType(data_buffer_ + index_buffer_[current_index_],
                         manager_);
    }

    ProxyType operator[](difference_type n) const
    {
        return ProxyType(data_buffer_ + index_buffer_[current_index_ + n],
                         manager_);
    }

    indexed_info_iterator_& operator++()
    {
        ++current_index_;
        return *this;
    }

    indexed_info_iterator_ operator++(int)
    {
        auto temp = *this;
        ++(*this);
        return temp;
    }

private:
    std::size_t current_index_;
    const std::size_t* index_buffer_;
    InfoType* data_buffer_;
    const reflection_manager* manager_;
};


template <class InfoType, class ProxyType>
class info_iterator_
{
public:
    typedef ProxyType value_type;
    typedef typename std::iterator_traits<const InfoType*>::difference_type
        difference_type;
    typedef ProxyType reference;
    typedef ProxyType pointer;
    typedef typename std::iterator_traits<const InfoType*>::iterator_category
        iterator_category;

public:
    info_iterator_() = default;


    info_iterator_(const InfoType* current, const reflection_manager* manager)
        : current_(current), manager_(manager)
    {
    }

public:
    ProxyType operator*() const
    {
        return ProxyType(current_, manager_);
    }

    ProxyType operator->() const
    {
        return ProxyType(current_, manager_);
    }

    ProxyType operator[](difference_type n) const
    {
        return ProxyType(current_ + n, manager_);
    }

    info_iterator_& operator++()
    {
        ++current_;
        return *this;
    }

    info_iterator_ operator++(int)
    {
        auto temp = *this;
        ++(*this);
        return temp;
    }

    info_iterator_& operator--()
    {
        --current_;
        return *this;
    }

    info_iterator_ operator--(int)
    {
        auto temp = *this;
        --(*this);
        return temp;
    }

    info_iterator_&
    operator+=(difference_type n)
    {
        current_ += n;
        return *this;
    }

    info_iterator_&
    operator-=(difference_type n)
    {
        current_ -= n;
        return *this;
    }

    info_iterator_
    operator+(difference_type n) const
    {
        return info_iterator_(current_ + n, manager_);
    }

    info_iterator_
    operator-(difference_type n) const
    {
        return info_iterator_(current_ - n, manager_);
    }

    difference_type
    operator-(const info_iterator_& other)
    {
        return current_ - other.current_;
    }

    friend info_iterator_
    operator+(difference_type lhs, const info_iterator_& rhs)
    {
        return rhs + lhs;
    }

    bool
    operator==(const info_iterator_& other) const
    {
        return current_ == other.current_;
    }

    bool
    operator!=(const info_iterator_& other) const
    {
        return current_ != other.current_;
    }

    bool
    operator<(const info_iterator_& other) const
    {
        return current_ < other.current_;
    }

    bool
    operator>(const info_iterator_& other) const
    {
        return current_ > other.current_;
    }

    bool
    operator<=(const info_iterator_& other) const
    {
        return current_ <= other.current_;
    }

    bool
    operator>=(const info_iterator_& other) const
    {
        return current_ >= other.current_;
    }

private:
    InfoType* current_;
    const reflection_manager* manager_;
};
}


namespace std
{
// specialize std::iterator_traits for info_iterator_

template <class InfoType, class ProxyType>
struct iterator_traits<shadow::info_iterator_<InfoType, ProxyType>>
{
    typedef typename shadow::info_iterator_<InfoType, ProxyType>::value_type
        value_type;
    typedef
        typename shadow::info_iterator_<InfoType, ProxyType>::difference_type
            difference_type;
    typedef typename shadow::info_iterator_<InfoType, ProxyType>::reference
        reference;
    typedef
        typename shadow::info_iterator_<InfoType, ProxyType>::pointer pointer;
    typedef
        typename shadow::info_iterator_<InfoType, ProxyType>::iterator_category
            iterator_category;
};
}
