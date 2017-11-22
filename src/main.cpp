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
    b() = default;
    b(char k, const a& v) : key(k), value(v)
    {
    }

    char key;
    a value;
    const char&
    get_key() const
    {
        return key;
    }

    char&
    get_key()
    {
        return key;
    }

    double
    overload1(int i)
    {
        return static_cast<double>(i * 10);
    }

    double
    overload1(double d)
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
REGISTER_CONSTRUCTOR(b)
REGISTER_CONSTRUCTOR(b, char, const a&)

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
    for(auto i = myspace::manager.types(); i.first != i.second; ++i.first)
    {
        // std::cout << myspace::manager.type_name(*i.first) << '\n';
    }
    for(auto i = myspace::manager.constructors(); i.first != i.second;
        ++i.first)
    {
        std::cout << myspace::manager.type_name(
                         myspace::manager.constructor_type(*i.first))
                  << ": ";

        for(auto j = myspace::manager.constructor_parameter_types(*i.first);
            j.first != j.second;
            ++j.first)
        {
            std::cout << myspace::manager.type_name(*j.first) << ", ";
        }

        std::cout << "\n\n";
    }
    std::cout << '\n';


    for(auto i = myspace::manager.free_functions(); i.first != i.second;
        ++i.first)
    {
        std::cout << myspace::manager.type_name(
                         myspace::manager.free_function_return_type(*i.first))
                  << ' ' << myspace::manager.free_function_name(*i.first)
                  << '(';

        for(auto j = myspace::manager.free_function_parameter_types(*i.first);
            j.first != j.second;
            ++j.first)
        {
            std::cout << myspace::manager.type_name(*j.first) << ", ";
        }

        std::cout << '\n';
    }
    std::cout << '\n';

    for(auto i = myspace::manager.member_functions(); i.first != i.second;
        ++i.first)
    {
        std::cout << myspace::manager.type_name(
                         myspace::manager.member_function_return_type(*i.first))
                  << " "
                  << myspace::manager.type_name(
                         myspace::manager.member_function_object_type(*i.first))
                  << "::" << myspace::manager.member_function_name(*i.first)
                  << '(';
        for(auto j = myspace::manager.member_function_parameter_types(*i.first);
            j.first != j.second;
            ++j.first)
        {
            std::cout << myspace::manager.type_name(*j.first) << ", ";
        }
        std::cout << ")";
        if(myspace::manager.member_function_is_const(*i.first))
        {
            std::cout << " const";
        }
        std::cout << '\n';
    }

    for(auto i = myspace::manager.member_variables(); i.first != i.second;
        ++i.first)
    {
        std::cout << '.';
    }
    std::cout << '\n';
}
