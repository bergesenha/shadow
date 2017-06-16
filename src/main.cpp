#include <iostream>
#include <vector>
#include <string>

#include <compile_time.hpp>

namespace myspace
{
REGISTER_TYPE_BEGIN()
REGISTER_TYPE(int)
REGISTER_TYPE(float)

REGISTER_TYPE(double)
REGISTER_TYPE_END()

REGISTER_CONSTRUCTOR_BEGIN()

REGISTER_CONSTRUCTOR(int, int)
REGISTER_CONSTRUCTOR(float, float)
REGISTER_CONSTRUCTOR(double, double)

REGISTER_CONSTRUCTOR_END()

SHADOW_INIT()
}


template <class T>
void p(T)
{
    puts(__PRETTY_FUNCTION__);
}

int
main()
{
    p(myspace::type_universe());
    std::cout << myspace::compile_time_constructor_info<20>::type_index << '\n';

    auto double_constructor =
        myspace::compile_time_constructor_info<20>::bind_point;

    shadow::any adouble = 25.3;

    auto constructed_double = double_constructor(&adouble);

    std::cout << constructed_double.get<double>() << '\n';

    std::cout << myspace::compile_time_constructor_info<20>::num_parameters
              << '\n';

    p(myspace::compile_time_constructor_info<20>::parameter_index_sequence());
}
