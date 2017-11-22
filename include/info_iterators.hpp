#pragma once

#include <iterator>

namespace shadow
{
class reflection_manager;
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


    explicit info_iterator_(const InfoType* current,
                            const reflection_manager* man)
        : current_(current), manager_(man)
    {
    }

public:
    ProxyType operator*() const
    {
        return ProxyType(*current_, *manager_);
    }

    ProxyType operator->() const
    {
        return ProxyType(*current_, *manager_);
    }

    ProxyType operator[](difference_type n) const
    {
        return ProxyType(*(current_ + n), *manager_);
    }

    info_iterator_& operator++()
    {
        ++current_;
        return *this;
    }

    const info_iterator_ operator++(int)
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

    const info_iterator_ operator--(int)
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
        return info_iterator_(current_ + n);
    }

    info_iterator_
    operator-(difference_type n) const
    {
        return info_iterator_(current_ - n);
    }

    difference_type
    operator-(const info_iterator_& other) const
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
                           InfoType* data_buffer)
        : current_index_(current_index),
          index_buffer_(index_buffer),
          data_buffer_(data_buffer)
    {
    }

public:
    ProxyType operator*() const
    {
        return ProxyType(*(data_buffer_ + index_buffer_[current_index_]));
    }

    ProxyType operator->() const
    {
        return ProxyType(*(data_buffer_ + index_buffer_[current_index_]));
    }

    ProxyType operator[](difference_type n) const
    {
        return ProxyType(*(data_buffer_ + index_buffer_[current_index_ + n]));
    }

    indexed_info_iterator_& operator++()
    {
        ++current_index_;
        return *this;
    }

    const indexed_info_iterator_ operator++(int)
    {
        auto temp = *this;
        ++(*this);
        return temp;
    }

    indexed_info_iterator_& operator--()
    {
        --current_index_;
        return *this;
    }

    const indexed_info_iterator_ operator--(int)
    {
        auto temp = *this;
        --(*this);
        return temp;
    }

    indexed_info_iterator_&
    operator+=(difference_type n)
    {
        current_index_ += n;
        return *this;
    }

    indexed_info_iterator_&
    operator-=(difference_type n)
    {
        current_index_ -= n;
        return *this;
    }

    indexed_info_iterator_
    operator+(difference_type n) const
    {
        return indexed_info_iterator_(
            current_index_ + n, index_buffer_, data_buffer_);
    }

    indexed_info_iterator_
    operator-(difference_type n) const
    {
        return indexed_info_iterator_(
            current_index_ - n, index_buffer_, data_buffer_);
    }

    difference_type
    operator-(const indexed_info_iterator_& other) const
    {
        return current_index_ - other.current_index_;
    }

    friend indexed_info_iterator_
    operator+(difference_type lhs, const indexed_info_iterator_& rhs)
    {
        return rhs + lhs;
    }

    bool
    operator==(const indexed_info_iterator_& other) const
    {
        return current_index_ == other.current_index_;
    }

    bool
    operator!=(const indexed_info_iterator_& other) const
    {
        return current_index_ != other.current_index_;
    }

    bool
    operator<(const indexed_info_iterator_& other) const
    {
        return current_index_ < other.current_index_;
    }

    bool
    operator>(const indexed_info_iterator_& other) const
    {
        return current_index_ > other.current_index_;
    }

    bool
    operator<=(const indexed_info_iterator_& other) const
    {
        return current_index_ <= other.current_index_;
    }

    bool
    operator>=(const indexed_info_iterator_& other) const
    {
        return current_index_ >= other.current_index_;
    }

private:
    std::size_t current_index_;
    const std::size_t* index_buffer_;
    InfoType* data_buffer_;
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


template <class InfoType, class ProxyType>
struct iterator_traits<shadow::indexed_info_iterator_<InfoType, ProxyType>>
{
    typedef
        typename shadow::indexed_info_iterator_<InfoType, ProxyType>::value_type
            value_type;

    typedef typename shadow::indexed_info_iterator_<InfoType,
                                                    ProxyType>::difference_type
        difference_type;

    typedef
        typename shadow::indexed_info_iterator_<InfoType, ProxyType>::reference
            reference;

    typedef
        typename shadow::indexed_info_iterator_<InfoType, ProxyType>::pointer
            pointer;

    typedef
        typename shadow::indexed_info_iterator_<InfoType,
                                                ProxyType>::iterator_category
            iterator_category;
};
}
