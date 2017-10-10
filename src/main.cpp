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
} // namespace myspace


int
main()
{
    auto anint = myspace::static_create<int>(23);

    try
    {
        int i = myspace::static_value_cast<float>(anint);
        std::cout << i << '\n';
    }
    catch(const shadow::type_conversion_error& err)
    {
        std::cout << err.what() << '\n';
    }

    try
    {
        int i = myspace::static_value_cast<int>(anint);
        std::cout << i << '\n';
    }
    catch(const shadow::type_conversion_error& err)
    {
        std::cout << err.what() << '\n';
    }
}
