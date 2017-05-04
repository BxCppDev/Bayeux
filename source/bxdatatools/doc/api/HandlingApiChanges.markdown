Managing changes to the datatools API
=====================================
Compiler, Runtime and Documentation tools for handling API changes {#dt_dev_apichanges}
================================================================== 
[TOC]
Introduction {#dt_dev_apichanges_intro}
------------
APIs like datatools undergo changes to their interface and available
features as time passes. It's important to think about how these changes
impact users of datatools because even minor changes can often break
client code in awkward ways.

Datatools provides some code based tools to help you manage this change,
but other aspects such as binary compatibility require thought at the
code level. In this guide, we show how to use the former, and discuss the
latter.

Deprecating interfaces {#dt_dev_apichanges_deprecation}
----------------------
A common API change is the deprecation of an interface. A usual practice
is to maintain the deprecated interface over one release, warning users
about the deprecation and future removal and indicating how to update their
code. This warning should be both documented in the API and User Guides,
and warnings should be emitted by code using the deprecated functionality
when compiled, and also when run.

To document an interface as deprecated, use Doxygen's deprecated command. 
This will mark the interface as deprecated in the main API documents,
and add an entry to the Deprecated list under Related Pages.

You can also make use of the deprecated interface emit a warning at
compile time by using the datatools/detail/api.h header. This provides
a (mostly) portable definition that you can use to mark an interface as 
deprecated. Use of this definition will make the compiler emit a warning
with, in most cases, a useful message. For example, say we have a function
`foo` whose name will change to `bar` in the next release. We can
mark this 

~~~~ {.cpp}
#include "datatools/detail/api.h"

namespace datatools {
DATATOOLS_DEPRECATED("use bar function now") void foo() {
  ... foo impl ...
}
}
~~~~

Clients using `foo` would now see a compiler warning being emitted 
reporting the function as deprecated. For modern compilers such as
Clang, GNU 4.5 and later, and MSVC, the message supplied to 
`DATATOOLS_DEPRECATED` will also be emitted. You should always supply this 
message so that it will be displayed to the user if it can. 



