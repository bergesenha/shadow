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
    auto intobj = myspace::static_construct<int>(0);
    auto aobj = myspace::static_construct<a>(0, 0.0);
    auto bobj = myspace::static_construct<b>('a', a{0, 0.0});


    std::istringstream intdata("10 11 12 23");

    intdata >> intobj;
    std::cout << intobj << '\n';
    intdata >> intobj;
    std::cout << intobj << '\n';
    intdata >> intobj;
    std::cout << intobj << '\n';
    intdata >> intobj;
    std::cout << intobj << '\n';

    std::istringstream adata("{1000, 734.234}");

    adata >> aobj;

    std::cout << aobj << '\n';


    std::istringstream bdata("{b, {232, 3.14}}");

    bdata >> bobj;

    std::cout << bobj;
}
