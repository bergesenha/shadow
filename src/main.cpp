#include <iostream>

#include <compile_time.hpp>

namespace myspace
{
REGISTER_TYPE_BEGIN()
REGISTER_TYPE(int)
REGISTER_TYPE(float)

REGISTER_TYPE(double)
REGISTER_TYPE_END()
}


template <class T>
void p(T)
{
    puts(__PRETTY_FUNCTION__);
}


int
main()
{
    p(myspace::instantiated_compile_time_infos());

    for(auto str : myspace::type_name_array_holder::value)
    {
        std::cout << str << '\n';
    }
}
