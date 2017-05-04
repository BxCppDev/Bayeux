//! \file   lahague/lahague.h
//! \author François Mauger
//! \brief  Describe the LaHague API configuration
//
// Copyright (c) 2015 by François Mauger <mauger@lpccaen.in2p3.fr>
//                       and Arnaud Chapon <achapon@cerap.fr>
//
// This file is part of LaHague.
//
// LaHague is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// LaHague is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with LaHague. If not, see <http://www.gnu.org/licenses/>.

#ifndef LAHAGUE_LAHAGUE_H
#define LAHAGUE_LAHAGUE_H

// Third party:
// - Bayeux/datatools :
#include <datatools/datatools.h>

// This project
#include <lahague/lahague_config.h>

/// Top-level namespace of the Bayeux/LaHague module library
namespace lahague {

  void initialize(int argc_ = 0, char ** argv_ = 0);

  void terminate();

}

/// Bayeux/LaHague initialization macro using main function arguments
#define LAHAGUE_INIT_MAIN(Argc,Argv)            \
  DATATOOLS_INIT_MAIN( Argc , Argv );           \
  ::lahague::initialize( Argc , Argv );         \
  /**/

/// Bayeux/LaHague kernel initialization macro
#define LAHAGUE_INIT()                          \
  DATATOOLS_INIT_MAIN( 0, 0 );                  \
  ::lahague::initialize(0, 0);                  \
  /**/

#define LAHAGUE_FINI()                          \
  ::lahague::terminate();                       \
  DATATOOLS_FINI();                             \
  /**/

#endif // LAHAGUE_LAHAGUE_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
