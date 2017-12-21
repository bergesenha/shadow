#pragma once


#include <string>
#include <cstddef>
#include <cstring>
#include <ostream>
#include <istream>
#include <sstream>
#include <algorithm>

#include "any.hpp"
#include "reflection_info.hpp"
#include "info_iterators.hpp"


namespace shadow
{

class reflection_manager;

// policy aggregator for information/id types
template <class InfoType, template <class> class... Policies>
class info_type_aggregate
    : public Policies<info_type_aggregate<InfoType, Policies...>>...
{
    friend class reflection_manager;

    template <class Derived>
    friend struct type_name_policy;

    template <class Derived>
    friend struct name_policy;

    template <class Derived>
    friend struct return_type_policy;

    template <class Derived>
    friend struct type_policy;

    template <class Derived>
    friend struct parameter_types_policy;

    template <class Derived>
    friend struct object_type_policy;

    template <class Derived>
    friend struct member_function_is_const_policy;

    template <class Derived>
    friend struct offset_policy;

    template <class Derived>
    friend struct equal_by_name_policy;

public:
    info_type_aggregate() = default;

    info_type_aggregate(const InfoType& info, const reflection_manager& man)
        : info_ptr_(&info), manager_(&man)
    {
    }

    info_type_aggregate* operator->()
    {
        return this;
    }

    const info_type_aggregate* operator->() const
    {
        return this;
    }

protected:
    const InfoType* info_ptr_;
    const reflection_manager* manager_;
};


// type_name_policy: implements the member function name, building a string from
// a type's name and qualifiers and attributes
template <class Derived>
struct type_name_policy
{
    std::string
    name() const
    {
        std::ostringstream out;
        out << static_cast<const Derived*>(this)
                   ->manager_
                   ->type_info_view_[static_cast<const Derived*>(this)
                                         ->info_ptr_->type_index]
                   .name;

        auto att_begin =
            static_cast<const Derived*>(this)->info_ptr_->attributes;
        auto att_end =
            static_cast<const Derived*>(this)->info_ptr_->attributes +
            static_cast<const Derived*>(this)->info_ptr_->num_attributes;

        std::for_each(
            att_begin, att_end, [&out](auto att) { out << ' ' << att; });

        return out.str();
    }
};


// name_policy: implements the member function name, building a string from the
// pointer to char array of an information struct (typically instantiated at
// compile time)
template <class Derived>
struct name_policy
{
    std::string
    name() const
    {
        return static_cast<const Derived*>(this)->info_ptr_->name;
    }
};


// equal_by_name_policy: implements operator== based on string matching of the
// name
template <class Derived>
struct equal_by_name_policy
{
    bool
    operator==(const Derived& other) const
    {
        const Derived& derived = *static_cast<const Derived*>(this);

        return derived.name() == other.name();
    }
};


// type_id: aggregate information type representing a type
typedef info_type_aggregate<type_description,
                            type_name_policy,
                            equal_by_name_policy>
    type_id;

typedef info_iterator_<const type_description, type_id> type_id_iterator;


// return_type_policy: implements the member function return_type, returning a
// type_id of the return type of a function-like invocation
template <class Derived>
struct return_type_policy
{
    type_id
    return_type() const
    {
        return type_id(
            *(static_cast<const Derived*>(this)->info_ptr_->return_type),
            *(static_cast<const Derived*>(this)->manager_));
    }
};

template <class Derived>
struct type_policy
{
    type_id
    type() const
    {
        return type_id(*(static_cast<const Derived*>(this)->info_ptr_->type),
                       *(static_cast<const Derived*>(this)->manager_));
    }
};

template <class Derived>
struct parameter_types_policy
{
    std::pair<type_id_iterator, type_id_iterator>
    parameter_types() const
    {
        const Derived* derived_this = static_cast<const Derived*>(this);

        return std::make_pair(
            type_id_iterator(derived_this->info_ptr_->parameter_types,
                             derived_this->manager_),
            type_id_iterator(derived_this->info_ptr_->parameter_types +
                                 derived_this->info_ptr_->num_parameters,
                             derived_this->manager_));
    }
};

template <class Derived>
struct object_type_policy
{
    type_id
    object_type() const
    {
        return type_id(
            *static_cast<const Derived*>(this)->info_ptr_->object_type,
            *static_cast<const Derived*>(this)->manager_);
    }
};

template <class Derived>
struct member_function_is_const_policy
{
    bool
    is_const() const
    {
        return static_cast<const Derived*>(this)->info_ptr_->constness;
    }
};

template <class Derived>
struct offset_policy
{
    std::size_t
    offset() const
    {
        return static_cast<const Derived*>(this)->info_ptr_->offset;
    }
};

typedef info_type_aggregate<constructor_info,
                            type_policy,
                            parameter_types_policy>
    constructor_id;

typedef info_type_aggregate<conversion_info> conversion_id;

typedef info_type_aggregate<free_function_info,
                            name_policy,
                            return_type_policy,
                            parameter_types_policy>
    free_function_id;
typedef info_type_aggregate<member_function_info,
                            name_policy,
                            return_type_policy,
                            parameter_types_policy,
                            object_type_policy,
                            member_function_is_const_policy>
    member_function_id;
typedef info_type_aggregate<member_variable_info,
                            name_policy,
                            type_policy,
                            object_type_policy,
                            offset_policy,
                            equal_by_name_policy>
    member_variable_id;


class object
{
    friend class reflection_manager;

    object(const any& value, const type_description* type);

public:
    object() = default;

    bool has_value() const;

private:
    any value_;
    const type_description* type_;
};
}


namespace shadow
{
inline object::object(const any& value, const type_description* type)
    : value_(value), type_(type)
{
}

inline bool
object::has_value() const
{
    return value_.has_value();
}
}
