#include "api_types.hpp"
#include "reflection_manager.hpp"


namespace shadow
{
object::object() : value_(), type_info_(&void_info), manager_(nullptr)
{
}


object::object(any value, const type_info* ti, const reflection_manager* man)
    : value_(std::move(value)), type_info_(ti), manager_(man)
{
}


type_tag
object::type() const
{
    return type_tag(*type_info_);
}


constexpr const type_info object::void_info;


std::ostream&
operator<<(std::ostream& out, const object& obj)
{
    if(obj.manager_ == nullptr)
    {
        out << "void";
        return out;
    }


    return out;
}
}
