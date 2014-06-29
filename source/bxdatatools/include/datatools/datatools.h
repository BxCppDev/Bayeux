// -*- mode: c++; -*-
//! \file   datatools/datatools.h
//! \author François Mauger
//! \brief  Describe the datatools API configuration

#ifndef DATATOOLS_DATATOOLS_H
#define DATATOOLS_DATATOOLS_H 1

// Third party:
// - Boost:
#include <boost/scoped_ptr.hpp>

// This project:
#include <datatools/datatools_config.h>

/// Top-level namespace of the Bayeux/datatools module library
namespace datatools {

  /// Initialize the datatools library and kernel
  void initialize(int argc_ = 0, char * argv_[] = 0);

  /// Terminate the datatools library and kernel
  void terminate();

  // details... should be hidden
  void _special_initialize_impl();

  // details... should be hidden
  void _special_terminate_impl();
}

/// datatools' kernel initialization macro using main function arguments
#define DATATOOLS_INIT_MAIN(Argc,Argv)          \
  ::datatools::initialize( Argc , Argv );       \
  /**/

/// datatools' kernel initialization macro usable from the main function of an application
#define DATATOOLS_INIT()                        \
  ::datatools::initialize(0, 0);                \
  /**/

/// datatools' kernel termination macro usable from the main function of an application
#define DATATOOLS_FINI()                        \
  ::datatools::terminate();                     \
  /**/

#endif // DATATOOLS_DATATOOLS_H
