# SHADOW REFLECTION LIBRARY

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
#include <shadow.hpp>


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
may be registered in any order you wish.

### Interacting with the Reflection System
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

If the types of the values held in the arguments or the number of arguments
do not exactly match the signature of the constructor, a shadow::argument_error
is thrown as an exception.

#### Querying and Getting/Setting Member Variables
As with constructors, member variables are represented by class that acts as an
identifier/tag for interacting with the value and as an information interface
for querying information about the member variable.

Member variables can be queried from the global instance of
`shadow::reflection_manager` (ie. `my_space::manager`) or directly from a
`shadow::variable`:
```c++
// a_baz_type is a shadow::variable holding an instance of a baz_type
auto baz_member_variables = a_baz_type.member_variables();

// baz_type_tag is a shadow::reflection_manager::type identifying baz_type
auto baz_member_variables2 = my_space::manager.member_variables_by_type(baz_type_tag);
```

Both member functions return a `std::pair` of iterators to
`shadow::reflection_manager::member_variable or
shadow::variable::member_variable`, which can be queried for information:
```c++
auto first_member_variable = *baz_member_variables.first;

// name of the member variable
std::string mem_var_name = first_member_variable.name();

// type of the object owning the member variable
auto mem_var_object_type = first_member_variable.object_type();
assert(mem_var_object_type == baz_type_tag);

// the type of the member variable
auto mem_var_type = first_member_variable.get_type();
```

The member variable of an object held by a `shadow::variable` can be get and
set with the member variable identifier:
```c++
shadow::variable baz_mem_var_value =
a_baz_type.get_member_variable(first_member_variable);

a_baz_type.set_member_variable(first_member_variable,
                               my_space::static_create<char>('a'));
```

#### Querying for and Calling Member Functions
As with member variables, member functions available can be queried from the
global instance of `shadow::reflection_manager` or directly from a
`shadow::variable`:
```c+++
auto baz_member_functions = a_baz_type.member_functions();

auto baz_member_functions2 =
    my_space::manager.member_functions_by_type(baz_type_tag);
```

As with member variables, we get a `std::pair` of iterators to
`shadow::reflection_manager::member_function or
shadow::variable::member_function` information objects. To query for information
about a member function:
```c++
auto first_member_function = *baz_member_functions.first;

std::string mem_fun_name = first_member_function.name();
std::size_t num_params = first_member_function.num_parameters();
auto param_types_pair = first_member_function.parameter_types();
shadow::reflection_manager::type return_type =
    first_member_function.return_type();
shadow::reflection_manager::type object_type =
    first_member_function.object_type();
```

A member function is called from the `shadow::variable` holding the object to
call the member function from:
```c++
// args is some container of shadow::variable as arguments
shadow::variable return_value = a_baz_type.call_member_function(
                        first_member_function,
                        std::begin(args),
                        std::end(args));
```

If the arguments given do not match the signature of the underlying member
function, a shadow::argument_error exception will be thrown.

#### Querying for and Calling Free Functions
To query for all free functions registered in the reflection system:
```c++
auto free_functions_pair = my_space::manager.free_functions();

shadow::reflection_manager::free_function first_free_function =
    *free_functions_pair.first;

std::string function_name = first_free_function.name();
std::size_t num_params = first_free_function.num_parameters();
auto parameter_types_pair = first_free_function.parameter_types();
shadow::reflection_manager::type return_type =
    first_free_function.return_type();
```

Free functions are called directly from the
shadow::reflection_manager::free_function information type via the function call
operator as it is not owned by any type.
```c++
// args is some container of shadow::variable
shadow::variable return_value = first_free_function(std::begin(args),
                                                    std::end(args));
```
