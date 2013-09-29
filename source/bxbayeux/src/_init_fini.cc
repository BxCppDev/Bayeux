/* bayeux/_init_fini.cc
 */
// Ourselves
#include <bayeux/bayeux_config.h>

// This package:
#include <datatools/logger.h>
#include <bayeux/bayeux.h>

__attribute__((constructor))
static void bayeux_initializer()
{
  /******************************************************
   * This is an implicit initialization of the library. *
   * This technique may/will change in the future.      *
   ******************************************************/
  DT_LOG_TRACE(datatools::logger::PRIO_TRACE, "Entering...");
  bayeux::initialize(0,0);
  bayeux::_special_initialize_impl();
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
  bayeux::_special_terminate_impl();
  bayeux::terminate();
  DT_LOG_TRACE(datatools::logger::PRIO_TRACE, "Exiting.");
  return;
}


// end of bayeux/_init_fini.cc
