#include <iostream>
#include <vector>
#include <string>

#include <compile_time.hpp>

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

SHADOW_INIT()
}


int
main()
{


    shadow::any anint = 234;
    shadow::any args[] = {10.3, 200};

    myspace::free_function_info_array_holder::value[0].bind_point(&anint);
    myspace::free_function_info_array_holder::value[1].bind_point(nullptr);
    auto result =
        myspace::free_function_info_array_holder::value[2].bind_point(args);

    std::cout << result.get<double>() << '\n';

    for(auto& ffi : myspace::free_function_info_array_holder::value)
    {
        std::cout << ffi.name << '\n';
    }
}

