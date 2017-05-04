// lahague/_init_fini.cc
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

// This package:
#include <lahague/lahague_config.h>
#include <lahague/lahague.h>

__attribute__((constructor))
static void lahague_initializer()
{
  lahague::initialize(0,0);
  return;
}

__attribute__((destructor))
static void lahague_finalizer()
{
  lahague::terminate();
  return;
}
