#pragma once


#include <string>
#include <cstddef>
#include <cstring>
#include <ostream>
#include <istream>

#include <any.hpp>
#include <reflection_info.hpp>


namespace shadow
{

class reflection_manager;

template <class InfoType>
class info_type_aggregate
{
    friend class reflection_manager;

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

private:
    const InfoType* info_ptr_;
    const reflection_manager* manager_;
};


typedef info_type_aggregate<type_description> type_id;
typedef info_type_aggregate<type_description> instance_type_id;
typedef info_type_aggregate<constructor_info> constructor_id;
typedef info_type_aggregate<conversion_info> conversion_id;
typedef info_type_aggregate<free_function_info> free_function_id;
typedef info_type_aggregate<member_function_info> member_function_id;
typedef info_type_aggregate<member_variable_info> member_variable_id;


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
