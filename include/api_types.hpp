#pragma once


#include <string>
#include <cstddef>
#include <cstring>

#include <any.hpp>
#include <reflection_info.hpp>


namespace shadow
{
class type_tag
{
public:
    type_tag() = default;

    type_tag(const type_info& info);

    std::string name() const;
    std::size_t size() const;

    bool operator==(const type_tag& other) const;
    bool operator!=(const type_tag& other) const;

private:
    const type_info* info_ptr_;
};


class object
{
public:
private:
    any value_;
};
}


namespace shadow
{

inline std::string
type_tag::name() const
{
    return std::string(info_ptr_->name);
}

inline std::size_t
type_tag::size() const
{
    return info_ptr_->size;
}

inline bool
type_tag::operator==(const type_tag& other) const
{
    if(info_ptr_ == other.info_ptr_)
    {
        return true;
    }
    else if(std::strcmp(info_ptr_->name, other.info_ptr_->name) == 0)
    {
        return true;
    }

    return false;
}

inline bool
type_tag::operator!=(const type_tag& other) const
{
    return !operator==(other);
}

inline type_tag::type_tag(const type_info& info) : info_ptr_(&info)
{
}
}
