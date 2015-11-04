//! \file  datatools/introspection/unit_support_type-reflect.h
//! \brief Data type introspection support
//
// Copyright (c) 2015 by François Mauger <mauger@lpccaen.in2p3.fr>
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

#ifndef DATATOOLS_INTROSPECTION_UNIT_SUPPORT_TYPE_REFLECT_H
#define DATATOOLS_INTROSPECTION_UNIT_SUPPORT_TYPE_REFLECT_H

// Ourselves:
#include <datatools/introspection/unit_support.h>

// Registration:
DR_TYPE_EXPORT(::datatools::introspection::unit_support_type, "datatools::introspection::unit_support_type");

namespace datatools {
  namespace detail {
    namespace reflection {

      DR_TYPE_IMPLEMENT_REFLECTION_HEAD(::datatools::introspection::unit_support_type, /*tag_*/)
      {
        DR_ENUM_DECLARE(::datatools::introspection::unit_support_type)
          .DR_ENUM_PAIR("invalid",            ::datatools::introspection::UNIT_SUPPORT_INVALID)
          .DR_ENUM_PAIR("none",               ::datatools::introspection::UNIT_SUPPORT_NONE)
          .DR_ENUM_PAIR("implicit_unit",      ::datatools::introspection::UNIT_SUPPORT_IMPLICIT_UNIT)
          .DR_ENUM_PAIR("explicit_unit_dimension", ::datatools::introspection::UNIT_SUPPORT_EXPLICIT_DIMENSION)
          ;
        return;
      }

    } // end of namespace reflection
  } // end of namespace detail
} // end of namespace datatools

#endif // DATATOOLS_INTROSPECTION_UNIT_SUPPORT_TYPE_REFLECT_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
