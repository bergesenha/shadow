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

template <class InfoType>
class info_type_aggregate
{
public:
    info_type_aggregate() = default;

    info_type_aggregate(const InfoType& info) : info_ptr_(&info)
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
};




typedef info_type_aggregate<type_info> type_id;
typedef info_type_aggregate<type_description> instance_type_id;
typedef info_type_aggregate<constructor_info> constructor_id;
typedef info_type_aggregate<conversion_info> conversion_id;
typedef info_type_aggregate<free_function_info> free_function_id;
typedef info_type_aggregate<member_function_info> member_function_id;
typedef info_type_aggregate<member_variable_info> member_variable_id;
}
