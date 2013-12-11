// -*- mode: c++; -*-
//! \file geomtools/blur_spot-reflect.h
#ifndef GEOMTOOLS_BLUR_SPOT_REFLECT_H_
#define GEOMTOOLS_BLUR_SPOT_REFLECT_H_

// Standard Library

// Datatools
//#include <datatools/reflection_macros.h>

// Geomtools
#include <geomtools/blur_spot.h>

// Export the introspection code for the 'geomtools::blur_spot' class:
DR_CLASS_EXPORT (::geomtools::blur_spot, "geomtools::blur_spot")

// Mandatory namespace is datatools::detail::reflection :
namespace datatools {
  namespace detail {
    namespace reflection {

      DR_CLASS_IMPLEMENT_REFLECTION_HEAD(::geomtools::blur_spot, /*tag_*/)
      {
// 				std::cerr << "****** DEVEL ****** datatools::detail::reflection: "
// 									<< "geomtools::blur_spot" << std::endl;

        DR_CLASS_DECLARE (::geomtools::blur_spot)
          .DR_CLASS_BASE(::datatools::i_serializable)
          .DR_CLASS_BASE(::datatools::i_tree_dumpable)
          .DR_CLASS_CONSTRUCTOR()
          .DR_CLASS_CONSTRUCTOR2(uint8_t, double)

          .DR_CLASS_PROPERTY_GETTER_SETTER("x_error",
                                           ::geomtools::blur_spot::get_x_error,
                                           ::geomtools::blur_spot::set_x_error)
          .DR_CLASS_PROPERTY_GETTER_SETTER("y_error",
                                           ::geomtools::blur_spot::get_y_error,
                                           ::geomtools::blur_spot::set_y_error)
          .DR_CLASS_PROPERTY_GETTER_SETTER("z_error",
                                           ::geomtools::blur_spot::get_z_error,
                                           ::geomtools::blur_spot::set_z_error)
          .DR_CLASS_PROPERTY_GETTER_SETTER("tolerance",
                                           ::geomtools::blur_spot::get_tolerance,
                                           ::geomtools::blur_spot::set_tolerance)
          .DR_CLASS_PROPERTY_GETTER_SETTER("blur_dimension",
                                           ::geomtools::blur_spot::get_blur_dimension,
                                           ::geomtools::blur_spot::set_blur_dimension)
          .DR_CLASS_PROPERTY_GETTER_SETTER("placement",
                                           ::geomtools::blur_spot::get_placement,
                                           ::geomtools::blur_spot::set_placement)
          .DR_CLASS_PROPERTY_GETTER_SETTER("position",
                                           ::geomtools::blur_spot::get_position,
                                           ::geomtools::blur_spot::set_position)
          .DR_CLASS_PROPERTY_GETTER_SETTER("auxiliaries",
                                           ::geomtools::blur_spot::get_auxiliaries,
                                           ::geomtools::blur_spot::set_auxiliaries)
          .DR_CLASS_PROPERTY_GETTER("rotation",
                                    ::geomtools::blur_spot::get_rotation)
          .DR_CLASS_PROPERTY_GETTER("inverse_rotation",
                                    ::geomtools::blur_spot::get_inverse_rotation)

          .DR_CLASS_METHOD_CONST("is_dimension_zero",
                                  ::geomtools::blur_spot,
                                  ::geomtools::blur_spot::is_dimension_zero,
                                  bool)
          .DR_CLASS_METHOD_CONST("is_dimension_one",
                                  ::geomtools::blur_spot,
                                  ::geomtools::blur_spot::is_dimension_one,
                                  bool)
          .DR_CLASS_METHOD_CONST("is_dimension_two",
                                  ::geomtools::blur_spot,
                                  ::geomtools::blur_spot::is_dimension_two,
                                  bool)
          .DR_CLASS_METHOD_CONST("is_dimension_three",
                                  ::geomtools::blur_spot,
                                  ::geomtools::blur_spot::is_dimension_three,
                                  bool)
          .DR_CLASS_METHOD_CONST("is_valid",
                                  ::geomtools::blur_spot,
                                  ::geomtools::blur_spot::is_valid,
                                  bool)
          .DR_CLASS_VOID_METHOD3("set_errors",
                                 ::geomtools::blur_spot,
                                 ::geomtools::blur_spot::set_errors,
                                 double, double, double)
          .DR_CLASS_VOID_METHOD("invalidate",
                                ::geomtools::blur_spot,
                                ::geomtools::blur_spot::invalidate)
          .DR_CLASS_VOID_METHOD("reset",
                                ::geomtools::blur_spot,
                                ::geomtools::blur_spot::reset)
          // .DR_CLASS_METHOD2_CONST("match",
          //                         ::geomtools::blur_spot,
          //                         ::geomtools::blur_spot::match_impl,
          //                         bool,
          //                         const ::geomtools::blur_spot &,
          //                         bool)

          ;
        return;
      }

    } // end of namespace reflection
  } // end of namespace detail
} // end of namespace datatools

#endif // GEOMTOOLS_BLUR_SPOT_REFLECT_H_
