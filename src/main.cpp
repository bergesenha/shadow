#include <iostream>
#include <vector>
#include <string>

#include <compile_time.hpp>

namespace myspace
{
REGISTER_TYPE_BEGIN()
REGISTER_TYPE(int)
REGISTER_TYPE(float)

REGISTER_TYPE(double)
REGISTER_TYPE_END()

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
