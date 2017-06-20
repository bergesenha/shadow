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

    intholder(int i) : i_(i), d_(0.0)
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


template <class A, class B>
struct mypair
{
};


typedef metamusil::t_list::
    for_each_combination_t<myspace::fundamental_type_universe, mypair>
        combinations;

template <class Pair>
struct is_defined_predicate;

template <class A, class B>
struct is_defined_predicate<mypair<A, B>>
{
    static const bool value = metamusil::specialization_defined_v<
        shadow::conversion_detail::conversion_specializer,
        A,
        B>;
};

typedef metamusil::t_list::filter_t<combinations, is_defined_predicate>
    filtered_combinations;

int
main()
{
    std::cout
        << "number of type combinations: "
        << metamusil::t_list::length_v<myspace::type_combinations> << '\n';

    std::cout << "number of valid conversion combinations: "
              << metamusil::t_list::length_v<
                     myspace::valid_conversion_combinations> << '\n';
}

