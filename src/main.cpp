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
    auto an_intholder =
        myspace::static_create<intholder>(intholder(100, 200.4));

    std::cout << an_intholder << '\n';


    auto mem_var_range = an_intholder.member_variables();

    auto an_int = myspace::static_create<int>(44);

    for(; mem_var_range.first != mem_var_range.second; ++mem_var_range.first)
    {
        if(mem_var_range.first->get_type() == an_int.type())
        {
            std::cout << "found an int member variable\n";
        }

        std::cout << *mem_var_range.first << " == ";
        std::cout << an_intholder.get_member_variable(mem_var_range.first)
                  << '\n';
    }

    std::cout << "\n\n\nFree Functions:\n";
    auto free_function_range = myspace::manager.free_functions();
    for(auto i = free_function_range.first; i != free_function_range.second;
        ++i)
    {
        std::cout << *i << '\n';
    }

    // find free function 'hello'
    auto find_hello = std::find_if(
        free_function_range.first,
        free_function_range.second,
        [](const auto& ffi) { return ffi.name() == std::string("hello"); });

    if(find_hello != free_function_range.second)
    {
        auto hello = *find_hello;


        std::cout << "\n\nCalling free function 'hello'\n";
        auto return_value = hello();

        std::cout << "returned value: " << return_value << '\n';
    }

    auto find_mult = std::find_if(
        free_function_range.first,
        free_function_range.second,
        [](const auto& ffi) { return ffi.name() == std::string("mult"); });

    if(find_mult != free_function_range.second)
    {
        auto mult = *find_mult;

        std::cout << "\n\nCalling free function 'mult'\n";

        std::vector<shadow::variable> args;
        args.push_back(myspace::static_create<double>(34.2));
        args.push_back(myspace::static_create<int>(3));

        std::vector<shadow::variable> wrong_args;
        wrong_args.push_back(myspace::static_create<float>(34.2f));
        wrong_args.push_back(myspace::static_create<std::size_t>(3ul));

        auto return_value = mult(args.begin(), args.end());
        std::cout << "return value: " << return_value << '\n';

        auto return_value2 = mult.call_unsafe(args.begin(), args.end());
        std::cout << "return value: " << return_value2 << '\n';


        auto return_value3 =
            mult.call_with_conversion(wrong_args.begin(), wrong_args.end());
        std::cout << "return value: " << return_value3 << '\n';
    }

    auto find_overload1_int = std::find_if(
        free_function_range.first,
        free_function_range.second,
        [](const auto& ff) {

            return ff.name() == std::string("overload1") &&
                   ff.parameter_types().first->name() == std::string("int");
        });

    if(find_overload1_int != free_function_range.second)
    {

        std::cout << "\n\nCalling free function 'overload1(int)'\n";

        auto return_value = find_overload1_int->call_static_unsafe(23);

        std::cout << return_value << '\n';

        try
        {
            find_overload1_int->call_static_safe<myspace::type_universe>(23,
                                                                         30);
        }
        catch(const shadow::argument_error& exc)
        {
            std::cout << "caught argument_error: " << exc.what() << '\n';
        }

        auto return_value2 =
            find_overload1_int->call_static_safe<myspace::type_universe>(50);

        std::cout << return_value2 << '\n';
    }
}
