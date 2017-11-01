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


std::string
reflection_manager::type_name(const type_tag& tag) const
{
    return tag.name();
}

std::size_t
reflection_manager::type_size(const type_tag& tag) const
{
    return tag.size();
}

std::pair<typename reflection_manager::const_constructor_iterator,
          typename reflection_manager::const_constructor_iterator>
reflection_manager::constructors() const
{
    return std::make_pair(
        const_constructor_iterator(constructor_info_view_.cbegin()),
        const_constructor_iterator(constructor_info_view_.cend()));
}


std::pair<reflection_manager::const_indexed_constructor_iterator,
          reflection_manager::const_indexed_constructor_iterator>
reflection_manager::constructors_by_type(const type_tag& tag) const
{
    const auto index = index_of_type(tag);

    return std::make_pair(const_indexed_constructor_iterator(
                              0,
                              constructor_indices_by_type_[index].data(),
                              constructor_info_view_.data()),
                          const_indexed_constructor_iterator(
                              constructor_indices_by_type_[index].size(),
                              constructor_indices_by_type_[index].data(),
                              constructor_info_view_.data()));
}

type_tag
reflection_manager::constructor_type(const constructor_tag& tag) const
{
    return type_tag(type_info_view_[tag.info_ptr_->type_index]);
}

std::pair<reflection_manager::const_indexed_type_iterator,
          reflection_manager::const_indexed_type_iterator>
reflection_manager::constructor_parameter_types(
    const constructor_tag& tag) const
{
    auto index_buffer = tag.info_ptr_->parameter_type_indices;
    auto data_buffer = type_info_view_.data();

    return std::make_pair(
        const_indexed_type_iterator(0, index_buffer, data_buffer),
        const_indexed_type_iterator(
            tag.info_ptr_->num_parameters, index_buffer, data_buffer));
}

object
reflection_manager::construct_object(const constructor_tag& tag) const
{
    if(tag.info_ptr_->num_parameters != 0)
    {
        throw std::runtime_error("wrong number of arguments");
    }

    return object(tag.info_ptr_->bind_point(nullptr),
                  type_info_view_.data() + tag.info_ptr_->type_index,
                  this);
}

bool
reflection_manager::compare_type(const type_tag& tag, std::size_t index) const
{
    type_tag tag_from_index(type_info_view_[index]);

    return tag == tag_from_index;
}

std::size_t
reflection_manager::index_of_type(const type_tag& tag) const
{
    auto found = std::find_if(
        type_info_view_.cbegin(),
        type_info_view_.cend(),
        [&tag](const type_info& info) { return tag == type_tag(info); });

    if(found == type_info_view_.cend())
    {
        throw std::runtime_error("type not registered in reflection_manager");
    }

    return found - type_info_view_.cbegin();
}

std::size_t
reflection_manager::index_of_object(const object& obj) const
{
    if(obj.manager_ == this)
    {
        return obj.type_info_ - type_info_view_.data();
    }

    return index_of_type(obj.type());
}


std::pair<reflection_manager::const_conversion_iterator,
          reflection_manager::const_conversion_iterator>
reflection_manager::conversions() const
{
    return std::make_pair(
        const_conversion_iterator(conversion_info_view_.cbegin()),
        const_conversion_iterator(conversion_info_view_.cend()));
}

std::pair<reflection_manager::const_indexed_conversion_iterator,
          reflection_manager::const_indexed_conversion_iterator>
reflection_manager::conversions_by_from_type(const type_tag& tag) const
{
    const auto index = index_of_type(tag);

    return std::make_pair(const_indexed_conversion_iterator(
                              0,
                              conversion_indices_by_type_[index].data(),
                              conversion_info_view_.data()),
                          const_indexed_conversion_iterator(
                              conversion_indices_by_type_[index].size(),
                              conversion_indices_by_type_[index].data(),
                              conversion_info_view_.data()));
}

std::pair<type_tag, type_tag>
reflection_manager::conversion_types(const conversion_tag& tag) const
{
    return std::make_pair(
        type_tag(type_info_view_[tag.info_ptr_->from_type_index]),
        type_tag(type_info_view_[tag.info_ptr_->to_type_index]));
}

object
reflection_manager::convert(const conversion_tag& tag, const object& val) const
{
    if(val.type() != type_tag(type_info_view_[tag.info_ptr_->from_type_index]))
    {
        throw std::runtime_error(
            "type of object doesn't match conversion binding");
    }

    return object(tag.info_ptr_->bind_point(val.value_),
                  type_info_view_.data() + tag.info_ptr_->to_type_index,
                  this);
}


std::pair<reflection_manager::const_free_function_iterator,
          reflection_manager::const_free_function_iterator>
reflection_manager::free_functions() const
{
    return std::make_pair(
        const_free_function_iterator(free_function_info_view_.cbegin()),
        const_free_function_iterator(free_function_info_view_.cend()));
}

std::string
reflection_manager::free_function_name(const free_function_tag& tag) const
{
    return std::string(tag.info_ptr_->name);
}

type_tag
reflection_manager::free_function_return_type(
    const free_function_tag& tag) const
{
    return type_tag(type_info_view_[tag.info_ptr_->return_type_index]);
}


std::pair<reflection_manager::const_indexed_type_iterator,
          reflection_manager::const_indexed_type_iterator>
reflection_manager::free_function_parameter_types(
    const free_function_tag& tag) const
{
    return std::make_pair(
        const_indexed_type_iterator(
            0, tag.info_ptr_->parameter_type_indices, type_info_view_.data()),
        const_indexed_type_iterator(tag.info_ptr_->num_parameters,
                                    tag.info_ptr_->parameter_type_indices,
                                    type_info_view_.data()));
}

object
reflection_manager::call_free_function(const free_function_tag& tag) const
{
    if(tag.info_ptr_->num_parameters != 0)
    {
        throw std::runtime_error("wrong number of arguments");
    }

    return object(tag.info_ptr_->bind_point(nullptr),
                  type_info_view_.data() + tag.info_ptr_->return_type_index,
                  this);
}


std::pair<reflection_manager::const_member_function_iterator,
          reflection_manager::const_member_function_iterator>
reflection_manager::member_functions() const
{
    return std::make_pair(
        const_member_function_iterator(member_function_info_view_.cbegin()),
        const_member_function_iterator(member_function_info_view_.cend()));
}


std::pair<reflection_manager::const_indexed_member_function_iterator,
          reflection_manager::const_indexed_member_function_iterator>
reflection_manager::member_functions_by_class_type(const type_tag& tag) const
{
    const auto index = index_of_type(tag);

    return std::make_pair(const_indexed_member_function_iterator(
                              0,
                              member_function_indices_by_type_[index].data(),
                              member_function_info_view_.data()),
                          const_indexed_member_function_iterator(
                              member_function_indices_by_type_[index].size(),
                              member_function_indices_by_type_[index].data(),
                              member_function_info_view_.data()));
}


type_tag
reflection_manager::member_function_class_type(
    const member_function_tag& tag) const
{
    return type_tag(type_info_view_[tag.info_ptr_->object_type_index]);
}


std::string
reflection_manager::member_function_name(const member_function_tag& tag) const
{
    return std::string(tag.info_ptr_->name);
}


std::pair<reflection_manager::const_indexed_type_iterator,
          reflection_manager::const_indexed_type_iterator>
reflection_manager::member_function_parameter_types(
    const member_function_tag& tag) const
{
    return std::make_pair(
        const_indexed_type_iterator(
            0, tag.info_ptr_->parameter_type_indices, type_info_view_.data()),
        const_indexed_type_iterator(tag.info_ptr_->num_parameters,
                                    tag.info_ptr_->parameter_type_indices,
                                    type_info_view_.data()));
}


object
reflection_manager::call_member_function(object& obj,
                                         const member_function_tag& tag) const
{
    if(tag.info_ptr_->num_parameters != 0)
    {
        throw std::runtime_error("wrong number of arguments");
    }

    if(!check_member_class_type(obj, *tag.info_ptr_))
    {
        throw std::runtime_error("wrong class type for member function");
    }

    return object(tag.info_ptr_->bind_point(obj.value_, nullptr),
                  type_info_view_.data() + tag.info_ptr_->return_type_index,
                  this);
}


std::pair<reflection_manager::const_member_variable_iterator,
          reflection_manager::const_member_variable_iterator>
reflection_manager::member_variables() const
{
    return std::make_pair(
        const_member_variable_iterator(member_variable_info_view_.cbegin()),
        const_member_variable_iterator(member_variable_info_view_.cend()));
}


std::pair<reflection_manager::const_indexed_member_variable_iterator,
          reflection_manager::const_indexed_member_variable_iterator>
reflection_manager::member_variables_by_class_type(const type_tag& tag) const
{
    const auto index = index_of_type(tag);

    return std::make_pair(const_indexed_member_variable_iterator(
                              0,
                              member_variable_indices_by_type_[index].data(),
                              member_variable_info_view_.data()),
                          const_indexed_member_variable_iterator(
                              member_variable_indices_by_type_[index].size(),
                              member_variable_indices_by_type_[index].data(),
                              member_variable_info_view_.data()));
}


type_tag
reflection_manager::member_variable_type(const member_variable_tag& tag) const
{
    return type_tag(type_info_view_[tag.info_ptr_->type_index]);
}


type_tag
reflection_manager::member_variable_class_type(
    const member_variable_tag& tag) const
{
    return type_tag(type_info_view_[tag.info_ptr_->object_type_index]);
}


std::string
reflection_manager::member_variable_name(const member_variable_tag& tag) const
{
    return std::string(tag.info_ptr_->name);
}


void
reflection_manager::set_member_variable(object& obj,
                                        const member_variable_tag& tag,
                                        const object& val) const
{
    if(val.type() != member_variable_type(tag))
    {
        throw std::runtime_error(
            "attempting to set member variable of wrong type");
    }

    if(obj.type() != member_variable_class_type(tag))
    {
        throw std::runtime_error(
            "attempting to set member variable belonging to wrong class");
    }

    tag.info_ptr_->set_bind_point(obj.value_, val.value_);
}


object
reflection_manager::get_member_variable(const object& obj,
                                        const member_variable_tag& tag) const
{
    if(obj.type() != member_variable_class_type(tag))
    {
        throw std::runtime_error(
            "attempting to set member variable belonging to wrong class");
    }

    return object(tag.info_ptr_->get_bind_point(obj.value_),
                  type_info_view_.data() + tag.info_ptr_->type_index,
                  this);
}
}
