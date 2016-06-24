Unit Testing for Bayeux using Catch {#bxdevel_unittest}
===================================

\tableofcontents

Introduction {#bxdevel_unittest_intro}
============

All functions and classes within Bayeux must have accompanying unit tests 
that exercise and verify the specifications of their interfaces. To help
developers easily write and run tests, Bayeux uses the [Catch](https://github.com/philsquared/Catch)
framework for authoring C++ tests, and the [CTest](https://cmake.org/cmake/help/v3.3/manual/ctest.1.html) 
component of CMake to run them.

To enable the build and running of tests in Bayeux, the CMake variable
`BAYEUX_ENABLE_TESTING` must be set to `ON`. This may be done on the command
line via a `-D` argument to CMake or through the Curses/GUI CMake interfaces.
It can be set/unset either on the initial configuration, or in subsequent
reconfigurations.


Writing Catch-based Unit Tests for Bayeux {#bxdevel_unittest_writing}
=========================================

We defer documentation on writing unit tests with Catch to the comprehensive
<A HREF="https://github.com/philsquared/Catch/blob/master/docs/tutorial.md">Tutorial</A>
and
[Reference Guides](https://github.com/philsquared/Catch/tree/master/docs) available
on Catch's github site. However, there are a couple of coding and structure issues to be
aware of to ensure your tests integrate easily with Bayeux.

To speed up compile times, Bayeux precompiles a main program for Catch into the `BXCatch`
static library that is linked to tests together with the `Bayeux` library itself. The structure 
of Bayeux's Catch tests thus only requires inclusion of the Catch header, and the `CATCH_CONFIG_MAIN` 
macro should *not* be defined. Therefore most tests should take the form:

```cpp
#include "catch.hpp"   // Just the Catch API

#include "bayeux/mymodule/myinterfacetotest.h"    // Declaration(s) for what needs testing

TEST_CASE("Test myinterface", "[bayeux][mymodule]") {
...
}
```

As shown in the above example, you should also use Catch's
<A HREF="https://github.com/philsquared/Catch/blob/master/docs/test-cases-and-sections.md#tags">Tags facility</A>
to name cases according to the submodule they are part of the aspect of the interface they are testing.

BY default, the `main` program built for Catch testing does not run any of Bayeux's
setup/teardown functions for self-location. If your tests require this functionality,
you should add the calls at the start/end of your test cases as needed, e.g.

```cpp
TEST_CASE("Test resources", "[bayeux][mymodule]") {
  bayeux::initialize();
  ...

  bayeux::terminate();
}
```

Addition of the calls to Bayeux's Catch-main program is being reviewed.


Integrating Tests into Bayeux's Testing System {#bxdevel_unittest_integrating}
==============================================

To let Bayeux know about your Catch-based tests, simply add the file to the list of Catch tests declared
for the module in its `.cmake` file. In the transition to full Catch based tests, Bayeux modules need
to define two variables to list source files for tests, as shown in the following example:

```cmake
... the "bxmymodule_module.cmake" file...

# List old, non-Catch tests in this variable
set(${module_name}_MODULE_TESTS
  ...
  ${module_test_dir}/test_myoldtest.cxx
  ..
  )

# List new Catch based tests in this one
set(${module_name}_MODULE_TESTS_CATCH
  ...
  ${module_test_dir}/test_myinterfacetest.cxx
  ...
  )

```

As you migrate your tests to Catch, simply move the file from the old list
to the new. Bayeux's CMake system will automatically pick up the test lists and compile 
test programs for them named `<submodule>-<sourcefilename>`. Taking the above script as an example, the 
`test_myinterfacetest.cxx` file in the `mymodule` submodule would be compiled to a program
named `mymodule-test_myinterfacetest`. All test executables are output to the 
`BuildProducts/bin/bxtests` subdirectory of Bayeux's build directory.

Bayeux's CMake system will take care of the compilation and addition of the executables to
the testing system.

Running Bayeux's Tests with CMake/CTest {#bxdevel_unittest_running}
=======================================

When Bayeux's testing is enabled, CMake will build the executables and add them to the
tests known by CTest. There are three main ways to run the tests after building Bayeux
itself.

CMake will create a target in the buildtool that can be "built" to run all the tests and
report their success/failure. For single mode tools like Make and Ninja this is the `test`
target, which may be "built" by:

```console
$ make test
```

or

```console
$ ninja test
```

In IDEs like Xcode, a new build spec/template is created, usually named "RUN_TEST" or similar.
This may be built/run using the tools in the IDE.

The second way to run tests, independent of the buildtool, is to run CMake's [`ctest` executable](https://cmake.org/cmake/help/v3.3/manual/ctest.1.html)
directly in Bayeux's build directory. This is useful if you find tests are failing and wish to run individual tests
to inspect the failures in detail. Please consult the [ctest manual](https://cmake.org/cmake/help/v3.3/manual/ctest.1.html)
for full details on the command line options to select individual tests and verbosity levels.

Finally, the test programs are fully functional command line executables, so may be runn directly.
Tests with a Catch supplied main provide a command line interface for listing and working with
individual test cases in the program. Simply run

```console
$ ./BuildProducts/bin/bxtests/<testname> --help
```

for a list of options.

