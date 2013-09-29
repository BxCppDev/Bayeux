// -*- mode: c++; -*-
//! \file geomtools/utils-reflect.h
#ifndef GEOMTOOLS_UTILS_REFLECT_H_
#define GEOMTOOLS_UTILS_REFLECT_H_

// Standard Library

// Geomtools
#include <geomtools/utils.h>

// Export the introspection code for the 'geomtools::XXX' enums:
DR_TYPE_EXPORT(::geomtools::orientation_type, "geomtools::orientation_type")
DR_TYPE_EXPORT(::geomtools::direction_type, "geomtools::direction_type")
DR_TYPE_EXPORT(::geomtools::axis_type, "geomtools::axis_type")
DR_TYPE_EXPORT(::geomtools::rotation_axis_type, "geomtools::rotation_axis_type")

namespace datatools {
  namespace detail {
    namespace reflection {

      DR_TYPE_IMPLEMENT_REFLECTION_HEAD(::geomtools::orientation_type, tag_)
      {
        DR_ENUM_DECLARE(::geomtools::orientation_type)
          .DR_ENUM_PAIR("vertical",   geomtools::VERTICAL)
          .DR_ENUM_PAIR("horizontal", geomtools::HORIZONTAL)
          ;
        return;
      }

      DR_TYPE_IMPLEMENT_REFLECTION_HEAD(::geomtools::direction_type, tag_)
      {
        DR_ENUM_DECLARE(::geomtools::direction_type)
          .DR_ENUM_PAIR("back",   geomtools::BACK)
          .DR_ENUM_PAIR("front",  geomtools::FRONT)
          .DR_ENUM_PAIR("left",   geomtools::LEFT)
          .DR_ENUM_PAIR("right",  geomtools::RIGHT)
          .DR_ENUM_PAIR("bottom", geomtools::BOTTOM)
          .DR_ENUM_PAIR("top",    geomtools::TOP)
          ;
        return;
      }

      DR_TYPE_IMPLEMENT_REFLECTION_HEAD(::geomtools::axis_type, tag_)
      {
        DR_ENUM_DECLARE(::geomtools::axis_type)
          .DR_ENUM_PAIR("invalid", geomtools::AXIS_INVALID)
          .DR_ENUM_PAIR("x",       geomtools::AXIS_X)
          .DR_ENUM_PAIR("y",       geomtools::AXIS_Y)
          .DR_ENUM_PAIR("z",       geomtools::AXIS_Z)
          ;
        return;
      }

      DR_TYPE_IMPLEMENT_REFLECTION_HEAD(::geomtools::rotation_axis_type, tag_)
      {
        DR_ENUM_DECLARE(::geomtools::rotation_axis_type)
          .DR_ENUM_PAIR("invalid", geomtools::ROTATION_AXIS_INVALID)
          .DR_ENUM_PAIR("x",       geomtools::ROTATION_AXIS_X)
          .DR_ENUM_PAIR("y",       geomtools::ROTATION_AXIS_Y)
          .DR_ENUM_PAIR("z",       geomtools::ROTATION_AXIS_Z)
          ;
        return;
      }

    } // end of namespace reflection
  } // end of namespace detail
} // end of namespace datatools

#endif // GEOMTOOLS_UTILS_REFLECT_H_
