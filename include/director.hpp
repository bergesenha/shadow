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
              class ConstructorInfoHolder,
              class ConversionInfoHolder>
    director(TypeNameHolder,
             TypeInfoHolder,
             ConstructorInfoHolder,
             ConversionInfoHolder)
        : names_(std::begin(TypeNameHolder::value),
                 std::end(TypeNameHolder::value)),
          type_infos_(std::begin(TypeInfoHolder::value),
                      std::end(TypeInfoHolder::value)),
          constructor_infos_(std::begin(ConstructorInfoHolder::value),
                             std::end(ConstructorInfoHolder::value)),
          conversion_infos_(std::begin(ConversionInfoHolder::value),
                            std::end(ConversionInfoHolder::value))
    {
    }

private:
    std::vector<std::string> names_;
    std::vector<type_info> type_infos_;
    std::vector<constructor_info> constructor_infos_;
    std::vector<conversion_info> conversion_infos_;
};
}
