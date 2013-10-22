// -*- mode: c++; -*-
//! \file   mctools.h
//! \author François Mauger
//! \brief  Describe the mctools API configuration

#ifndef MCTOOLS_MCTOOLS_H_
#define MCTOOLS_MCTOOLS_H_ 1

// This project
#include <datatools/datatools.h>
#include <mctools/mctools_config.h>

namespace mctools {

  void initialize(int argc_ = 0, char * argv_[] = 0);

  void terminate();

  // details... should be hidden
  //void _special_initialize_impl();

  //void _special_terminate_impl();
}

/// mctools' kernel initialization macro using main function arguments
#define MCTOOLS_INIT_MAIN(Argc,Argv)             \
  DATATOOLS_INIT_MAIN( Argc , Argv );            \
  ::mctools::initialize( Argc , Argv );          \
  /**/

/// mctools' kernel initialization macro
#define MCTOOLS_INIT()                           \
  DATATOOLS_INIT_MAIN( 0, 0 );                   \
  ::mctools::initialize(0, 0);                   \
  /**/

#define MCTOOLS_FINI()                             \
  ::mctools::terminate();                          \
  DATATOOLS_FINI();                                \
  /**/


#endif // MCTOOLS_MCTOOLS_H_

// end of mctools.h
