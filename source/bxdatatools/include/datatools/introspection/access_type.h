//! \file  datatools/introspection/access_type.h
//! \brief Method argument access type
//
// Copyright (c) 2015 by François Mauger <mauger@lpccaen.in2p3.fr>
//
// This file is part of Datatools.
//
// Datatools is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Datatools is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Datatools. If not, see <http://www.gnu.org/licenses/>.

#ifndef DATATOOLS_INTROSPECTION_ACCESS_TYPE_H
#define DATATOOLS_INTROSPECTION_ACCESS_TYPE_H

// Standard library:
#include <string>

#ifndef Q_MOC_RUN
#include <datatools/reflection_interface.h>
#endif // Q_MOC_RUN

namespace datatools {

  /// \brief Nested namespace of the Bayeux/datatools module library
  namespace introspection {

    //! \brief Access type associated to a method argument
    enum access_type {
      ACCESS_INVALID =  0, //!< Invalid access
      ACCESS_INPUT   =  1, //!< Input
      ACCESS_OUTPUT  =  2  //!< Output
    };

    //! Convert an access type to a string
    const std::string & to_string(access_type);

    //! Convert a string to an access type
    bool from_string(const std::string & label_, access_type & access_);

    //! Check the validity of an access type
    bool is_valid(access_type t_);

    //! Check if an access type is input
    bool is_input(access_type t_);

    //! Check if an access type is output
    bool is_output(access_type t_);

  } // namespace introspection

} // namespace datatools

#ifndef Q_MOC_RUN
// Activate reflection layer:
DR_TYPE_INIT(::datatools::introspection::access_type)
#endif // Q_MOC_RUN

#endif // DATATOOLS_INTROSPECTION_ACCESS_TYPE_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
