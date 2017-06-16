#pragma once

#include <vector>
#include <string>

#include "reflection_info.hpp"


namespace shadow
{
class director
{
public:
    template <class TypeNameHolder,
              class TypeInfoHolder,
              class ConstructorInfoHolder>
    director(TypeNameHolder, TypeInfoHolder, ConstructorInfoHolder)
        : names_(std::begin(TypeNameHolder::value),
                 std::end(TypeNameHolder::value)),
          type_infos_(std::begin(TypeInfoHolder::value),
                      std::end(TypeInfoHolder::value)),
          constructor_infos_(std::begin(ConstructorInfoHolder::value),
                             std::end(ConstructorInfoHolder::value))
    {
    }

private:
    std::vector<std::string> names_;
    std::vector<shadow::type_info> type_infos_;
    std::vector<shadow::constructor_info> constructor_infos_;
};
}
