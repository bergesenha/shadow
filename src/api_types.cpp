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

    const auto index = obj.manager_->index_of_object(obj);
    const auto& serializers = obj.manager_->string_serialization_info_view_;

    auto found = std::find_if(
        serializers.cbegin(), serializers.cend(), [index](const auto& ssi) {
            return ssi.type_index == index;
        });

    if(found != serializers.cend())
    {
        out << found->serialize_bind_point(obj.value_);
        return out;
    }


    return out;
}
}
