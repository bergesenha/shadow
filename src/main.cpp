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

    for(auto i : myspace::compile_time_constructor_info<
            20>::parameter_type_indices_holder::value)
    {
        std::cout << i << '\n';
    }

    typedef metamusil::t_list::type_list<
        myspace::compile_time_constructor_info<18>,
        myspace::compile_time_constructor_info<19>,
        myspace::compile_time_constructor_info<20>>
        constructor_list;

    p(constructor_list());

    typedef shadow::generate_array_of_constructor_info<constructor_list>
        constructor_info_holder;


    for(auto& ci : constructor_info_holder::value)
    {
        std::cout << ci.type_index << " " << ci.num_parameters << " "
                  << ci.parameter_type_indices[0] << '\n';
    }
}
