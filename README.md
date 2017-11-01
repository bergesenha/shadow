## Warning
Shadow reflection library is in early development. The API is probably going to
change and the library is not feature complete.


# SHADOW REFLECTION LIBRARY
## Motivation
Usage of Shadow is centered around registering types, functions, member
variables, member functions, and constructors nonintrusively to the reflection
system. This means you can bring only the parts you want from existing code into
a reflection system that lets you query, call, construct, serialize, deserialize
etc... at runtime.

Shadow was originally intended as a means to support plugin infrastructures,
where one could dynamically load objects and functions of unknown types from
shared binary modules and interact with them in a meaningful way.


## Cloning and Compiling
Shadow has been tested on Linux with clang and gcc. Currently, Shadow does not
build with msvc on Windows, however, using Visual Studio with clang should work.

Make sure to git clone with the --recursive option to pull in the libraries
'helene' and 'metamusil' as submodules. Alternatively, clone in the normal way
and the git submodule init and submodule update for each dependency.

Shadow uses CMake as a build system. The root CMakeLists.txt specifies the
library target 'shadow'. In your projects CMakeLists.txt, simply add:
```cmake
add_subdirectory(path/to/shadow_root)

target_link_libraries(your_target shadow)
```

This also sets up the required include directories for you, so no need to
explicitly target_include_directories(your_target etc...).

It also respects the cmake variable BUILD_SHARED_LIBS, so the compiled part of
Shadow will be built as a shared library if this is ON.

## Registering
Before anything else you need to register the parts of your existing code that
you wish to interact with through the reflection system and initialize the
library. Registering is done with macros. It is recommended to do the
registering within a namespace other than the global as initialization
instantiates a global `shadow::reflection_manager` as well as defining some
utility functions that depend on compile time lookup of information.

The simplest working example possible is
```c++
#include <shadow.hpp>

namespace myspace
{
REGISTER_TYPE_BEGIN()
REGISTER_TYPE_END()


SHADOW_INIT()
}

int main()
{
}
```

Even though nothing is explicitly registered, shadow registers the fundamental
types as well as `std::string`.

To register your own types, use the REGISTER_TYPE(type_name) macro:
```c++
#include <shadow.hpp>
#include <string>

struct mystruct
{
    double d;
    std::string s;
};

namespace myspace
{
REGISTER_TYPE_BEGIN()
REGISTER_TYPE(mystruct)
REGISTER_TYPE_END()


SHADOW_INIT()
}

int main()
{
}
```

Types must be registered between REGISTER_TYPE_BEGIN() and REGISTER_TYPE_END(),
everything else can be registered after that and before SHADOW_INIT() in any
order desired. Any type that will later appear in registration as a return type,
type of member variable, as a parameter type etc... needs to be registered here
first.

In order to be able to construct instances of custom types through the
reflection system at runtime it is necessary to register constructors. This is
done through the macro REGISTER_CONSTRUCTOR(type_name, param1, param2, ...):
```c++
#include <shadow.hpp>
#include <string>

struct mystruct
{
    double d;
    std::string s;
};

namespace myspace
{
REGISTER_TYPE_BEGIN()
REGISTER_TYPE(mystruct)
REGISTER_TYPE_END()

REGISTER_CONSTRUCTOR(mystruct)
REGISTER_CONSTRUCTOR(mystruct, double)
REGISTER_CONSTRUCTOR(mystruct, double, std::string)

SHADOW_INIT()
}

int main()
{
}
```

Since no constructors were declared in mystruct, REGISTER_CONSTRUCTOR falls back
to using braced initialization. For a type with a declared constructor the
parameter types given to REGISTER_CONSTRUCTOR must match the desired
constructor:
```c++
#include <shadow.hpp>
#include <string>

struct mystruct
{
    double d;
    std::string s;
};

class myclass
{
public:
    myclass(int i) : i_(i)
    {}

private:
    int i_;
};

namespace myspace
{
REGISTER_TYPE_BEGIN()
REGISTER_TYPE(mystruct)
REGISTER_TYPE_END()

REGISTER_CONSTRUCTOR(mystruct)
REGISTER_CONSTRUCTOR(mystruct, double)
REGISTER_CONSTRUCTOR(mystruct, double, std::string)

REGISTER_CONSTRUCTOR(myclass, int)

SHADOW_INIT()
}

int main()
{
}
```

To be able to query and access member variables, they also need to be explicitly
registered. This is done with the macro REGISTER_MEMBER_VARIABLE(type_name,
member_variable_name):
```c++
#include <shadow.hpp>
#include <string>

struct mystruct
{
    double d;
    std::string s;
};

class myclass
{
public:
    myclass(int i) : i_(i)
    {}

private:
    int i_;
};

namespace myspace
{
REGISTER_TYPE_BEGIN()
REGISTER_TYPE(mystruct)
REGISTER_TYPE_END()

REGISTER_CONSTRUCTOR(mystruct)
REGISTER_CONSTRUCTOR(mystruct, double)
REGISTER_CONSTRUCTOR(mystruct, double, std::string)

REGISTER_CONSTRUCTOR(myclass, int)

REGISTER_MEMBER_VARIABLE(myclass, d)
REGISTER_MEMBER_VARIABLE(myclass, s)

SHADOW_INIT()
}

int main()
{
}
```

The type of the member variable is inferred at compile time within
REGISTER_MEMBER_VARIALBE.


Similarly, member functions are registered with the
REGISTER_MEMBER_FUNCTION(class_name, member_function_name). Return type and
parameter types are inferred at compile time:
```c++
#include <shadow.hpp>
#include <string>

struct mystruct
{
    double d;
    std::string s;

    int bar(double dd)
    {
        if(dd > d) { return 1; }
        return 0;
    }

    int bar(const std::string& ss)
    {
        if(ss.size() > s.size()) { return 1; }
        return 0;
    }
};

class myclass
{
public:
    myclass(int i) : i_(i)
    {}
    
    double foo(const mystruct& ms)
    {
        return ms.d;
    }

private:
    int i_;
};

namespace myspace
{
REGISTER_TYPE_BEGIN()
REGISTER_TYPE(mystruct)
REGISTER_TYPE_END()

REGISTER_CONSTRUCTOR(mystruct)
REGISTER_CONSTRUCTOR(mystruct, double)
REGISTER_CONSTRUCTOR(mystruct, double, std::string)

REGISTER_CONSTRUCTOR(myclass, int)

REGISTER_MEMBER_VARIABLE(myclass, d)
REGISTER_MEMBER_VARIABLE(myclass, s)

REGISTER_MEMBER_FUNCTION(myclass, foo)

REGISTER_MEMBER_FUNCTION_EXPLICIT(mystruct, bar, int, double)
REGISTER_MEMBER_FUNCTION_EXPLICIT(mystruct, bar, int, std::string)

SHADOW_INIT()
}

int main()
{
}
```

As can be seen in the example above, overloaded functions need to be registered
with REGISTER_MEMBER_FUNCTION_EXPLICIT(type_name, function_name, return_type,
params...), since attempting to infer the signature would be ambiguous.


Registering free functions is similarly done with
REGISTER_FREE_FUNCTION(function_name) and
REGISTER_FREE_FUNCTION_EXPLICIT(function_name, return_type, params...):
```c++
#include <shadow.hpp>
#include <string>

struct mystruct
{
    double d;
    std::string s;

    int bar(double dd)
    {
        if(dd > d) { return 1; }
        return 0;
    }

    int bar(const std::string& ss)
    {
        if(ss.size() > s.size()) { return 1; }
        return 0;
    }
};

class myclass
{
public:
    myclass(int i) : i_(i)
    {}
    
    double foo(const mystruct& ms)
    {
        return ms.d;
    }

private:
    int i_;
};


static void myfunction(char c)
{
    // do something with c
}

namespace myspace
{
REGISTER_TYPE_BEGIN()
REGISTER_TYPE(mystruct)
REGISTER_TYPE_END()

REGISTER_CONSTRUCTOR(mystruct)
REGISTER_CONSTRUCTOR(mystruct, double)
REGISTER_CONSTRUCTOR(mystruct, double, std::string)

REGISTER_CONSTRUCTOR(myclass, int)

REGISTER_MEMBER_VARIABLE(myclass, d)
REGISTER_MEMBER_VARIABLE(myclass, s)

REGISTER_MEMBER_FUNCTION(myclass, foo)

REGISTER_MEMBER_FUNCTION_EXPLICIT(mystruct, bar, int, double)
REGISTER_MEMBER_FUNCTION_EXPLICIT(mystruct, bar, int, std::string)

REGISTER_FREE_FUNCTION(myfunction)

SHADOW_INIT()
}

int main()
{
}
```

## Interacting with the Reflection System
SHADOW_INIT() instantiates an immutable global instance of
`shadow::reflection_manager` with the name `manager` within the namespace. 

The `shadow::reflection_manager` instance becomes the central source of
information about everything registered in the reflection system. Types,
functions, members etc... are identified by different 'tag type' classes.
Queries generally return iterators to these tag types. Calls to mutate, call
functions, construct objects etc... take these 'tag type' classes as an argument
to identify which function, member variable etc... is used.

To hold values of arbitrary types at runtime, Shadow uses the class
`shadow::object`. It is similar to std::any or boost::any in that it uses type
erasure to hold any type of object. In addition, `shadow::object` carries with
it type/reflection information about the currently held value, in the form of a
pointer to the information.

### Querying for Types
```c++
std::pair<const_type_iterator, const_type_iterator>
shadow::reflection_manager::types() const;
```
To get all types registered, call the above member function on the global
instance 'manager':
```c++
auto types = myspace::manager.types();

auto found_mystruct = std::find_if(types.first, types.second, [](const auto& tp)
{
    return tp.name() == std::string("mystruct");
});
```

All iterators returned by `shadow::reflection_manager` satisfy
RandomAccessIterator, so they should work well with the standard library
algorithms. The type_tag class we get when dereferencing the iterators has two
member functions name() and size() returning the name of the type and size
respectively.


### Querying for and calling Constructors
```c++
std::pair<const_indexed_constructor_iterator, const_indexed_constructor_iterator>
shadow::reflection_manager::constructors_by_type(const type_tag& tag) const;
```

To find all constructors for a certain type, call the above with the desired
type_tag:
```c++
// continuing from example above...
auto constructors = myspace::manager.constructors_by_type(*found_mystruct);

// find default constructor, ie 0 parameters
auto found_constructor = std::find_if(constructors.first, constructors.second,
    [](const auto& ctr){
        auto param_types = myspace::manager.constructor_parameter_types(ctr);
        return std::distance(param_types.first, param_types.second) == 0;
    });

// construct an object holding a mystruct instance
shadow::object holds_mystruct =
    myspace::manager.construct_object(*found_constructor);
```

The above example additionally finds the default constructor for the type
`mystruct` and uses it to construct an instance of it, returned within a
`shadow::object`. Since there are no other values to use as arguments yet we
demostrate with the default constructor.The
`shadow::reflection_manager::constructo_object` above is an overload for calling
default constructors. In general one would use the overload taking two
additional arguments, templated for any kind of iterator that specify a range of
shadow::objects to use as arguments. The arguments types (ie. the
shadow::objects::type()) need to match the parameter types returned by:
```c++
std::pair<const_indexed_type_iterator, const_indexed_type_iterator>
shadow::reflection_manager::constructor_parameter_types(
        const constructor_tag& tag) const;
```

If the types don't match, or the wrong number of arguments are passed, an
exception of type `shadow::argument_error` is thrown (prevents a segfault).

### Types Known at Compile Time
In addition to the facilites for constructing arbitrary objects at runtime,
there are utility functions for constructing objects of type known at compile
time. These utility functions are defined within the namespace where SHADOW_INIT
is:
```c++
template <class T, class... Args>
shadow::object
yournamespace::static_construct(Args&&... args);

template <class T>
shadow::object
yournamespace::static_make_object(T&& value);
```

static_construct lets you explicitly specify the type T and forwards the args to
the constructor of T.
static_make_object deduces the type T and takes an already existing object of
type T. In both cases a shadow::object is returned. These functions will fail at
compile time if the type T is not registered, however, the actual constructor
invoked doesn't necessarily need to be registered.

To access the value held within the shadow::object the following function
templates can be used:
```c++
template <class T>
T& yournamespace::get_held_value(shadow::object& obj);

template <class T>
const T& yournamespace::get_held_value(const shadow::object& obj);
```

If the value held by obj isn't of the type T, an exception of the type
`shadow::type_error` is thrown.


### Type Conversion at Runtime
The reflection system has information about implicit conversions between
registered types. For example, if your custom types has a non-explicit
constructor with one parameter, the reflection system will pick up on the
possible implicit conversion from the parameter type to the custom type.
Conversion of values held in shadow::objects can be queried
and invoked explicitly through:
```c++
std::pair<const_conversion_iterator, const_conversion_iterator>
reflection_manager::conversions() const;

std::pair<const_indexed_conversion_iterator, const_indexed_conversion_iterator>
reflection_manager::conversions_by_from_type(const type_tag& tag) const;

shadow::object
reflection_manager::convert(const conversion_tag& tag,
const shadow::object& val) const;
```


### Free Functions
Free functions are queried and called with the following member functions of
shadow::reflection_manager:
```c++
std::pair<const_free_function_iterator, const_free_function_iterator>
reflection_manager::free_functions() const;

std::string
reflection_manager::free_function_name(const free_function_tag& tag) const;

type_tag
reflection_manager::free_function_return_type(const free_function_tag& tag) const;

std::pair<const_indexed_type_iterator, const_indexed_type_iterator>
reflection_manager::free_function_parameter_types(const free_function_tag& tag) const;

template <class Iterator>
shadow::object
reflection_manager::call_free_function(const free_function_tag& tag,
            Iterator first,
            Iterator last) const;
```

If the arguments supplied through the range Iterator first -> last have types
different from what is given by `free_function_parameter_types` an exception of
the type `shadow::argument_error` is thrown (similar to calling constructors).
In the case that the function is non-returning, the shadow::object returned has
the type `void` when queried through `type()`.

If the underlying function has out parameters in the form of non-const reference
or pointer, the modified value will be passed back out through the Iterator
range supplied.

Shadow doesn't support holding pointers as values in shadow::objects or
registering pointer types. Because of this, if a function requires a pointer
parameter, you must pass a shadow::object of the value that a pointer would have
pointed to. The address of the shadow::object is taken automatically by
`call_free_function` in the case of pointer parameters. Any modification of the
value is passed out through the supplied iterator range.


### Member Functions
Member functions are queried and called similarly to free functions, except that
an object of the class owning the member function is supplied.
```c++
std::pair<const_member_function_iterator, const_member_function_iterator>
reflection_manager::member_functions() const;

std::pair<const_indexed_member_function_iterator, const_indexed_member_function_iterator>
reflection_manager::member_functions_by_class_type(const type_tag& tag) const;

type_tag
reflection_manager::member_function_class_type(const member_function_tag& tag) const;

std::string
reflection_manager::member_function_name(const member_function_tag& tag) const;

std::pair<const_indexed_type_iterator, const_indexed_type_iterator>
reflection_manager::member_function_parameter_types(const member_function_tag& tag) const;
```


### Member Variables
Member variables are queried with through the following member functions:
```c++
std::pair<const_member_variable_iterator, const_member_variable_iterator>
reflection_manager::member_variables() const;

std::pair<const_indexed_member_variable_iterator,
          const_indexed_member_variable_iterator>
reflection_manager::member_variables_by_class_type(const type_tag& tag) const;

type_tag
reflection_manager::member_variable_type(const member_variable_tag& tag) const;

type_tag
reflection_manager::member_variable_class_type(const member_variable_tag& tag) const;

std::string
reflection_managermember_variable_name(const member_variable_tag& tag) const;
```

Since member variables aren't held as `shadow::object`s in memory, we can't
simply access them through a reference. Because of this, access is split into
two member functions, one for setting and one for getting:
```c++
void
reflection_manager::set_member_variable(object& obj,
                         const member_variable_tag& tag,
                         const object& val) const;

shadow::object
reflection_manager::get_member_variable(const object& obj,
                           const member_variable_tag& tag) const;

```


### Built-in Serialization
Shadow overloads the stream operators for `shadow::object`. The format for
fundamental types correspond to the operator<< overloads on std::ostream.

```c++
auto intobj = myspace::static_construct<int>(238);

std::cout << intobj << '\n';
```
output:
```
238
$
```

std::string corresponds to the usual stream overloads, except that the string is
surrounded by double quotes:

```c++
auto stringobj = myspace::static_make_object(std::string("hello world"));

std::cout << stringobj << '\n';
```
output:
```
"hello world"
$
```

The format for custom types is a comma separated list of the member variables
registered surrounded by curly braces:
```c++
auto mystructobj = myspace::static_construct<mystruct>(23.4, std::string("foo"));

std::cout << mystructobj << '\n';
```
output:
```
{23.4, "foo"}
$
```

If a member variable is also a custom type, it will be embedded recursively as
an object within an object:
ie
```
{101, {23.4, "foo"}, c}
$
```

The same format can be deserialized to a shadow::object. The target
shadow::object needs to have the right type matching the input stream:
```c++
std::istringstream in(std::string("{432.4, \"bar\"}"));

in >> mystructobj;

assert(myspace::get_held_value<mystruct>(mystructobj).d == Approx(432.4));
assert(myspace::get_held_value<mystruct>(mystructobj).s == std::string("bar"));
```
