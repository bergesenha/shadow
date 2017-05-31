#pragma once

#include <utility>


namespace shadow
{
namespace t_list
{
template <class...>
struct type_list
{
};


template <class TypeList>
struct integer_sequence_from_type_list;

template <class... Types>
struct integer_sequence_from_type_list<type_list<Types...>>
{
    typedef std::index_sequence_for<Types...> type;
};

template <class TypeList>
using integer_sequence_from_type_list_t =
    typename integer_sequence_from_type_list<TypeList>::type;
}
}
