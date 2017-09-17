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

Registering of types needs to come before all other, however once types are
registered, constructors, free functions, member functions and member variables
may be registered in any order you wish. For example, you may register all
member variables before registering all the constructors.

### Registering Constructors
In order to have the ability to construct and instantiate objects of a given
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

### Registering Free Functions
Registering free functions in the reflection system is done with the
REGISTER_FREE_FUNCTION and REGISTER_FREE_FUNCTION_EXPLICIT macros enclosed
within REGISTER_FREE_FUNCTION_BEGIN and REGISTER_FREE_FUNCTION_END macros.

REGISTER_FREE_FUNCTION(function_name) is used for functions where there is no
ambiguity in what the function_name refers to, in general when there are no
overloads of the function and so the reflection system can infer the signature
of the function.

REGISTER_FREE_FUNCTION_EXPLICIT(function_name, return_type, param_type1,
param_type2, ...) is used when there exists an overload set for the function
and so the reflection system needs to be told the exact signature.

For example, if foo_function refers to a simple function with no overloads and
bar_function refers to two overloads returning int, one has a double parameter
and one has a foo_type parameter:

```c++
namespace my_space
{
// type registration as above


REGISTER_FREE_FUNCTION_BEGIN()

REGISTER_FREE_FUNCTION(foo_function)

REGISTER_FREE_FUNCTION_EXPLICIT(bar_function, int, double)
REGISTER_FREE_FUNCTION_EXPLICIT(bar_function, int, foo_type)

REGISTER_FREE_FUNCTION_END()

}
```

## Building
