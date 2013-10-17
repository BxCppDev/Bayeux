// -*- mode: c++; -*-
//! \file   genbb_help.h
//! \author François Mauger
//! \brief  Describe the genbb_help API configuration

#ifndef GENBB_HELP_GENBB_HELP_H_
#define GENBB_HELP_GENBB_HELP_H_ 1

// This project
#include <datatools/datatools.h>
#include <genbb_help/genbb_help_config.h>

namespace genbb {

  void initialize(int argc_ = 0, char * argv_[] = 0);

  void terminate();

  // details... should be hidden
  void _special_initialize_impl();

  void _special_terminate_impl();
}

/// genbb_help' kernel initialization macro using main function arguments
#define GENBB_HELP_INIT_MAIN(Argc,Argv)          \
  DATATOOLS_INIT_MAIN( Argc , Argv );		 \
  ::genbb::initialize( Argc , Argv );		 \
  /**/

/// genbb_help' kernel initialization macro
#define GENBB_HELP_INIT()                        \
  DATATOOLS_INIT_MAIN( 0, 0 );			 \
  ::genbb::initialize(0, 0);			 \
  /**/

#define GENBB_HELP_FINI()                        \
  ::genbb_help::terminate();                     \
  DATATOOLS_FINI();				 \
  /**/


#endif // GENBB_HELP_GENBB_HELP_H_

// end of genbb_help.h
