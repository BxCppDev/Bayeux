//! \file    bayeux/bayeux.h
//! \brief   Provide system init for Bayeux internals
//! \details
//
// Copyright (c) 2013 by Ben Morgan <bmorgan.warwick@gmail.com>
// Copyright (c) 2013 by The University of Warwick
// Copyright (c) 2013 by Francois Mauger <mauger@lpccaen.in2p3.fr>
// Copyright (c) 2013 by Université de Caen
//
// This file is part of Bayeux.
//
// Bayeux is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Bayeux is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Bayeux.  If not, see <http://www.gnu.org/licenses/>.

#ifndef BAYEUX_BAYEUX_H
#define BAYEUX_BAYEUX_H
// Standard Library
#include <string>

// This project

namespace bayeux {

  void initialize(int argc_ = 0, char * argv_[] = 0);

  void terminate();

  void _special_initialize_impl();

  void _special_terminate_impl();

}

/// bayeux initialization macro using main function arguments
#define BAYEUX_INIT_MAIN(Argc,Argv)		\
  ::bayeux::initialize( Argc , Argv );		\
  ::bayeux::_special_initialize_impl();		\
  /**/

/// bayeux initialization macro
#define BAYEUX_INIT()				\
  ::bayeux::initialize();			\
  ::bayeux::_special_initialize_impl();		\
  /**/

/// bayeux termination macro
#define BAYEUX_FINI()				\
  ::bayeux::_special_terminate_impl();		\
  ::bayeux::terminate();			\
  /**/

#endif // BAYEUX_BAYEUX_H
