#include <iostream>
#include <string>
#include <vector>

#include <algorithm>
#include <shadow.hpp>
#include <array_view.hpp>

namespace myspace
{
REGISTER_TYPE_BEGIN()

REGISTER_TYPE_END()


SHADOW_INIT()
} // namespace myspace


template <class T>
struct array_pointer_selector
{
    static helene::array_view<std::remove_pointer_t<T>> generate_view(T)
    {
        return helene::array_view<std::remove_pointer_t<T>>();
    }
};

template <class I, std::size_t N>
struct array_pointer_selector<I[N]>
{
    static helene::array_view<I> generate_view(I (&arr)[N])
    {
        return helene::array_view<I>(arr);
    }
};


class holds_array_view
{
public:
    template <class TypeInfoArrayType, class ConstructorInfoArrayType>
    holds_array_view(TypeInfoArrayType& ti_arr,
                     ConstructorInfoArrayType& ci_arr)
        : view_(
              array_pointer_selector<TypeInfoArrayType>::generate_view(ti_arr)),
          ci_view_(
              array_pointer_selector<ConstructorInfoArrayType>::generate_view(
                  ci_arr))
    {
    }

private:
    helene::array_view<const shadow::type_info> view_;
    helene::array_view<const shadow::constructor_info> ci_view_;
};


int
main()
{
    std::cout << "\n\n";
    holds_array_view a{myspace::type_info_array_holder::value,
                       myspace::constructor_info_array_holder::value};
}
