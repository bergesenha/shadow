#include <iostream>
#include <sstream>
#include <algorithm>
#include <shadow.hpp>
#include <type_traits>

struct a
{
    int i;
    double d;
    void
    set_i(int ii)
    {
        i = ii;
    }

    int
    get_i() const
    {
        return i;
    }
};

struct b
{
    char key;
    a value;
    const char& get_key() const
    {
        return key;
    }

    char& get_key()
    {
        return key;
    }

    double overload1(int i)
    {
        return static_cast<double>(i * 10);
    }

    double overload1(double d)
    {
        return d * 10.0;
    }
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

void
void_fun()
{
}

double
mult(int n, const double& d)
{
    return n * d;
}

double
mult(double n, const double& d)
{
    return n * d;
}

typedef const char& refconstchar;
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

REGISTER_MEMBER_FUNCTION(a, set_i)
REGISTER_MEMBER_FUNCTION(a, get_i)

REGISTER_MEMBER_FUNCTION_EXPLICIT(b, overload1, double, int)
REGISTER_MEMBER_FUNCTION_EXPLICIT(b, overload1, double, double)

REGISTER_MEMBER_FUNCTION_EXPLICIT(b, get_key, char&)
REGISTER_CONST_MEMBER_FUNCTION_EXPLICIT(b, get_key, const char&)

REGISTER_MEMBER_VARIABLE(a, i)
REGISTER_MEMBER_VARIABLE(a, d)

REGISTER_MEMBER_VARIABLE(b, key)
REGISTER_MEMBER_VARIABLE(b, value)

SHADOW_INIT()
} // namespace myspace


int
main()
{
    for(auto i = myspace::manager.types() ; i.first != i.second ; ++i.first)
    {
        std::cout << i.first->name() << " " << i.first->size() << '\n';
    }
}
