#include "reflection_manager.hpp"


namespace shadow
{
std::pair<typename reflection_manager::const_type_iterator,
          typename reflection_manager::const_type_iterator>
reflection_manager::types() const
{
    return std::make_pair(const_type_iterator(type_info_view_.cbegin()),
                          const_type_iterator(type_info_view_.cend()));
}
}
