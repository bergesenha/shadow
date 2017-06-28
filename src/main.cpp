#include <iostream>
#include <vector>
#include <string>

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

    explicit intholder(int i) : i_(i), d_(0.0), mem_var1(i), mem_var2(0.0)
    {
    }

    intholder(int i, double d) : i_(i), d_(d), mem_var1(i), mem_var2(d)
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
        return i_ * i;
    }

    int
    member_overload(float f)
    {
        return d_ * f;
    }


    int mem_var1;
    double mem_var2;

private:
    int i_;
    double d_;
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


typedef shadow::api_type_aggregator<shadow::type_info,
                                    shadow::get_name_policy,
                                    shadow::get_size_policy>
    myapitype;

int
main()
{
    std::cout << "Types and Constructors:\n";
    for(auto i = myspace::manager.types().first;
        i != myspace::manager.types().second;
        ++i)
    {
        std::cout << i->name() << ":\n";
        for(auto j = myspace::manager.constructors_by_type(*i).first;
            j != myspace::manager.constructors_by_type(*i).second;
            ++j)
        {
            std::cout << "\t" << *j << '\n';
        }
        for(auto j = myspace::manager.type_conversions_by_type(*i).first;
            j != myspace::manager.type_conversions_by_type(*i).second;
            ++j)
        {
            std::cout << '\t' << j->from_type() << " --> " << j->to_type()
                      << '\n';
        }
        for(auto j = myspace::manager.member_functions_by_type(*i).first;
            j != myspace::manager.member_functions_by_type(*i).second;
            ++j)
        {
            std::cout << "\t" << *j << '\n';
        }
        for(auto j = myspace::manager.member_variables_by_type(*i).first;
            j != myspace::manager.member_variables_by_type(*i).second;
            ++j)
        {
            std::cout << "\t" << *j << '\n';
        }
        std::cout << '\n';
    }


    std::cout << "\n\n\nFree Functions:\n";
    for(auto i = myspace::manager.free_functions().first;
        i != myspace::manager.free_functions().second;
        ++i)
    {
        std::cout << *i << '\n';
    }


    std::cout << "\n\n\nString Serializers:\n";
    for(auto i = myspace::manager.string_serializers().first;
        i != myspace::manager.string_serializers().second;
        ++i)
    {
        std::cout << i->get_type() << '\n';
    }

    auto my_int_var =
        myspace::manager.static_create<myspace::type_universe>(10);

    std::cout << "\n\n\n";
    std::cout << my_int_var.type() << ":\n" << my_int_var << '\n';

    auto my_intholder_var =
        myspace::manager.static_create<myspace::type_universe>(
            intholder(13, 13.4));


    std::cout << "\n\n\n";
    std::cout << my_intholder_var << '\n';


    auto my_float_var = myspace::static_create<float>(24.5f);

    std::cout << "\n\n\n";
    std::cout << my_float_var.type() << ":\n" << my_float_var << '\n';


    auto my_contains_intholder_var = myspace::static_create<contains_intholder>(
        contains_intholder(3.4f, 3, 100.25));

    std::cout << "\n\n\n";
    std::cout << my_contains_intholder_var.type() << ":\n"
              << my_contains_intholder_var << '\n';


    std::cin >> my_int_var;

    std::cout << my_int_var << '\n';
}
