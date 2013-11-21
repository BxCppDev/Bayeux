/* datatools/_init_fini.cc
 */
// Third party
// - Boost
#include <boost/scoped_ptr.hpp>

// This package:
#include <datatools/datatools_config.h>
#include <datatools/library_info.h>
#include <datatools/properties.h>
#include <datatools/version.h>
#include <datatools/kernel.h>
#include <datatools/datatools.h>

// Useful reading:
// http://gcc.gnu.org/onlinedocs/gcc/Function-Attributes.html
// http://gcc.gnu.org/onlinedocs/gcc/C_002b_002b-Attributes.html#C_002b_002b-Attributes

__attribute__((constructor))
static void datatools_initializer()
{
  //DT_LOG_TRACE(datatools::logger::PRIO_TRACE, "Entering...");
  datatools::initialize(0,0);
  //DT_LOG_TRACE(datatools::logger::PRIO_TRACE, "Exiting.");
  return;
}

__attribute__((destructor))
static void datatools_finalizer()
{
  //DT_LOG_TRACE(datatools::logger::PRIO_TRACE, "Entering...");
  datatools::terminate();
  //DT_LOG_TRACE(datatools::logger::PRIO_TRACE, "Exiting.");
  return;
}

// end of datatools/_init_fini.cc
