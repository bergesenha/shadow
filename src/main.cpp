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


template <class TypePair, class AllTypesList>
struct extract_conversion_info;

template <class To, class From, class AllTypesList>
struct extract_conversion_info<shadow::type_pair<To, From>, AllTypesList>
{
    static constexpr shadow::conversion_info value = {
        metamusil::t_list::index_of_type_v<AllTypesList, From>,
        metamusil::t_list::index_of_type_v<AllTypesList, To>,
        &shadow::conversion_detail::generic_conversion_bind_point<To, From>};
};

template <class To, class From, class AllTypesList>
constexpr shadow::conversion_info
    extract_conversion_info<shadow::type_pair<To, From>, AllTypesList>::value;

template <class TypePair>
using bind_extract_conversion_info =
    extract_conversion_info<TypePair, myspace::type_universe>;

typedef metamusil::t_list::value_transform<
    myspace::valid_conversion_combinations,
    bind_extract_conversion_info>
    conversion_info_holder;


int
main()
{
    for(auto& ci : conversion_info_holder::value)
    {
        std::cout << myspace::type_name_array_holder::value[ci.from_type_index]
                  << " -> "
                  << myspace::type_name_array_holder::value[ci.to_type_index]
                  << '\n';
    }
}

