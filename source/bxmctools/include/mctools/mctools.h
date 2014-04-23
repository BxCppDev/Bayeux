//! \file   mctools/mctools.h
//! \author François Mauger
//! \brief  Describe the mctools API configuration

#ifndef MCTOOLS_MCTOOLS_H
#define MCTOOLS_MCTOOLS_H 1

// This project
#include <datatools/datatools.h>
#include <mctools/mctools_config.h>

/// Top-level namespace of the Bayeux/mctools module library
namespace mctools {

  void initialize(int argc_ = 0, char ** argv_ = 0);

  void terminate();

}

/// Bayeux/mctools initialization macro using main function arguments
#define MCTOOLS_INIT_MAIN(Argc,Argv)             \
  DATATOOLS_INIT_MAIN( Argc , Argv );            \
  ::mctools::initialize( Argc , Argv );          \
  /**/

/// Bayeux/mctools kernel initialization macro
#define MCTOOLS_INIT()                           \
  DATATOOLS_INIT_MAIN( 0, 0 );                   \
  ::mctools::initialize(0, 0);                   \
  /**/

#define MCTOOLS_FINI()                             \
  ::mctools::terminate();                          \
  DATATOOLS_FINI();                                \
  /**/


#endif // MCTOOLS_MCTOOLS_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
