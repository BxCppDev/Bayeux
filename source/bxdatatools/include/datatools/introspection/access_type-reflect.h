//! \file datatools/introspection/access_type-reflect.h
//! \brief Access type introspection support
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

#ifndef DATATOOLS_INTROSPECTION_ACCESS_TYPE_REFLECT_H
#define DATATOOLS_INTROSPECTION_ACCESS_TYPE_REFLECT_H

// Ourselves:
#include <datatools/introspection/access_type.h>

// This project:
#include <datatools/detail/reflection_macros.h>
#include <datatools/detail/reflection_export.h>

// Registration:
DR_TYPE_EXPORT(::datatools::introspection::access_type, "datatools::introspection::access_type")

namespace datatools {
  namespace detail {
    namespace reflection {

      DR_TYPE_IMPLEMENT_REFLECTION_HEAD(::datatools::introspection::access_type, tag_  __attribute__((unused)) )
      {
        DR_ENUM_DECLARE(::datatools::introspection::access_type)
          .value("invalid", ::datatools::introspection::ACCESS_INVALID)
          .value("input",   ::datatools::introspection::ACCESS_INPUT)
          .value("output",  ::datatools::introspection::ACCESS_OUTPUT)
          ;
        return;
      }

    } // end of namespace reflection
  } // end of namespace detail
} // end of namespace datatools

#endif // DATATOOLS_INTROSPECTION_ACCESS_TYPE_REFLECT_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
