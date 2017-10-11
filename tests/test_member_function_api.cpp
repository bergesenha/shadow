#include "catch.hpp"

#include <shadow.hpp>

class tmfapi1
{
public:
    int public_value;

    void
    fun1()
    {
        public_value = 100;
    }

    void
    fun2(int i)
    {
        public_value = i;
    }

    int
    fun3() const
    {
        return 23;
    }

    int
    fun4(int i) const
    {
        return 2 * i;
    }
};


namespace tmpfapi_space
{
REGISTER_TYPE_BEGIN()
REGISTER_TYPE(tmfapi1)
REGISTER_TYPE_END()

REGISTER_MEMBER_FUNCTION(tmfapi1, fun1)
REGISTER_MEMBER_FUNCTION(tmfapi1, fun2)
REGISTER_MEMBER_FUNCTION(tmfapi1, fun3)
REGISTER_MEMBER_FUNCTION(tmfapi1, fun4)

SHADOW_INIT()
}
