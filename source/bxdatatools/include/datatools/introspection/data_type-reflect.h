//! \file  datatools/introspection/data_type-reflect.h
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

#ifndef DATATOOLS_INTROSPECTION_DATA_TYPE_REFLECT_H
#define DATATOOLS_INTROSPECTION_DATA_TYPE_REFLECT_H

// Ourselves:
#include <datatools/introspection/data_type.h>

// Registration:
DR_TYPE_EXPORT(::datatools::introspection::data_type, "datatools::introspection::data_type");

namespace datatools {
  namespace detail {
    namespace reflection {

      DR_TYPE_IMPLEMENT_REFLECTION_HEAD(::datatools::introspection::data_type, /*tag_*/)
      {
        DR_ENUM_DECLARE(::datatools::introspection::data_type)
          .DR_ENUM_PAIR("invalid", ::datatools::introspection::DATA_TYPE_INVALID)
          .DR_ENUM_PAIR("void",    ::datatools::introspection::DATA_TYPE_VOID)
          .DR_ENUM_PAIR("boolean", ::datatools::introspection::DATA_TYPE_BOOLEAN)
          .DR_ENUM_PAIR("int8",    ::datatools::introspection::DATA_TYPE_INT8)
          .DR_ENUM_PAIR("uint8",   ::datatools::introspection::DATA_TYPE_UINT8)
          .DR_ENUM_PAIR("int16",   ::datatools::introspection::DATA_TYPE_INT16)
          .DR_ENUM_PAIR("uint16",  ::datatools::introspection::DATA_TYPE_UINT16)
          .DR_ENUM_PAIR("int32",   ::datatools::introspection::DATA_TYPE_INT32)
          .DR_ENUM_PAIR("uint32",  ::datatools::introspection::DATA_TYPE_UINT32)
          .DR_ENUM_PAIR("int64",   ::datatools::introspection::DATA_TYPE_INT64)
          .DR_ENUM_PAIR("uint64",  ::datatools::introspection::DATA_TYPE_UINT64)
          .DR_ENUM_PAIR("float_dimensionless",       ::datatools::introspection::DATA_TYPE_FLOAT_DIMENSIONLESS)
          .DR_ENUM_PAIR("float_with_explicit_unit",  ::datatools::introspection::DATA_TYPE_FLOAT_WITH_EXPLICIT_UNIT)
          .DR_ENUM_PAIR("float_with_implicit_unit",  ::datatools::introspection::DATA_TYPE_FLOAT_WITH_IMPLICIT_UNIT)
          .DR_ENUM_PAIR("double_dimensionless",      ::datatools::introspection::DATA_TYPE_DOUBLE_DIMENSIONLESS)
          .DR_ENUM_PAIR("double_with_explicit_unit", ::datatools::introspection::DATA_TYPE_DOUBLE_WITH_EXPLICIT_UNIT)
          .DR_ENUM_PAIR("double_with_implicit_unit", ::datatools::introspection::DATA_TYPE_DOUBLE_WITH_IMPLICIT_UNIT)
          .DR_ENUM_PAIR("string", ::datatools::introspection::DATA_TYPE_STRING)
          .DR_ENUM_PAIR("path",   ::datatools::introspection::DATA_TYPE_PATH)
          .DR_ENUM_PAIR("enum",   ::datatools::introspection::DATA_TYPE_ENUM)
          .DR_ENUM_PAIR("user",   ::datatools::introspection::DATA_TYPE_USER)
          ;
        return;
      }

    } // end of namespace reflection
  } // end of namespace detail
} // end of namespace datatools

#endif // DATATOOLS_INTROSPECTION_DATA_TYPE_REFLECT_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
