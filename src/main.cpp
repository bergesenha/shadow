#include <iostream>
#include <vector>
#include <string>

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

    explicit intholder(int i) : i_(i), d_(0.0)
    {
    }

    intholder(int i, double d) : i_(i), d_(d)
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
REGISTER_TYPE_END()


REGISTER_CONSTRUCTOR_BEGIN()

REGISTER_CONSTRUCTOR(intholder)
REGISTER_CONSTRUCTOR(intholder, int)
REGISTER_CONSTRUCTOR(intholder, int, double)

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
    std::cout << "Types and Constructors:\n";
    for(auto i = myspace::manager.types().first;
        i != myspace::manager.types().second;
        ++i)
    {
        std::cout << i->name() << ":\n";
        for(auto j = myspace::manager.constructors(*i).first;
            j != myspace::manager.constructors(*i).second;
            ++j)
        {
            std::cout << "\t" << *j << '\n';
        }
    }

    std::cout << "\n\n\nType Conversions:\n";


    for(auto i = myspace::manager.type_conversions().first;
        i != myspace::manager.type_conversions().second;
        ++i)
    {
        std::cout << i->from_type() << " --> " << i->to_type() << '\n';
    }

    std::cout << "\n\n\nFree Functions:\n";

    for(auto i = myspace::manager.free_functions().first;
        i != myspace::manager.free_functions().second;
        ++i)
    {
        std::cout << i->return_type() << " " << i->name() << "(";
        for(auto j = i->parameter_types().first;
            j != i->parameter_types().second;
            ++j)
        {
            std::cout << *j << ", ";
        }
        std::cout << ")\n";
    }
}
