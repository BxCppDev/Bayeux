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

// This project:
#include <datatools/detail/reflection_macros.h>
#include <datatools/detail/reflection_export.h>

// Registration:
DR_TYPE_EXPORT(::datatools::introspection::data_type, "datatools::introspection::data_type")

namespace datatools {
  namespace detail {
    namespace reflection {

      DR_TYPE_IMPLEMENT_REFLECTION_HEAD(::datatools::introspection::data_type, tag_  __attribute__((unused)) )
      {
        DR_ENUM_DECLARE(::datatools::introspection::data_type)
          .value("invalid", ::datatools::introspection::DATA_TYPE_INVALID)
          .value("void",    ::datatools::introspection::DATA_TYPE_VOID)
          .value("boolean", ::datatools::introspection::DATA_TYPE_BOOLEAN)
          .value("int8",    ::datatools::introspection::DATA_TYPE_INT8)
          .value("uint8",   ::datatools::introspection::DATA_TYPE_UINT8)
          .value("int16",   ::datatools::introspection::DATA_TYPE_INT16)
          .value("uint16",  ::datatools::introspection::DATA_TYPE_UINT16)
          .value("int32",   ::datatools::introspection::DATA_TYPE_INT32)
          .value("uint32",  ::datatools::introspection::DATA_TYPE_UINT32)
          .value("int64",   ::datatools::introspection::DATA_TYPE_INT64)
          .value("uint64",  ::datatools::introspection::DATA_TYPE_UINT64)
          .value("float",   ::datatools::introspection::DATA_TYPE_FLOAT)
          .value("double",  ::datatools::introspection::DATA_TYPE_DOUBLE)
          .value("string",  ::datatools::introspection::DATA_TYPE_STRING)
          .value("path",    ::datatools::introspection::DATA_TYPE_PATH)
          .value("enum",    ::datatools::introspection::DATA_TYPE_ENUM)
          .value("user",    ::datatools::introspection::DATA_TYPE_USER)
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
