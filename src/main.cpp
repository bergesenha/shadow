#include <iostream>
#include <vector>
#include <string>
#include <sstream>

#include <compile_time.hpp>
#include <algorithm>

#include <sfinae.hpp>
#include <type_list.hpp>

template <class T>
void p(T)
{
    puts(__PRETTY_FUNCTION__);
}

class intholder
{
public:
    intholder() = default;

    explicit intholder(int i) : i_(i), d_(0.0), mem_var1(i), mem_var2(0.0)
    {
    }

    intholder(int i, double d) : i_(i), d_(d), mem_var1(i), mem_var2(d)
    {
    }

    void
    member_function1(void)
    {
        std::cout << "hello member function\n";
    }

    int
    member_function2(int i, char c)
    {
        return i * c;
    }

    int
    member_overload(int i)
    {
        return i_ * i;
    }

    int
    member_overload(float f)
    {
        return d_ * f;
    }


    int mem_var1;
    double mem_var2;

private:
    int i_;
    double d_;
};


struct contains_intholder
{
    contains_intholder(float f, int i, double d) : f_(f), ih_(i, d)
    {
    }

    float f_;
    intholder ih_;
};

void
free_function1(int i)
{
    std::cout << i << '\n';
}

void
hello()
{
    std::cout << "hello\n";
}

double
mult(double d, int i)
{
    return d * i;
}


int
overload1(int i)
{
    return 3 * i;
}

int
overload1(double d)
{
    return 3 * d;
}

namespace myspace
{
REGISTER_TYPE_BEGIN()

REGISTER_TYPE(intholder)
REGISTER_TYPE(contains_intholder)
REGISTER_TYPE_END()


REGISTER_CONSTRUCTOR_BEGIN()

REGISTER_CONSTRUCTOR(intholder)
REGISTER_CONSTRUCTOR(intholder, int)
REGISTER_CONSTRUCTOR(intholder, int, double)
REGISTER_CONSTRUCTOR(contains_intholder, float, int, double)

REGISTER_CONSTRUCTOR_END()


REGISTER_FREE_FUNCTION_BEGIN()

REGISTER_FREE_FUNCTION(free_function1)
REGISTER_FREE_FUNCTION(hello)
REGISTER_FREE_FUNCTION(mult)

REGISTER_FREE_FUNCTION_EXPLICIT(overload1, int, int)
REGISTER_FREE_FUNCTION_EXPLICIT(overload1, int, double)

REGISTER_FREE_FUNCTION_END()


REGISTER_MEMBER_FUNCTION_BEGIN()

REGISTER_MEMBER_FUNCTION(intholder, member_function1)
REGISTER_MEMBER_FUNCTION(intholder, member_function2)

REGISTER_MEMBER_FUNCTION_EXPLICIT(intholder, member_overload, int, int)
REGISTER_MEMBER_FUNCTION_EXPLICIT(intholder, member_overload, int, float)

REGISTER_MEMBER_FUNCTION_END()


REGISTER_MEMBER_VARIABLE_BEGIN()

REGISTER_MEMBER_VARIABLE(intholder, mem_var1)
REGISTER_MEMBER_VARIABLE(intholder, mem_var2)

REGISTER_MEMBER_VARIABLE(contains_intholder, f_)
REGISTER_MEMBER_VARIABLE(contains_intholder, ih_)

REGISTER_MEMBER_VARIABLE_END()

SHADOW_INIT()
}


typedef shadow::api_type_aggregator<shadow::type_info,
                                    shadow::get_name_policy,
                                    shadow::get_size_policy>
    myapitype;

int
main()
{
    auto an_intholder =
        myspace::static_create<intholder>(intholder(100, 200.4));

    std::cout << an_intholder << '\n';

    auto mem_var_range = an_intholder.member_variables();

    for(; mem_var_range.first != mem_var_range.second; ++mem_var_range.first)
    {
        std::cout << *mem_var_range.first << " == ";
        std::cout << an_intholder.get_member_variable(*mem_var_range.first)
                  << '\n';
    }
}
