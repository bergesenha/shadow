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

typedef shadow::generate_type_description<const int (&)[10],
                                          myspace::type_universe>
    type_description_holder;

int
main()
{
    std::cout << myspace::type_name_array_holder::value
                     [shadow::generate_type_description_v<const int (&)[10], myspace::type_universe>.type_index]
              << ' ';

    for(auto i = 0ul; i < type_description_holder::value.num_attributes; ++i)
    {
        auto att = type_description_holder::value.attributes[i];
        if(att == shadow::type_attribute::const_tag)
        {
            std::cout << "const ";
        }
        else if(att == shadow::type_attribute::lreference_tag)
        {
            std::cout << "& ";
        }
        else if(att == shadow::type_attribute::pointer_tag)
        {
            std::cout << "* ";
        }
        else
        {
            std::cout << '[' << att << "] ";
        }
    }

    std::cout << '\n';
}
