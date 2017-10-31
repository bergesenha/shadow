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
