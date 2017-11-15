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


typedef shadow::generate_array_of_type_descriptions<
    metamusil::t_list::type_list<int, const int&, double*>,
    myspace::type_universe>
    type_description_array_holder;


void p(const shadow::type_description& desc)
{
    std::cout << myspace::type_name_array_holder::value[desc.type_index] << ' ';

    for(auto i = 0ul ; i < desc.num_attributes; ++i)
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
    for(const auto& i : type_description_array_holder::value)
    {
        p(i);
    }
}
