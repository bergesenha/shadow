#include "catch.hpp"

#include <shadow.hpp>


class tct1_class
{
public:
    tct1_class(int i) : i_(i)
    {
    }

    int
    get_i() const
    {
        return i_;
    }

private:
    int i_;
};


int
extract_i(const tct1_class& a)
{
    return a.get_i();
}


namespace tct1_space
{
REGISTER_TYPE_BEGIN()
REGISTER_TYPE(tct1_class)
REGISTER_TYPE_END()

REGISTER_FREE_FUNCTION(extract_i)

SHADOW_INIT()
}


namespace tct1_space2
{
REGISTER_TYPE_BEGIN()
REGISTER_TYPE(tct1_class)
REGISTER_TYPE_END()

REGISTER_CONSTRUCTOR(tct1_class, int)

REGISTER_MEMBER_FUNCTION(tct1_class, get_i)

SHADOW_INIT()
}

TEST_CASE("create an int using static_construct", "[static_construct]")
{
    auto anint = tct1_space::static_construct<int>(23);

    REQUIRE(anint.type().name() == std::string("int"));
}
