#include <iostream>
#include <sstream>
#include <algorithm>
#include <shadow.hpp>
#include <type_traits>

struct a
{
    int i;
    double d;
};

struct b
{
    char key;
    a value;
};

int&
get_i(a& aa)
{
    return aa.i;
}

void
long_function(int i, const int& ii, int* iii)
{
}

void void_fun()
{
}

double mult(int n, const double& d)
{
    return n * d;
}

double mult(double n, const double& d)
{
    return n * d;
}

namespace myspace
{
REGISTER_TYPE_BEGIN()
REGISTER_TYPE(a)
REGISTER_TYPE(b)
REGISTER_TYPE_END()

REGISTER_CONSTRUCTOR(a, int, double)
REGISTER_CONSTRUCTOR(a*, a*)

REGISTER_FREE_FUNCTION(get_i)
REGISTER_FREE_FUNCTION(long_function)
REGISTER_FREE_FUNCTION(void_fun)

REGISTER_FREE_FUNCTION_EXPLICIT(mult, double, int, const double&)
REGISTER_FREE_FUNCTION_EXPLICIT(mult, double, double, const double&)

REGISTER_MEMBER_VARIABLE(a, i)
REGISTER_MEMBER_VARIABLE(a, d)

REGISTER_MEMBER_VARIABLE(b, key)
REGISTER_MEMBER_VARIABLE(b, value)

SHADOW_INIT()
} // namespace myspace


void
p(const shadow::type_description& desc)
{
    std::cout << myspace::type_name_array_holder::value[desc.type_index] << ' ';

    for(auto i = 0ul; i < desc.num_attributes; ++i)
    {
        auto att = desc.attributes[i];

        if(att == shadow::type_attribute::const_tag)
        {
            std::cout << "const ";
        }
        if(att == shadow::type_attribute::pointer_tag)
        {
            std::cout << "* ";
        }
        if(att == shadow::type_attribute::lreference_tag)
        {
            std::cout << "& ";
        }
    }
    std::cout << '\n';
}

namespace shadow
{
std::ostream&
operator<<(std::ostream& out, const type_description& td)
{
    out << myspace::type_name_array_holder::value[td.type_index] << " ";
    for(auto i = 0ul ; i < td.num_attributes; ++i)
    {
        auto att = td.attributes[i];

        if(att == type_attribute::const_tag)
        {
            out << "const ";
        }
        if(att == type_attribute::pointer_tag)
        {
            out << "* ";
        }
        if(att == type_attribute::lreference_tag)
        {
            out << "& ";
        }
        if(att == type_attribute::rreference_tag)
        {
            out << "&& ";
        }
    }
    return out;
}

std::ostream& operator<<(std::ostream& out, const free_function_info& ffi)
{
    out << *ffi.return_type << " ";
    out << ffi.name << '(';
    
    for(auto i = 0ul; i < ffi.num_parameters ; ++i)
    {
        out << ffi.parameter_types[i] << ", ";
    }

    out << ')';

    return out;
}
}

int
main()
{
    std::cout << std::extent<decltype(myspace::constructor_info_array_holder::value)>::value << '\n';
}
