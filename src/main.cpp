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


int
free_function1(int i)
{
    return i * 2;
}

double
free_function_ol(double d)
{
    return 2.0 * d;
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

REGISTER_FREE_FUNCTION_END()

SHADOW_INIT()
}


int
main()
{
}

