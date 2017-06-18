#include <iostream>
#include <vector>
#include <string>

#include <compile_time.hpp>


class intholder
{
public:
    intholder() = default;

    intholder(int i) : i_(i)
    {
    }

private:
    int i_;
};


namespace myspace
{
REGISTER_TYPE_BEGIN()
REGISTER_TYPE(int)
REGISTER_TYPE(float)

REGISTER_TYPE(double)

REGISTER_TYPE(intholder)
REGISTER_TYPE_END()

REGISTER_CONSTRUCTOR_BEGIN()

REGISTER_CONSTRUCTOR(int, int)
REGISTER_CONSTRUCTOR(float, float)
REGISTER_CONSTRUCTOR(double, double)

REGISTER_CONSTRUCTOR(intholder, int)

REGISTER_CONSTRUCTOR_END()

SHADOW_INIT()
}


template <class T>
void p(T)
{
    puts(__PRETTY_FUNCTION__);
}

int
main()
{
}
