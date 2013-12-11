// -*- mode: c++; -*-
//! \file   materials.h
//! \author François Mauger
//! \brief  Describe the materials API configuration

#ifndef MATERIALS_MATERIALS_H_
#define MATERIALS_MATERIALS_H_ 1

// This project
#include <datatools/datatools.h>
#include <materials/materials_config.h>

namespace materials {

  void initialize(int argc_ = 0, char ** argv_ = 0);

  void terminate();

  // details... should be hidden
  void _special_initialize_impl();

  void _special_terminate_impl();
}

/// materials' kernel initialization macro using main function arguments
#define MATERIALS_INIT_MAIN(Argc,Argv)          \
  DATATOOLS_INIT_MAIN( Argc , Argv );           \
  ::materials::initialize( Argc , Argv );       \
  /**/

/// materials' kernel initialization macro
#define MATERIALS_INIT()                        \
  DATATOOLS_INIT_MAIN( 0, 0 );                  \
  ::materials::initialize(0, 0);                \
  /**/

#define MATERIALS_FINI()                        \
  ::materials::terminate();                     \
  DATATOOLS_FINI();                             \
  /**/


#endif // MATERIALS_MATERIALS_H_

// end of materials.h
