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
}


template <class T>
void p(T)
{
    puts(__PRETTY_FUNCTION__);
}


class director
{
public:
    template <class NameHolder, class TypeInfoHolder>
    director(NameHolder, TypeInfoHolder)
        : names_(std::begin(NameHolder::value), std::end(NameHolder::value)),
          type_infos_(std::begin(TypeInfoHolder::value),
                      std::end(TypeInfoHolder::value))
    {
    }

    std::vector<std::string>::const_iterator
    names_begin() const
    {
        return names_.cbegin();
    }

    std::vector<std::string>::const_iterator
    names_end() const
    {
        return names_.cend();
    }

private:
    std::vector<std::string> names_;
    std::vector<shadow::type_info> type_infos_;
};

static const director thedir((myspace::type_name_array_holder()),
                             myspace::type_info_array_holder());

int
main()
{
    for(auto i = thedir.names_begin(); i != thedir.names_end(); ++i)
    {
        std::cout << *i << '\n';
    }
}
