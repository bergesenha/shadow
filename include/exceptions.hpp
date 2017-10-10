#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP


#include <stdexcept>


namespace shadow
{
class argument_error : public std::runtime_error
{
public:
    using runtime_error::runtime_error;
};


class type_conversion_error : public std::runtime_error
{
public:
    using runtime_error::runtime_error;
};


class type_error : public std::runtime_error
{
public:
    using runtime_error::runtime_error;
};
} // namespace shadow

#endif
