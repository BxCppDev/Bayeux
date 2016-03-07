/// \file geomtools/placement-reflect.h

#ifndef GEOMTOOLS_PLACEMENT_REFLECT_H
#define GEOMTOOLS_PLACEMENT_REFLECT_H

// Third party:
// - Bayeux/datatools:
#include <datatools/detail/reflection_macros.h>
#include <datatools/detail/reflection_export.h>

// This project:
#include <geomtools/placement.h>

// Export the introspection code for the 'geomtools::placement' class:
DR_CLASS_EXPORT (::geomtools::placement, "geomtools::placement")

// Mandatory namespace is datatools::detail::reflection :
namespace datatools {
  namespace detail {
    namespace reflection {

      DR_CLASS_IMPLEMENT_REFLECTION_HEAD(::geomtools::placement, tag_ __attribute__((unused)) )
        {
          // Declare and populate the reflection metaclass for the 'geomtools::placement' class:
          DR_CLASS_DECLARE (::geomtools::placement)
            .DR_CLASS_BASE(::geomtools::i_placement)
            .DR_CLASS_CONSTRUCTOR()
            .DR_CLASS_CONSTRUCTOR3(::geomtools::vector_3d, int, double)
            .DR_CLASS_CONSTRUCTOR4(::geomtools::vector_3d, double, double, double)

            .DR_CLASS_PROPERTY_GETTER_SETTER("translation",
                                             ::geomtools::placement::get_translation,
                                             ::geomtools::placement::set_translation_v)


            .DR_CLASS_PROPERTY_GETTER("rotation", ::geomtools::placement::get_rotation)
            .DR_CLASS_PROPERTY_GETTER("inverse_rotation", ::geomtools::placement::get_inverse_rotation)
            .DR_CLASS_PROPERTY_GETTER("phi", ::geomtools::placement::get_phi)
            .DR_CLASS_PROPERTY_GETTER("theta", ::geomtools::placement::get_theta)
            .DR_CLASS_PROPERTY_GETTER("delta", ::geomtools::placement::get_delta)


            .DR_CLASS_METHOD_CONST("is_valid",
                                   ::geomtools::placement,
                                   ::geomtools::placement::is_valid,
                                   bool)
            .DR_CLASS_VOID_METHOD("invalidate",
                                  ::geomtools::placement,
                                  ::geomtools::placement::invalidate)
            .DR_CLASS_VOID_METHOD3("set_translation",
                                   ::geomtools::placement,
                                   ::geomtools::placement::set_translation,
                                   double, double, double)
            .DR_CLASS_METHOD_CONST("is_simple_rotation",
                                   ::geomtools::placement,
                                   ::geomtools::placement::is_simple_rotation,
                                   bool)
            .DR_CLASS_METHOD_CONST("is_rotation_x",
                                   ::geomtools::placement,
                                   ::geomtools::placement::is_rotation_x,
                                   bool)
            .DR_CLASS_METHOD_CONST("is_rotation_y",
                                   ::geomtools::placement,
                                   ::geomtools::placement::is_rotation_y,
                                   bool)
            .DR_CLASS_METHOD_CONST("is_rotation_z",
                                   ::geomtools::placement,
                                   ::geomtools::placement::is_rotation_z,
                                   bool)
            .DR_CLASS_METHOD_CONST("get_rotation_axis",
                                   ::geomtools::placement,
                                   ::geomtools::placement::get_rotation_axis,
                                   int)
            .DR_CLASS_METHOD_CONST("get_rotation_angle",
                                   ::geomtools::placement,
                                   ::geomtools::placement::get_rotation_angle,
                                   double)
            .DR_CLASS_METHOD_CONST("is_zyz_rotation",
                                   ::geomtools::placement,
                                   ::geomtools::placement::is_zyz_rotation,
                                   bool)
            .DR_CLASS_METHOD_CONST("has_angles",
                                   ::geomtools::placement,
                                   ::geomtools::placement::has_angles,
                                   bool)
            .DR_CLASS_VOID_METHOD2("set_orientation_axis_angle",
                                   ::geomtools::placement,
                                   ::geomtools::placement::set_orientation_axis_angle,
                                   int, double)
            .DR_CLASS_VOID_METHOD3("set_orientation_zyz",
                                   ::geomtools::placement,
                                   ::geomtools::placement::set_orientation_zyz,
                                   double, double, double)

            ;
          return;
        }

    } // end of namespace reflection
  } // end of namespace detail
} // end of namespace datatools

#endif // GEOMTOOLS_PLACEMENT_REFLECT_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
