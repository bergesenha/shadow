#include <iostream>
#include <vector>
#include <string>

#include <compile_time.hpp>


class intholder
{
public:
    intholder() = default;

    intholder(int i) : i_(i)
    {
    }

private:
    int i_;
};


namespace myspace
{
REGISTER_TYPE_BEGIN()

REGISTER_TYPE(intholder)
REGISTER_TYPE_END()

REGISTER_CONSTRUCTOR_BEGIN()


REGISTER_CONSTRUCTOR(intholder)
REGISTER_CONSTRUCTOR(intholder, int)

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
    for(auto& ci : myspace::constructor_info_array_holder::value)
    {
        std::cout << myspace::type_info_array_holder::value[ci.type_index].name
                  << "(";

        for(auto i = 0; i < ci.num_parameters; ++i)
        {
            std::cout << myspace::type_info_array_holder::value
                             [ci.parameter_type_indices[i]]
                                 .name
                      << ',';
        }

        std::cout << ")\n";
    }
}
