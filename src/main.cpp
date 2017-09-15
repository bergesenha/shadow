#include <iostream>
#include <vector>
#include <string>
#include <sstream>

#include <compile_time.hpp>
#include <algorithm>

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

    explicit intholder(int i) : mem_var1(i), mem_var2(0.0)
    {
    }

    intholder(int i, double d) : mem_var1(i), mem_var2(d)
    {
    }

    void
    member_function1(void)
    {
        std::cout << "hello member function\n";
    }

    int
    member_function2(int i, char c)
    {
        return i * c;
    }

    int
    member_overload(int i)
    {
        return mem_var1 * i;
    }

    int
    member_overload(float f)
    {
        return mem_var2 * f;
    }


    int mem_var1;
    double mem_var2;
};


struct contains_intholder
{
    contains_intholder(float f, int i, double d) : f_(f), ih_(i, d)
    {
    }

    float f_;
    intholder ih_;
};

void
free_function1(int i)
{
    std::cout << i << '\n';
}

void
hello()
{
    std::cout << "hello\n";
}

double
mult(double d, int i)
{
    return d * i;
}


int
overload1(int i)
{
    return 3 * i;
}

int
overload1(double d)
{
    return 3 * d;
}

namespace myspace
{
REGISTER_TYPE_BEGIN()

REGISTER_TYPE(intholder)
REGISTER_TYPE(contains_intholder)
REGISTER_TYPE_END()


REGISTER_CONSTRUCTOR_BEGIN()

REGISTER_CONSTRUCTOR(intholder)
REGISTER_CONSTRUCTOR(intholder, int)
REGISTER_CONSTRUCTOR(intholder, int, double)
REGISTER_CONSTRUCTOR(contains_intholder, float, int, double)

REGISTER_CONSTRUCTOR_END()


REGISTER_FREE_FUNCTION_BEGIN()

REGISTER_FREE_FUNCTION(free_function1)
REGISTER_FREE_FUNCTION(hello)
REGISTER_FREE_FUNCTION(mult)

REGISTER_FREE_FUNCTION_EXPLICIT(overload1, int, int)
REGISTER_FREE_FUNCTION_EXPLICIT(overload1, int, double)

REGISTER_FREE_FUNCTION_END()


REGISTER_MEMBER_FUNCTION_BEGIN()

REGISTER_MEMBER_FUNCTION(intholder, member_function1)
REGISTER_MEMBER_FUNCTION(intholder, member_function2)

REGISTER_MEMBER_FUNCTION_EXPLICIT(intholder, member_overload, int, int)
REGISTER_MEMBER_FUNCTION_EXPLICIT(intholder, member_overload, int, float)

REGISTER_MEMBER_FUNCTION_END()


REGISTER_MEMBER_VARIABLE_BEGIN()

REGISTER_MEMBER_VARIABLE(intholder, mem_var1)
REGISTER_MEMBER_VARIABLE(intholder, mem_var2)

REGISTER_MEMBER_VARIABLE(contains_intholder, f_)
REGISTER_MEMBER_VARIABLE(contains_intholder, ih_)

REGISTER_MEMBER_VARIABLE_END()

SHADOW_INIT()
}


int
main()
{
    // find type of intholder
    auto types_pair = myspace::manager.types();

    auto found_intholder =
        std::find_if(types_pair.first, types_pair.second, [](const auto& tp) {
            return tp.name() == std::string("intholder");
        });

    if(found_intholder == types_pair.second)
    {
        std::cerr << "unable to find intholder type\n";
        exit(EXIT_FAILURE);
    }


    // get constructors for intholder
    auto constructors_pair =
        myspace::manager.constructors_by_type(*found_intholder);

    std::cout << std::distance(constructors_pair.first,
                               constructors_pair.second)
              << " constructors found for: " << *found_intholder << '\n';

    // find constructor taking two arguments
    auto found_constructor =
        std::find_if(constructors_pair.first,
                     constructors_pair.second,
                     [](const auto& ctr) { return ctr.num_parameters() == 2; });

    if(found_constructor == constructors_pair.second)
    {
        std::cerr << "constructor taking two arguments not found\n";
        exit(EXIT_FAILURE);
    }


    // construct arguments
    shadow::variable args[] = {myspace::static_create<int>(23),
                               myspace::static_create<double>(2.534)};

    // construct an intholder variable
    try
    {
        auto intholder_variable = myspace::manager.construct(
            *found_constructor, std::begin(args), std::end(args));

        std::cout << intholder_variable << '\n';
    }
    catch(const shadow::argument_error& err)
    {
        std::cerr << "construction failed with: " << err.what() << '\n';
    }


    // find default constructor
    auto found_default_ctor =
        std::find_if(constructors_pair.first,
                     constructors_pair.second,
                     [](const auto& ctr) { return ctr.num_parameters() == 0; });

    if(found_default_ctor == constructors_pair.second)
    {
        std::cerr << "default constructor not found\n";
        exit(EXIT_FAILURE);
    }

    try
    {
        auto intholder_variable =
            myspace::manager.construct(*found_default_ctor);
        std::cout << intholder_variable << '\n';
    }
    catch(const shadow::argument_error& err)
    {
        std::cerr << "constructor failed with: " << err.what() << '\n';
    }


    // construct intholder variable using static_create
    {
        auto intholder_variable = myspace::static_create<intholder>(10, 23.5);
        std::cout << intholder_variable << '\n';
    }
}
