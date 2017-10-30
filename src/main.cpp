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

struct b
{
    char key;
    a value;
};

namespace myspace
{
REGISTER_TYPE_BEGIN()
REGISTER_TYPE(a)
REGISTER_TYPE(b)
REGISTER_TYPE_END()


REGISTER_MEMBER_VARIABLE(a, i)
REGISTER_MEMBER_VARIABLE(a, d)

REGISTER_MEMBER_VARIABLE(b, key)
REGISTER_MEMBER_VARIABLE(b, value)

SHADOW_INIT()
} // namespace myspace


int
main()
{
    auto intobj = myspace::static_construct<int>(112);
    auto aobj = myspace::static_construct<a>(23, 345.146);
    auto bobj = myspace::static_construct<b>('a', a{40, 23.653});

    std::cout << intobj << '\n';
    std::cout << aobj << '\n';
    std::cout << bobj << "\n\n\n";

    std::istringstream data("10 11 12 23");

    data >> intobj;
    std::cout << intobj << '\n';
    data >> intobj;
    std::cout << intobj << '\n';
    data >> intobj;
    std::cout << intobj << '\n';
    data >> intobj;
    std::cout << intobj << '\n';
}
