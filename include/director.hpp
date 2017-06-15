#pragma once

#include <vector>
#include <string>

#include "reflection_info.hpp"


namespace shadow
{
class director
{
public:
    template <class TypeNameHolder, class TypeInfoHolder>
    director(TypeNameHolder, TypeInfoHolder)
        : names_(std::begin(TypeNameHolder::value),
                 std::end(TypeNameHolder::value)),
          type_infos_(std::begin(TypeInfoHolder::value),
                      std::end(TypeInfoHolder::value))
    {
    }

private:
    std::vector<std::string> names_;
    std::vector<shadow::type_info> type_infos_;
};
}
