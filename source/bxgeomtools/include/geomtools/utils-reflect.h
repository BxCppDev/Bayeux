/// \file geomtools/utils-reflect.h

#ifndef GEOMTOOLS_UTILS_REFLECT_H
#define GEOMTOOLS_UTILS_REFLECT_H

// Ourselves:
#include <geomtools/utils.h>

// This project:
#include <datatools/detail/reflection_macros.h>
#include <datatools/detail/reflection_export.h>
#include <datatools/compiler_macros.h>

// Export the introspection code for geomtools enums:
DR_TYPE_EXPORT(::geomtools::orientation_type, "geomtools::orientation_type")
DR_TYPE_EXPORT(::geomtools::direction_type, "geomtools::direction_type")
DR_TYPE_EXPORT(::geomtools::axis_type, "geomtools::axis_type")
DR_TYPE_EXPORT(::geomtools::rotation_axis_type, "geomtools::rotation_axis_type")

namespace datatools {
  namespace detail {
    namespace reflection {

      DR_TYPE_IMPLEMENT_REFLECTION_HEAD(::geomtools::orientation_type, DT_UNUSED(tag_) )
      {
        DR_ENUM_DECLARE(::geomtools::orientation_type)
          .value("vertical",   geomtools::VERTICAL)
          .value("horizontal", geomtools::HORIZONTAL)
          ;
        return;
      }

      DR_TYPE_IMPLEMENT_REFLECTION_HEAD(::geomtools::direction_type, DT_UNUSED(tag_) )
      {
        DR_ENUM_DECLARE(::geomtools::direction_type)
          .value("back",   geomtools::DIRECTION_BACK)
          .value("front",  geomtools::DIRECTION_FRONT)
          .value("left",   geomtools::DIRECTION_LEFT)
          .value("right",  geomtools::DIRECTION_RIGHT)
          .value("bottom", geomtools::DIRECTION_BOTTOM)
          .value("top",    geomtools::DIRECTION_TOP)
          ;
        return;
      }

      DR_TYPE_IMPLEMENT_REFLECTION_HEAD(::geomtools::axis_type, DT_UNUSED(tag_) )
      {
        DR_ENUM_DECLARE(::geomtools::axis_type)
          .value("invalid", geomtools::AXIS_INVALID)
          .value("x",       geomtools::AXIS_X)
          .value("y",       geomtools::AXIS_Y)
          .value("z",       geomtools::AXIS_Z)
          ;
        return;
      }

      DR_TYPE_IMPLEMENT_REFLECTION_HEAD(::geomtools::rotation_axis_type, DT_UNUSED(tag_) )
      {
        DR_ENUM_DECLARE(::geomtools::rotation_axis_type)
          .value("invalid", geomtools::ROTATION_AXIS_INVALID)
          .value("x",       geomtools::ROTATION_AXIS_X)
          .value("y",       geomtools::ROTATION_AXIS_Y)
          .value("z",       geomtools::ROTATION_AXIS_Z)
          ;
        return;
      }

    } // end of namespace reflection
  } // end of namespace detail
} // end of namespace datatools

#endif // GEOMTOOLS_UTILS_REFLECT_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
