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

// Ourselves
#include "bayeux/bayeux.h"

// This project
#include <datatools/datatools.h>

namespace bayeux {

  void initialize(int argc_, char * argv_[])
  {
    // Wrap datatools kernel initialization
    datatools::initialize(argc_,argv_);
    return;
  }

  void terminate()
  {
    // Wrap datatools kernel termination
    ::datatools::terminate();
   return;
  }

} // namespace bayeux
