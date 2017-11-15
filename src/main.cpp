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

int&
get_i(a& aa)
{
    return aa.i;
}

void
long_function(int i, const int& ii, int* iii)
{
}

namespace myspace
{
REGISTER_TYPE_BEGIN()
REGISTER_TYPE(a)
REGISTER_TYPE(b)
REGISTER_TYPE_END()

REGISTER_FREE_FUNCTION(get_i)
REGISTER_FREE_FUNCTION(long_function)

REGISTER_MEMBER_VARIABLE(a, i)
REGISTER_MEMBER_VARIABLE(a, d)

REGISTER_MEMBER_VARIABLE(b, key)
REGISTER_MEMBER_VARIABLE(b, value)

SHADOW_INIT()
} // namespace myspace


void
p(const shadow::type_description& desc)
{
    std::cout << myspace::type_name_array_holder::value[desc.type_index] << ' ';

    for(auto i = 0ul; i < desc.num_attributes; ++i)
    {
        auto att = desc.attributes[i];

        if(att == shadow::type_attribute::const_tag)
        {
            std::cout << "const ";
        }
        if(att == shadow::type_attribute::pointer_tag)
        {
            std::cout << "* ";
        }
        if(att == shadow::type_attribute::lreference_tag)
        {
            std::cout << "& ";
        }
    }
    std::cout << '\n';
}

int
main()
{
    auto ffinfo = shadow::extract_free_function_info<
        myspace::compile_time_ff_info<38>>::value;

    std::cout << ffinfo.name << '\n';
    p(*ffinfo.return_type);
    for(auto i = 0ul ; i < ffinfo.num_parameters ; ++i)
    {
        p(ffinfo.parameter_types[i]);
    }
}
