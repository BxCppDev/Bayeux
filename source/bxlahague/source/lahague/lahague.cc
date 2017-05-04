// lahague.cc
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
//

// Ourselves:
#include <lahague/lahague.h>

// Third Party:
// - datatools:
#include <datatools/kernel.h>

namespace lahague {

  void initialize(int /*argc_*/, char ** /*argv_*/)
  {
    //::lahague::_special_initialize_impl();
    return;
  }

  void terminate()
  {
    //::lahague::_special_terminate_impl();
    return;
  }

} // end of namespace lahague
