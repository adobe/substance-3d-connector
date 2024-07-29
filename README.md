# Substance Connector

Substance Connector is a communication framework for connecting applications
to one another to share generic data. Our initial use case is to automate and
simplify the process of exporting and importing assets between applications. 
This framework can be expanded for use in sharing any data between applications.


--------------------------------------------------------------------------
                        CLI Tools and Default Build
--------------------------------------------------------------------------
The default cmake project build will create the python project folder for
the CLI tool set. The CLI tools act as a connector endpoint and can be used
to test features and/or integrations of Substance Connector. The CLI tools also
acts as a recommended example of how a connector integration is structured.
The CLI tools are written in Python but the structure remains the same for all
language implementations. The default install directory will either be
./install or ./cli/install based on if cmake is run at the root or within the
./cli folder respectively.


--------------------------------------------------------------------------
    Implementation Details
--------------------------------------------------------------------------
The core library is implemented in the C programming language, allowing it to
natively bind to most any existing programming language with little effort.
Almost every language allows to bind to native C code.

This allows the core module to be used in conceivably any language that has bindings
to C, although certain things like function pointers may become difficult,
depending on the FFI of the language.


--------------------------------------------------------------------------
    Using the Framework
--------------------------------------------------------------------------
There are build flags listed in the top level of the cmake. The build flags
can be enabled for choosing which language wrapper for the current environment.
One flag should be enabled at a time and it will create all of the relevant cmake 
targets. Enabling more than one could lead to build errors or unused cmake targets.
For example, enabling the QT module flag will build core, C++ module, and
the QT module. If no flags are set, the project will default to building the CLI
tools. The CLI tool is a standalone connector instance and can be used for 
testing and can also be used as a reference for how an integration is structured.

General lifecycle of an integrations:
- Create instance
- Create feature components
- Bind callbacks to components
- Initialize instance
- Broadcast that a new instance is available
- Shutdown

Connections are automatic once a broadcast message has been sent. Every existing connector
end point will have context of a new connection and vice versa. 


--------------------------------------------------------------------------
    Expanding the Framework 
--------------------------------------------------------------------------

Messages are sent using a string. We use json as a string for formatting
message structure but this can be used however the user sees fit. All that is
needed is to create a new derivative of the module base class. As an example, 
the application.h file in the C++ module is the base for feature components and
export.h is an implementation of a feature. In this instance, we are creating a
feature component that handles sending and receiving of assets that do not need
any additional context for exporting and importing. For adding a PR of a new feature/
schema, json is expected and an implementation is expected in all language modules.

--------------------------------------------------------------------------
    Coding Style
--------------------------------------------------------------------------

The naming convention for functions is as follows:
    * All externally facing functions shall be prefixed by substance_connector
    * All internal functions that do not have static linkage are prefixed
      by connector (code inside the details folder)
    * All static functions need not be prefixed by either

Some internal headers have compatibility with C++, in order to leverage C++
testing engines with internals of the libraries. This isn't a requirement,
but may be needed for testing to ensure proper linking.

All external headers must be wrapped in extern "C" declarations for C++
compatibility, with the corresponding test for C++ defines so that they
may also be used in C.

Headers must have a guard statement with the format _SUBSTANCE_CONNECTOR_<HEADER>_H
to ensure reasonable header define namespacing.

External headers should be located in the include/substance/connector folder,
meaning that with include as an include directory, an application can
include a header with #include <substance/connector/<header>.h>. All headers
used internally must be in the details subfolder, so they can be included
as #include <substance/connector/details/<header>.h> by any internal file or
by the testing suite.

All macro flags to be respected at compile time (wrapped in ifndef flags)
must be prefixed with SUBSTANCE_CONNECTOR. Flags that are shared between files,
such as in a header, must use the same prefix. Macros used within a single
file should be prefixed with CONNECTOR.

Enum constants shall also use the SUBSTANCE_CONNECTOR prefix, and be in all caps,
if they are included in a header. If the enum is local in scope to a
compilation unit, such as inside of a .c file, they should be prefixed by
CONNECTOR and should also remain in all caps.

Typedefs for structure types or enum types should have the format connector_<name>_t
if internal, and should have the name substance_connector_<name>_t if they are in an
external header. Function pointers will have the same convention, except with
_fp at the end instead of _t.

Structures generally have the form struct _connector_<name>, with the corresponding
typedef as connector_<name>_t.

Lines should generally be kept at 80 characters or less. There may be function
signatures that need to break this.

Snake case should be used for both variable names and for function names.
Allman-style braces should be used, with four spaces for indentation.

Variables should always have an initial value, unless some form of struct.
Pointers must be initialized to a value or to NULL. Integers should be
initialized to an intentional value or to zero.

Pointers should be declared constant if they are not modified, unless the
internal function reserves the right to modify it.

Memory allocations must go through the proper connector_allocate and connector_free
calls, to ensure a standard memory allocation throughout the codebase. The
API allows for changing the allocator and free function before initialization.

Rationale - There is a lot of system-specific code that occurs here. There
should be as few surprises when going into a piece of the codebase, as it
should look similar to the whole. It is also a C codebase, and should follow
conventions that are different than the C++ codebases that are typical to
to integrations.

--------------------------------------------------------------------------
    Compile flags 
--------------------------------------------------------------------------

There are some define flags that are open for input at compilation. They
have not all been documented here, but some of them are the following:

Context count - The amount of contexts available can be compiled in:
    SUBSTANCE_CONNECTOR_CONTEXT_COUNT

select vs. poll - There are platform defines for each of these, but it will
    honor if the following are set:
    SUBSTANCE_CONNECTOR_FORCE_SELECT
    SUBSTANCE_CONNECTOR_FORCE_POLL
If both are set, it gives precedence to select

Inbound and outbound thread count - Inbound and outbound worker threads can
    be set separately, with the following:
    SUBSTANCE_CONNECTOR_INBOUND_COUNT
    SUBSTANCE_CONNECTOR_OUTBOUND_COUNT

Dispatch thread count - The number of dispatch threads can be set with
    the following:
    SUBSTANCE_CONNECTOR_DISPATCH_COUNT

Socket backlog count - Number of connections to allow on a listen call
    SUBSTANCE_CONNECTOR_SOCK_BACKLOG

--------------------------------------------------------------------------
    Bindings
--------------------------------------------------------------------------

Currently supported language bindings:

C, C++ (Natively)
Python 2
Python 3
Tcl
Haskell
Qml
SBCL Common Lisp

Tested build platforms (just C):
Linux (x86-64, arm-musl-32-bit, arm-musl-64-bit, PowerPC 32-bit)
MacOS (x86-64)
