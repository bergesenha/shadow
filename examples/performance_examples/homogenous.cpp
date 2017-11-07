#include <iostream>
#include <numeric>
#include <utility>
#include <iterator>
#include <vector>
#include <random>
#include <chrono>
#include <cassert>
#include <unordered_map>

#include <shadow.hpp>


template <class InputIt>
typename InputIt::value_type
average(InputIt first, InputIt last)
{
    const auto sum = std::accumulate(first, last, 0.0);
    return sum / std::distance(first, last);
}

double
pluss(double a, double b)
{
    return a + b;
}

namespace refl
{
typedef std::vector<double> vector_double;
typedef typename vector_double::iterator vector_double_iterator;
REGISTER_TYPE_BEGIN()
REGISTER_TYPE(vector_double)
REGISTER_TYPE(vector_double_iterator)
REGISTER_TYPE_END()

REGISTER_FREE_FUNCTION(average<vector_double_iterator>)
REGISTER_FREE_FUNCTION(pluss)

REGISTER_MEMBER_FUNCTION_EXPLICIT(vector_double, begin, vector_double_iterator)
REGISTER_MEMBER_FUNCTION_EXPLICIT(vector_double, end, vector_double_iterator)


SHADOW_INIT()
}


constexpr auto num_samples = 1000000ul;

void
test1()
{
    // prepare data
    std::vector<double> data_native;
    data_native.reserve(num_samples);
    std::random_device rd{};

    std::normal_distribution<double> d(4.0, 1.0);

    std::generate_n(std::back_inserter(data_native), num_samples, [&d, &rd]() {
        return d(rd);
    });


    const auto start_native = std::chrono::system_clock::now();
    const auto res_native = average(data_native.begin(), data_native.end());
    const auto end_native = std::chrono::system_clock::now();
    const auto dur_native = end_native - start_native;
    const double secs_native = static_cast<double>(dur_native.count()) /
                               std::chrono::system_clock::duration::period::den;

    std::cout << "call natively: " << secs_native
              << "s, result = " << res_native << '\n';


    // prepare reflected data
    auto data_refl = refl::static_make_object(data_native);

    // find reflected average function
    auto funs = refl::manager.free_functions();

    auto found_average =
        std::find_if(funs.first, funs.second, [](const auto& fi) {
            return refl::manager.free_function_name(fi) ==
                   std::string("average<vector_double_iterator>");
        });

    assert(found_average != funs.second);

    const auto average_tag = *found_average;

    // find member functions begin and end of std::vector<double>
    auto types = refl::manager.types();

    auto found_vector_double =
        std::find_if(types.first, types.second, [](const auto& ti) {
            return ti.name() == std::string("vector_double");
        });

    assert(found_vector_double != types.second);

    auto mem_funs =
        refl::manager.member_functions_by_class_type(*found_vector_double);

    std::unordered_map<std::string, shadow::member_function_tag> vd_mfs;

    std::for_each(mem_funs.first, mem_funs.second, [&vd_mfs](const auto& mft) {
        vd_mfs.insert(
            std::make_pair(refl::manager.member_function_name(mft), mft));
    });

    std::vector<shadow::object> args;
    args.push_back(
        refl::manager.call_member_function(data_refl, vd_mfs.at("begin")));
    args.push_back(
        refl::manager.call_member_function(data_refl, vd_mfs.at("end")));

    const auto start_refl = std::chrono::system_clock::now();
    auto res_refl =
        refl::manager.call_free_function(average_tag, args.begin(), args.end());
    const auto end_refl = std::chrono::system_clock::now();
    const auto dur_refl = end_refl - start_refl;
    const double secs_refl = static_cast<double>(dur_refl.count()) /
                             std::chrono::system_clock::duration::period::den;


    std::cout << "call through reflection: " << secs_refl
              << "s, result = " << res_refl << '\n';
}


void
test2()
{
    // prepare native data
    std::vector<double> data1_native;
    std::vector<double> data2_native;
    data1_native.reserve(num_samples);
    data2_native.reserve(num_samples);
    std::random_device rd{};

    std::normal_distribution<double> d(4.0, 1.0);

    std::generate_n(std::back_inserter(data1_native), num_samples, [&d, &rd]() {
        return d(rd);
    });

    std::generate_n(std::back_inserter(data2_native), num_samples, [&d, &rd]() {
        return d(rd);
    });

    std::vector<double> result_native;
    result_native.reserve(num_samples);

    const auto start_native = std::chrono::system_clock::now();
    std::transform(data1_native.begin(),
                   data1_native.end(),
                   data2_native.begin(),
                   std::back_inserter(result_native),
                   [](const double a, const double b) { return pluss(a, b); });
    const auto end_native = std::chrono::system_clock::now();
    const auto dur_native = end_native - start_native;
    const double secs_native = static_cast<double>(dur_native.count()) /
                               std::chrono::system_clock::duration::period::den;

    std::cout << "call natively: " << secs_native << "s\n";


    // prepare reflection data
    std::vector<shadow::object> data1_refl;
    std::vector<shadow::object> data2_refl;
    data1_refl.reserve(num_samples);
    data2_refl.reserve(num_samples);

    std::transform(data1_native.begin(),
                   data1_native.end(),
                   std::back_inserter(data1_refl),
                   [](const double a) { return refl::static_make_object(a); });

    std::transform(data2_native.begin(),
                   data2_native.end(),
                   std::back_inserter(data2_refl),
                   [](const double a) { return refl::static_make_object(a); });

    std::vector<shadow::object> result_refl;
    result_refl.reserve(num_samples);

    // find pluss function
    auto funs = refl::manager.free_functions();

    auto found_pluss =
        std::find_if(funs.first, funs.second, [](const auto& ft) {
            return refl::manager.free_function_name(ft) == std::string("pluss");
        });

    assert(found_pluss != funs.second);

    auto pluss_tag = *found_pluss;

    const auto start_refl = std::chrono::system_clock::now();
    std::transform(data1_refl.begin(),
                   data1_refl.end(),
                   data2_refl.begin(),
                   std::back_inserter(result_refl),
                   [&pluss_tag](const auto& obj_a, const auto& obj_b) {
                       std::vector<shadow::object> args{obj_a, obj_b};
                       return refl::manager.call_free_function(
                           pluss_tag, args.begin(), args.end());
                   });
    const auto end_refl = std::chrono::system_clock::now();
    const auto dur_refl = end_refl - start_refl;
    const double secs_refl = static_cast<double>(dur_refl.count()) /
                             std::chrono::system_clock::duration::period::den;

    std::cout << "call reflectively: " << secs_refl << "s\n";
}

int
main()
{
    test1();
    std::cout << "\n\n\n";
    test2();
}
