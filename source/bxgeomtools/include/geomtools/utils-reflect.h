// -*- mode: c++; -*-
//! \file geomtools/utils-reflect.h
#ifndef GEOMTOOLS_UTILS_REFLECT_H_
#define GEOMTOOLS_UTILS_REFLECT_H_

// Standard Library

// Datatools
#include <datatools/reflection_macros.h>

// Geomtools
#include <geomtools/utils.h>

// Export the introspection code for the 'geomtools::vector_3d' class:
DR_CLASS_EXPORT (::geomtools::vector_3d, "geomtools::vector_3d")
DR_CLASS_EXPORT (::geomtools::vector_2d, "geomtools::vector_2d")

// Mandatory namespace is datatools::detail::reflection :
namespace datatools {
  namespace detail {
    namespace reflection {

      DR_CLASS_IMPLEMENT_REFLECTION_HEAD(::geomtools::vector_2d, tag_)
      {
        // Declare and populate the reflection metaclass for the 'geomtools::vector_2d' class:
        DR_CLASS_DECLARE (::geomtools::vector_2d)
          .DR_CLASS_CONSTRUCTOR()

          ;
        return;
      }

      DR_CLASS_IMPLEMENT_REFLECTION_HEAD(::geomtools::vector_3d, tag_)
      {
        // Declare and populate the reflection metaclass for the 'geomtools::vector_3d' class:
        DR_CLASS_DECLARE (::geomtools::vector_3d)
          .DR_CLASS_CONSTRUCTOR()
          .DR_CLASS_CONSTRUCTOR1(double)
          .DR_CLASS_CONSTRUCTOR2(double,double)
          .DR_CLASS_CONSTRUCTOR3(double,double,double)
          .DR_CLASS_PROPERTY_GETTER_SETTER("x",
                                           ::geomtools::vector_3d::x,
                                           ::geomtools::vector_3d::setX)
          .DR_CLASS_PROPERTY_GETTER_SETTER("y",
                                           ::geomtools::vector_3d::y,
                                           ::geomtools::vector_3d::setY)
          .DR_CLASS_PROPERTY_GETTER_SETTER("z",
                                           ::geomtools::vector_3d::z,
                                           ::geomtools::vector_3d::setZ)
          .DR_CLASS_METHOD_CONST("mag",
                                 ::geomtools::vector_3d,
                                 ::geomtools::vector_3d::mag,
                                 double)
          .DR_CLASS_METHOD_CONST("mag2",
                                 ::geomtools::vector_3d,
                                 ::geomtools::vector_3d::mag,
                                 double)
          .DR_CLASS_METHOD_CONST("theta",
                                 ::geomtools::vector_3d,
                                 ::geomtools::vector_3d::theta,
                                 double)
          .DR_CLASS_METHOD_CONST("phi",
                                 ::geomtools::vector_3d,
                                 ::geomtools::vector_3d::phi,
                                 double)
          .DR_CLASS_METHOD_CONST("cosTheta",
                                 ::geomtools::vector_3d,
                                 ::geomtools::vector_3d::cosTheta,
                                 double)
          .DR_CLASS_METHOD_CONST("cos2Theta",
                                 ::geomtools::vector_3d,
                                 ::geomtools::vector_3d::cos2Theta,
                                 double)
          .DR_CLASS_METHOD_CONST("perp",
                                 ::geomtools::vector_3d,
                                 ::geomtools::vector_3d::perp,
                                 double)
          .DR_CLASS_METHOD_CONST("perp2",
                                 ::geomtools::vector_3d,
                                 ::geomtools::vector_3d::perp2,
                                 double)

          ;
        return;
      }

    } // end of namespace reflection
  } // end of namespace detail
} // end of namespace datatools

#endif // GEOMTOOLS_UTILS_REFLECT_H_
