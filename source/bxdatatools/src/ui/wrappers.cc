// datatools/ui/wrappers.cc
//
// Copyright (c) 2016 by François Mauger <mauger@lpccaen.in2p3.fr>
//
// This file is part of datatools.
//
// datatools is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// datatools is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with datatools. If not, see <http://www.gnu.org/licenses/>.

// Ourselves
#include <datatools/ui/wrappers.h>

// This project:
#include <datatools/utils.h>
#include <datatools/ioutils.h>
#include <datatools/units.h>
#include <datatools/logger.h>

namespace datatools {

  namespace ui {

    wrapped_boolean::wrapped_boolean()
    {
      value = false;
      return;
    }

    wrapped_integer::wrapped_integer()
    {
      value = 0;
      return;
    }

    wrapped_real_without_unit::wrapped_real_without_unit()
    {
      value = datatools::invalid_real();
      normal = false;
      return;
    }

    wrapped_real_with_unit::wrapped_real_with_unit(const std::string & required_unit_dimension_)
    {
      required_unit_dimension = required_unit_dimension_;
      value = datatools::invalid_real();
      normal = false;
      unit_symbol.clear();
      return;
    }


  } // namespace ui

} // namespace datatools
