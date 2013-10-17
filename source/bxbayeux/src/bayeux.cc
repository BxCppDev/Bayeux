// bayeux/bayeux.cc - Implementation of bayeux init function
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

// Standard library
#include <iostream>

// Ourselves
#include "bayeux/bayeux.h"

// This project
#include <datatools/datatools.h>

// Tests
// #include <datatools/kernel.h>
// #include <datatools/library_info.h>

namespace bayeux {

  void initialize(int argc_, char * argv_[])
  {
    // Wrap datatools kernel initialization:
    ::datatools::initialize(argc_,argv_);

    // Tests :
    // datatools::kernel & krnl = datatools::kernel::instance();
    // if (krnl.has_library_info_register()) {
    //   datatools::library_info & lib_info_reg
    //     = krnl.grab_library_info_register();
    //   lib_info_reg.tree_dump(std::cerr, "bayeux::initialize: Before special initialize", "DEVEL: ");
    // }

    // Special initialization code:
    ::bayeux::_special_initialize_impl();
    return;
  }

  void terminate()
  {
    // Special termination code:
    ::bayeux::_special_terminate_impl();

    // Wrap datatools kernel termination:
    ::datatools::terminate();

    return;
  }

} // namespace bayeux
