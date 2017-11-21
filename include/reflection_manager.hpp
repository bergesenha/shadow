#pragma once

#include <array_view.hpp>
#include "reflection_info.hpp"


namespace shadow
{

template <class T>
struct array_specializer;

template <class T>
struct array_specializer<T*>
{
    static helene::array_view<T>
    initialize(T*)
    {
        return helene::array_view<T>();
    }
};

template <class T, std::size_t N>
struct array_specializer<T[N]>
{
    static helene::array_view<T> initialize(T (&arr)[N])
    {
        return helene::make_array_view(arr);
    }
};

class reflection_manager
{
public:
    reflection_manager() = default;

    template <class TypeInfoArray>
    reflection_manager(TypeInfoArray& ti_arr)
        : type_info_view_(array_specializer<TypeInfoArray>::initialize(ti_arr))
    {
    }


public:
    auto types() const
    {
        return std::make_pair(type_info_view_.cbegin(), type_info_view_.cend());
    }

private:
    helene::array_view<const type_info> type_info_view_;
};
}
