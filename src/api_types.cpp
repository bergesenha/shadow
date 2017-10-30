#include <limits>

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
        return out;
    }

    // find default serialization_info for obj
    const auto t_index = obj.manager_->index_of_object(obj);

    auto found =
        std::find_if(obj.manager_->serialization_info_view_.cbegin(),
                     obj.manager_->serialization_info_view_.cend(),
                     [t_index](const auto& si) {
                         return si.type_index == t_index &&
                                std::string(si.name) == std::string("default");
                     });
    if(found != obj.manager_->serialization_info_view_.cend())
    {
        return found->serialization_bind_point(out, obj.value_);
    }

    out << '{';

    // if not, find member variables
    auto mv_pair = obj.manager_->member_variables_by_class_type(obj.type());

    if(std::distance(mv_pair.first, mv_pair.second) < 2)
    {
        std::for_each(
            mv_pair.first, mv_pair.second, [&obj, &out](const auto& mv) {
                out << obj.manager_->get_member_variable(obj, mv);
            });

        return out;
    }

    out << obj.manager_->get_member_variable(obj, *mv_pair.first);
    ++mv_pair.first;

    std::for_each(mv_pair.first, mv_pair.second, [&out, &obj](const auto& mv) {
        out << ", " << obj.manager_->get_member_variable(obj, mv);
    });

    out << '}';

    return out;
}

std::istream&
operator>>(std::istream& in, object& obj)
{
    if(obj.manager_ == nullptr)
    {
        return in;
    }

    // find default serialization_info for obj
    const auto t_index = obj.manager_->index_of_object(obj);

    auto found =
        std::find_if(obj.manager_->serialization_info_view_.cbegin(),
                     obj.manager_->serialization_info_view_.cend(),
                     [t_index](const auto& si) {
                         return si.type_index == t_index &&
                                std::string(si.name) == std::string("default");
                     });
    if(found != obj.manager_->serialization_info_view_.cend())
    {
        return found->deserialization_bind_point(in, obj.value_);
    }

    return in;
}
}
