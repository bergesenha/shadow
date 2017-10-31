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


## Usage
Before anything else you need to register the parts of your existing code that
you wish to interact with through the reflection system. Registering is done
with macros.
