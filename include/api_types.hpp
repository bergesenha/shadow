#pragma once


#include <string>
#include <cstddef>

#include <any.hpp>
#include <reflection_info.hpp>

namespace shadow
{

class type_tag
{
public:
    std::string name() const;
    std::size_t size() const;

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
}
