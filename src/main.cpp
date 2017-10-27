#include <shadow.hpp>
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
}
