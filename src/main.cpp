#include <iostream>
#include <vector>
#include <string>

#include <compile_time.hpp>

#include <sfinae.hpp>
#include <type_list.hpp>

template <class T>
void p(T)
{
    puts(__PRETTY_FUNCTION__);
}

class intholder
{
public:
    intholder() = default;

    explicit intholder(int i) : i_(i), d_(0.0)
    {
    }

    intholder(int i, double d) : i_(i), d_(d)
    {
    }

private:
    int i_;
    double d_;
};


namespace myspace
{
REGISTER_TYPE_BEGIN()

REGISTER_TYPE(intholder)
REGISTER_TYPE_END()

REGISTER_CONSTRUCTOR_BEGIN()


REGISTER_CONSTRUCTOR(intholder)
REGISTER_CONSTRUCTOR(intholder, int)
REGISTER_CONSTRUCTOR(intholder, int, double)

REGISTER_CONSTRUCTOR_END()

SHADOW_INIT()
}


int
main()
{
}

