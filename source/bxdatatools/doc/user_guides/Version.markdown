How to adapt your code to work with different version of datatools 
==================================================================
Using datatools::version {#dt_howto_version}
========================
[TOC]
Introduction {#dt_howto_version_intro}
------------
Datatools provides a small header, datatools/version.h, which provides
compile time macros and a runtime object for querying the current
version number and features of the datatools API.

These are useful if your datatools dependent code needs to support
multiple versions of datatools. To use this part of datatools, simply

* Add `#include "datatools/version.h"` to your code.
* Link your binary to the datatools library (only needed if you wish to
  use the datatools::version struct).

Checking the version number at compile time {#dt_howto_version_compiletime}
-------------------------------------------
Describe how to use the datatools/version.h macros, for example:

~~~~ {.cpp}
#include "datatools/version.h"

...

void compiletime_check() {
#if DATATOOLS_VERSION_MAJOR > 4
  std::cout << "compiletime_check() handles datatools > version 4" << std::endl;
#else
  std::cout << "compiletime_check() legacy" << std::endl;
#endif
}
~~~~

Checking the version number at runtime {#dt_howto_version_runtime}
--------------------------------------
How to use the datatools::version struct and static methods, for example:

~~~~ {.cpp .lines}
#include "datatools/version.h"

...

void runtime_check() {
  if (datatools::version::get_major() > 4) {
    use_new_interface();
  } else {
    use_old_interface();
  }
}
~~~~

