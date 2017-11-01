#pragma once

#include <stdexcept>

namespace shadow
{
class argument_error : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};


class type_error : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};
}
