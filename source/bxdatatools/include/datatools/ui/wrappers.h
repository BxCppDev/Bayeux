//! \file  datatools/ui/wrappers.h
//! \brief Simple type wrappers
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

#ifndef DATATOOLS_UI_WRAPPERS_H
#define DATATOOLS_UI_WRAPPERS_H

// Standard library:
#include <string>

namespace datatools {

  namespace ui {

    /// \brief Wrapper for a boolean value
    struct wrapped_boolean {
    public:
      wrapped_boolean();
      bool value;
    };

    /// \brief Wrapper for an integer value
    struct wrapped_integer {
    public:
      wrapped_integer();
      int value;
    };

    /// \brief Wrapper for a dimensionless real value
    struct wrapped_real_without_unit {
    public:
      wrapped_real_without_unit();
      double value;
      bool   normal;
    };

    /// \brief Wrapper for a real value with associated unit
    struct wrapped_real_with_unit {
    public:
      wrapped_real_with_unit(const std::string & required_unit_dimension_ = "");
      double      value;
      bool        normal;
      std::string required_unit_dimension;
      std::string unit_symbol;
      std::string unit_label;
    };

  } // namespace ui

} // namespace datatools

#endif // DATATOOLS_UI_WRAPPERS_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
