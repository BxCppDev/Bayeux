//! \file  datatools/introspection/data_layout.h
//! \brief Data layout information
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

#ifndef DATATOOLS_INTROSPECTION_DATA_LAYOUT_H
#define DATATOOLS_INTROSPECTION_DATA_LAYOUT_H

// Standard library:
#include <string>

#ifndef Q_MOC_RUN
#include <datatools/reflection_interface.h>
#endif // Q_MOC_RUN

namespace datatools {

  namespace introspection {

    //! \brief Layout (scalar/vector) of the data
    //!
    //! Examples:
    //! \code
    //! int i;              // A "scalar" data
    //! int t[3];           // A "vector" data with fixed size
    //! std::vector<int> v; // A "vector" data with free size
    //! \endcode
    enum data_layout {
      DATA_LAYOUT_INVALID                = 0, //!< Invalid data layout
      DATA_LAYOUT_SCALAR                 = 1, //!< Scalar data
      DATA_LAYOUT_VECTOR_WITH_FIXED_SIZE = 2, //!< Vector data with fixed size
      DATA_LAYOUT_VECTOR_WITH_FREE_SIZE  = 3  //!< Vector data with free size
    };

    //! Convert a data layout to a string
    const std::string & to_string(data_layout);

    //! Convert a string to a data layout
    bool from_string(const std::string & label_, data_layout & layout_);

    //! Check the validity of a data layout
    bool is_valid(data_layout);

    //! Check if a data layout is scalar
    bool is_scalar(data_layout);

    //! Check if a data layout is vector
    bool is_vector(data_layout);

  } // namespace introspection

} // namespace datatools

#ifndef Q_MOC_RUN
// Activate reflection layer:
DR_TYPE_INIT(::datatools::introspection::data_layout)
#endif // Q_MOC_RUN

#endif // DATATOOLS_INTROSPECTION_DATA_LAYOUT_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
