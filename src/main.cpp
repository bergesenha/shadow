#include <iostream>
#include <vector>
#include <string>

#include <shadow.hpp>
#include <algorithm>

namespace myspace
{
REGISTER_TYPE_BEGIN()

REGISTER_TYPE_END()


SHADOW_INIT()
}


int
main()
{
    auto anint = myspace::static_create<int>(23);

    int i = myspace::static_value_cast<int>(anint);

    std::cout << i << '\n';
}
