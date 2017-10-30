#include <iostream>
#include <sstream>
#include <algorithm>
#include <shadow.hpp>
#include <type_traits>

struct a
{
    int i;
    double d;
};


namespace myspace
{
REGISTER_TYPE_BEGIN()
REGISTER_TYPE(a)
REGISTER_TYPE_END()


REGISTER_MEMBER_VARIABLE(a, i)
REGISTER_MEMBER_VARIABLE(a, d)

SHADOW_INIT()
} // namespace myspace


int
main()
{
    auto intobj = myspace::static_construct<int>(112);
    auto aobj = myspace::static_construct<a>(23, 345.146);

    std::cout << intobj << '\n';
    std::cout << aobj << '\n';
}
