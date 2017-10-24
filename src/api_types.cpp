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
    // check for empty condition
    if(obj.manager_ == nullptr)
    {
        out << "void";
        return out;
    }

    // find string serializer
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

    out << '{';

    // if no string serializer, recurse through all member variables
    auto mem_vars = obj.manager_->member_variables_by_class_type(obj.type());

    if(std::distance(mem_vars.first, mem_vars.second) <= 1)
    {
        std::for_each(
            mem_vars.first, mem_vars.second, [&obj, &out](const auto& mv) {
                out << obj.manager_->get_member_variable(obj, mv);
            });

        out << '}';
        return out;
    }

    out << obj.manager_->get_member_variable(obj, *mem_vars.first);
    ++mem_vars.first;

    std::for_each(
        mem_vars.first, mem_vars.second, [&obj, &out](const auto& mv) {
            out << ", ";
            out << obj.manager_->get_member_variable(obj, mv);
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

    const auto index = obj.manager_->index_of_object(obj);
    const auto& deserializers = obj.manager_->string_serialization_info_view_;

    auto found = std::find_if(
        deserializers.cbegin(), deserializers.cend(), [index](const auto& ssi) {
            return ssi.type_index == index;
        });

    if(found != deserializers.cend())
    {
        std::string str_val;
        in >> str_val;
        obj.value_ = found->deserialize_bind_point(str_val);

        return in;
    }

    auto mem_vars = obj.manager_->member_variables_by_class_type(obj.type());

    in.ignore(std::numeric_limits<std::streamsize>::max(), '{');

    std::for_each(mem_vars.first, mem_vars.second, [&obj, &in](const auto& mv) {
        auto mem_val = obj.manager_->get_member_variable(obj, mv);
        in >> mem_val;
        obj.manager_->set_member_variable(obj, mv, mem_val);
    });

    in.ignore(std::numeric_limits<std::streamsize>::max(), '}');

    return in;
}
}
