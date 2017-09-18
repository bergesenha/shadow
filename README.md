# SHADOW

## Warning
Shadow reflection library is in early development. The API is probably going to
change and the library is not feature complete.


## Usage

### Registering Information and Setup
Before you can use any of your own types, members, or functions through the
reflection system all the relevant information must be registered. This is done
using macros at namespace scope, typically in your own namespace within the
global namespace scope.

#### Registering Types
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

#### Registering Constructors
In order to have the ability to construct and instantiate objects of a given
type through the reflection system, the desired constructors of that type must
be registered. This is done with the REGISTER_CONSTRUCTOR(type_name,
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

#### Registering Free Functions
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

#### Registering Member Functions
Similarly to registering free functions, member functions are registered with
REGISTER_MEMBER_FUNCTION(object_type, member_function_name) and
REGISTER_MEMBER_FUNCTION_EXPLICIT(object_type, member_function_name,
return_type, param_type1, param_type2, ...).

For example, if foo_memfun is a member function of the type foo with no
overloads and bar_memfun of the type bar and has two overloads both with 'return
type' void, one taking an int and one taking char and double:

```c++
namespace my_space
{
// type registration as above

REGISTER_MEMBER_FUNCTION_BEGIN()

REGISTER_MEMBER_FUNCTION(foo, foo_memfun)

REGISTER_MEMBER_FUNCTION_EXPLICIT(bar, bar_memfun, void, int)
REGISTER_MEMBER_FUNCTION_EXPLICIT(bar, bar_memfun, void, char, double)

REGISTER_MEMBER_FUNCTION_END()

}
```

#### Registering Member Variables
Any public member variable may be registered (provided that the type of the
member variable is registered) using the macro
REGISTER_MEMBER_VARIABLE(object_type, mem_var_name) enclosed within
REGISTER_MEMBER_VARIABLE_BEGIN and REGISTER_MEMBER_VARIABLE_END macros. 

For example, if baz has a public member variable baz_data1:

```c++
namespace my_space
{
// type registration as above

REGISTER_MEMBER_VARIABLE_BEGIN()

REGISTER_MEMBER_VARIABLE(baz, baz_data1)

REGISTER_MEMBER_VARIABLE_END()
}
```

#### Finalizing Registration and Initializing Shadow
When all desired information is registered, the macro SHADOW_INIT() initializes
the library for use. SHADOW_INIT() declares a global variable named `manager` of
type `shadow::reflection_manager` within the namespace that information was
registered. The `shadow::reflection_manager` is the main point of interaction with
the reflection system.

Example combining the above:

```c++
namespace my_space
{
// register types
REGISTER_TYPE_BEGIN()

REGISTER_TYPE(foo_type)
REGISTER_TYPE(bar_type)

REGISTER_TYPE_END()

// register constructors
REGISTER_CONSTRUCTOR_BEGIN()

REGISTER_CONSTRUCTOR(foo_type)
REGISTER_CONSTRUCTOR(bar_type)
REGISTER_CONSTRUCTOR(bar_type, int, double)
REGISTER_CONSTRUCTOR(baz_type, char)

REGISTER_CONSTRUCTOR_END()

// register free functions
REGISTER_FREE_FUNCTION_BEGIN()

REGISTER_FREE_FUNCTION(foo_function)

REGISTER_FREE_FUNCTION_EXPLICIT(bar_function, int, double)
REGISTER_FREE_FUNCTION_EXPLICIT(bar_function, int, foo_type)

REGISTER_FREE_FUNCTION_END()

// register member functions
REGISTER_MEMBER_FUNCTION_BEGIN()

REGISTER_MEMBER_FUNCTION(foo, foo_memfun)

REGISTER_MEMBER_FUNCTION_EXPLICIT(bar, bar_memfun, void, int)
REGISTER_MEMBER_FUNCTION_EXPLICIT(bar, bar_memfun, void, char, double)

REGISTER_MEMBER_FUNCTION_END()

// register member variables
REGISTER_MEMBER_VARIABLE_BEGIN()

REGISTER_MEMBER_VARIABLE(baz, baz_data1)

REGISTER_MEMBER_VARIABLE_END()

SHADOW_INIT()
}
```

### Querying the Reflection System
For the following examples it is assumed that the registration and
initialization was done in the namespace 'my_space'.

#### Querying for Types
To query for all available types in the reflection system:
```c++
auto types_pair = my_space::manager.types();
```

The `types` member function returns a `std::pair` of iterators to a
`shadow::reflection_manager::type`, which can be queried for the name of the
type, the size in bytes of the type, and can be compared for equality:
```c++
auto first_type = *types_pair.first;

std::string name_of_first_type = first_type.name();
std::size_t size_of_first_type = first_type.size();
```

#### Querying for Constructors of a Type
To query for the constructors available for a type:
```c++
auto constructors_pair = my_space::manager.constructors_by_type(first_type);
```

The `constructors_by_type` member function returns a `std::pair` of iterators to
`shadow::reflection_manager::constructor`, that can be queried for the number of
parameters, the types of the parameters, and of course the type that it
constructs:
```c++
auto first_constructor = *constructors_pair.first;

std::size_t num_parameters = first_constructor.num_parameters();
auto parameters_pair = first_constructor.parameter_types();

assert(first_constructor.get_type() == first_type);
```

#### Constructing Objects and Variables
Shadow uses the type `shadow::variable` as a homogenous vessel to hold an object
of any type that is registered in the reflection system. It is similar in
concept to `std::any` in that it uses type erasure to be able to contain
values of arbitrary types. In addition, an instance of `shadow::variable`
contains metadata about the type of the value held within enabling us to
interact with the value at runtime without having to know the type at compile
time.

To construct an object of a type that is known at compile time, a function
template `template <class T, class... Args> shadow::variable
static_create(Args&&... args)` is available in the namespace that registration and
initialization was done:
```c++
shadow::variable a_foo_type = my_space::static_create<foo_type>();
shadow::variable a_bar_type = my_space::static_create<bar_type>(23, 55.234);
```
`static_create` forwards the arguments to the constructor of the type and looks
up the correct metadata about the type at compile time.

To construct an object of a type decided at runtime, we can use the member
function `shadow::reflection_manager::construct`. `construct` takes a
`shadow::reflection_manager::constructor` as its first argument, and two
iterators pointing to a range of `shadow::variable` to be used as arguments to
the constructor. `construct` is templated on the iterators so we are free to use
any container to hold the arguments.

Assuming we have correctly queried and identified the constructor for bar_type
taking int and double as arguments:
```c++
// create a container of arguments
std::vector<shadow::variable> args =
{ my_space::static_create<int>(23),
  my_space::static_create<double>(55.234) };

shadow::variable a_bar_type = my_space::manager.construct(bar_type_constructor,
                                                          args.begin(),
                                                          args.end());

```
