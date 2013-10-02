/* bayeux/_init_fini.cc
 */
// Ourselves
#include <bayeux/bayeux.h>

// This package:
#include <datatools/logger.h>

__attribute__((constructor))
static void bayeux_initializer()
{
  /******************************************************
   * This is an implicit initialization of the library. *
   * This technique may/will change in the future.      *
   ******************************************************/
  DT_LOG_TRACE(datatools::logger::PRIO_TRACE, "Entering...");
  ::bayeux::initialize(0,0);
  DT_LOG_TRACE(datatools::logger::PRIO_TRACE, "Exiting.");
  return;
}

__attribute__((destructor))
static void bayeux_finalizer()
{
  /******************************************************
   * This is an implicit termination of the library.    *
   * This technique may/will change in the future.      *
   ******************************************************/
  DT_LOG_TRACE(datatools::logger::PRIO_TRACE, "Entering...");
  ::bayeux::terminate();
  DT_LOG_TRACE(datatools::logger::PRIO_TRACE, "Exiting.");
  return;
}


// end of bayeux/_init_fini.cc
