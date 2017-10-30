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


typedef metamusil::t_list::type_list<int, char, std::string> types;

typedef typename shadow::generate_array_of_serialization_info<
    myspace::type_universe>::type array_holder;

int
main()
{
    std::for_each(std::begin(array_holder::value),
                  std::end(array_holder::value),
                  [](const auto& si) { std::cout << si.type_index << '\n'; });
}
