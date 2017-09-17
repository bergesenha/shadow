# SHADOW

## Warning
Shadow reflection library is in early development. The API is probably going to
change and the library is not feature complete.


## Use
Before you can use any of your own types, members, or functions through the
reflection system all the relevant information must be registered. This is done
using macros at namespace scope, typically in your own namespace within the
global namespace scope.

### Registering Types
Before registering anything else you need to register all the custom types that
you intend to have access to through the reflection system. This is done with
the REGISTER_TYPE(type_name) successively for all your types enclosed with
REGISTER_TYPE_BEGIN() and REGISTER_TYPE_END(). You can register types in
arbitrary order, even if a type requires a declaration of another type.

```c++
namespace my_space
{

REGISTER_TYPE_BEGIN()


REGISTER_TYPE(foo_type)
REGISTER_TYPE(bar_type)

REGISTER_TYPE(baz_type)

REGISTER_TYPE_END()

}
```

Registering of fundamental types is already handled and is done within
the REGISTER_TYPE_END macro, so there is no need to register ints, longs or
doubles etc...

### Registering Constructors
To order to have the ability to construct and instantiate objects of a given
type through the reflection system, the desired constructors of that type must
also be registered. This is done with the REGISTER_CONSTRUCTOR(type_name,
param_type1, param_type2, ...) macro. Similarly to the REGISTER_TYPE macros,
REGISTER_CONSTRUCTOR macro calls must be enclosed within
REGISTER_CONSTRUCTOR_BEGIN and REGISTER_CONSTRUCTOR_END macros.

For example, if foo_type only has a default constructor, while bar_type has both
a default constructor and a constructor taking one int and one double, and
baz_type only has a constructor taking a char:

```c++
namespace my_space
{
// REGISTER_TYPE... from above


REGISTER_CONSTRUCTOR_BEGIN()

REGISTER_CONSTRUCTOR(foo_type)
REGISTER_CONSTRUCTOR(bar_type)
REGISTER_CONSTRUCTOR(bar_type, int, double)
REGISTER_CONSTRUCTOR(baz_type, char)

REGISTER_CONSTRUCTOR_END()

}
```

## Building
