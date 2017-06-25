#pragma once

#include <utility>

#include "reflection_info.hpp"

namespace shadow
{
////////////////////////////////////////////////////////////////////////////////
// user facing API types carrying information and functionality for the
// reflection system

class reflection_manager;

// holds one value with type/reflection information
class variable
{
private:
    // holds type erased value
    any value_;
    // these identify the type erased value
    const reflection_manager* manager_;
    std::size_t type_index_;
};


template <class InfoType, template <class> class... Policies>
class api_type_aggregator
    : public Policies<api_type_aggregator<InfoType, Policies...>>...
{
public:
    api_type_aggregator() : info_(nullptr), manager_(nullptr)
    {
    }

    api_type_aggregator(const InfoType* info, const reflection_manager* manager)
        : info_(info), manager_(manager)
    {
    }

    api_type_aggregator* operator->()
    {
        return this;
    }

    const api_type_aggregator* operator->() const
    {
        return this;
    }

public:
    const InfoType* info_;
    const reflection_manager* manager_;
};


template <class Derived>
class get_name_policy
{
public:
    std::string
    name() const
    {
        return (static_cast<const Derived*>(this))->info_->name;
    }
};


template <class Derived>
class get_size_policy
{
public:
    std::size_t
    size() const
    {
        return static_cast<const Derived*>(this)->info_->size;
    }
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


namespace shadow
{
class reflection_manager
{
public:
    typedef api_type_aggregator<type_info, get_name_policy, get_size_policy>
        type;
    typedef info_iterator_<const type_info, const type> const_type_iterator;

public:
    reflection_manager() = default;

    template <class TypeInfoArrayHolder,
              class ConstructorInfoArrayHolder,
              class ConversionInfoArrayHolder,
              class StringSerializationInfoArrayHolder,
              class FreeFunctionInfoArrayHolder,
              class MemberFunctionInfoArrayHolder,
              class MemberVariableInfoArrayHolder>
    reflection_manager(TypeInfoArrayHolder,
                       ConstructorInfoArrayHolder,
                       ConversionInfoArrayHolder,
                       StringSerializationInfoArrayHolder,
                       FreeFunctionInfoArrayHolder,
                       MemberFunctionInfoArrayHolder,
                       MemberVariableInfoArrayHolder)
        : type_info_range_(initialize_range(TypeInfoArrayHolder())),
          constructor_info_range_(
              initialize_range(ConstructorInfoArrayHolder())),
          conversion_info_range_(initialize_range(ConversionInfoArrayHolder())),
          string_serialization_info_range_(
              initialize_range(StringSerializationInfoArrayHolder())),
          free_function_info_range_(
              initialize_range(FreeFunctionInfoArrayHolder())),
          member_function_info_range_(
              initialize_range(MemberFunctionInfoArrayHolder())),
          member_variable_info_range_(
              initialize_range(MemberVariableInfoArrayHolder()))
    {
    }

private:
    // generic initialization helper function
    template <class ArrayHolderType>
    std::pair<const typename ArrayHolderType::type*,
              const typename ArrayHolderType::type*>
        initialize_range(ArrayHolderType)
    {
// clang complains here due to the comparison of decayed array and
// nullptr, since in the case ArrayHolderType::value was an array, this
// would always evaluate to true. The fact is that
// ArrayHolderType::value may be a pointer to nullptr for some cases of
// ArrayHolderType.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wtautological-pointer-compare"
        if(ArrayHolderType::value != nullptr)
        {
            return std::make_pair(std::begin(ArrayHolderType::value),
                                  std::end(ArrayHolderType::value));
        }
        else
        {
            std::pair<const typename ArrayHolderType::type*,
                      const typename ArrayHolderType::type*>
                out(nullptr, nullptr);

            return out;
        }
    }
#pragma clang diagnostic pop


public:
    ////////////////////////////////////////////////////////////////////////////
    // main api interface for interacting with the reflection system
    std::pair<const_type_iterator, const_type_iterator>
    types() const
    {
        return std::make_pair(
            const_type_iterator(type_info_range_.first, this),
            const_type_iterator(type_info_range_.second, this));
    }

private:
    // pairs hold iterators to beginning and end of arrays of information
    // generated at compile time
    std::pair<const type_info*, const type_info*> type_info_range_;
    std::pair<const constructor_info*, const constructor_info*>
        constructor_info_range_;
    std::pair<const conversion_info*, const conversion_info*>
        conversion_info_range_;
    std::pair<const string_serialization_info*,
              const string_serialization_info*>
        string_serialization_info_range_;
    std::pair<const free_function_info*, const free_function_info*>
        free_function_info_range_;
    std::pair<const member_function_info*, const member_function_info*>
        member_function_info_range_;
    std::pair<const member_variable_info*, const member_variable_info*>
        member_variable_info_range_;
};
}

namespace std
{
// specialize std::iterator_traits for info_iterator_

template <class InfoType, class ProxyType>
struct std::iterator_traits<shadow::info_iterator_<InfoType, ProxyType>>
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
