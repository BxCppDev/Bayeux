//! \file  datatools/introspection/data_layout-reflect.h
//! \brief Data layout introspection support
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

#ifndef DATATOOLS_INTROSPECTION_DATA_LAYOUT_REFLECT_H
#define DATATOOLS_INTROSPECTION_DATA_LAYOUT_REFLECT_H

// Ourselves:
#include <datatools/introspection/data_layout.h>

// Registration:
DR_TYPE_EXPORT(::datatools::introspection::data_layout, "datatools::introspection::data_layout");

namespace datatools {
  namespace detail {
    namespace reflection {

      DR_TYPE_IMPLEMENT_REFLECTION_HEAD(::datatools::introspection::data_layout, /*tag_*/)
      {
        DR_ENUM_DECLARE(::datatools::introspection::data_layout)
          .DR_ENUM_PAIR("invalid",                ::datatools::introspection::DATA_LAYOUT_INVALID)
          .DR_ENUM_PAIR("scalar",                 ::datatools::introspection::DATA_LAYOUT_SCALAR)
          .DR_ENUM_PAIR("vector_with_fixed_size", ::datatools::introspection::DATA_LAYOUT_VECTOR_WITH_FIXED_SIZE)
          .DR_ENUM_PAIR("vector_with_free_size",  ::datatools::introspection::DATA_LAYOUT_VECTOR_WITH_FREE_SIZE)
          ;
        return;
      }

    } // end of namespace reflection
  } // end of namespace detail
} // end of namespace datatools

#endif // DATATOOLS_INTROSPECTION_DATA_LAYOUT_REFLECT_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
