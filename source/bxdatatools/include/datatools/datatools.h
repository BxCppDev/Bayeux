// -*- mode: c++; -*-
//! \file   datatools.h
//! \author François Mauger
//! \brief  Describe the datatools API configuration

#ifndef DATATOOLS_DATATOOLS_H_
#define DATATOOLS_DATATOOLS_H_ 1

// Third party
// - Boost
#include <boost/scoped_ptr.hpp>

// This project
#include <datatools/datatools_config.h>

/// Top-level namespace of the Bayeux/datatools module library
namespace datatools {

  void initialize(int argc_ = 0, char * argv_[] = 0);

  void terminate();

  // details... should be hidden
  void _special_initialize_impl();

  void _special_terminate_impl();
}

/// datatools' kernel initialization macro using main function arguments
#define DATATOOLS_INIT_MAIN(Argc,Argv)          \
  ::datatools::initialize( Argc , Argv );       \
  /**/

/// datatools' kernel initialization macro
#define DATATOOLS_INIT()                        \
  ::datatools::initialize(0, 0);                \
  /**/

#define DATATOOLS_FINI()                        \
  ::datatools::terminate();                     \
  /**/


#endif // DATATOOLS_DATATOOLS_H_

// end of datatools.h
