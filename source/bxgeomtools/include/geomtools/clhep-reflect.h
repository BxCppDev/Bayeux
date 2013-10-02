// -*- mode: c++; -*-
//! \file geomtools/clhep-reflect.h
#ifndef GEOMTOOLS_CLHEP_REFLECT_H_
#define GEOMTOOLS_CLHEP_REFLECT_H_

// Standard Library

// Datatools
#include <datatools/reflection_macros.h>

// Geomtools
#include <geomtools/clhep.h>

// Export the introspection code for the CLHEP class:
DR_CLASS_EXPORT (::geomtools::vector_2d, "geomtools::vector_2d")
DR_CLASS_EXPORT (::geomtools::vector_3d, "geomtools::vector_3d")
DR_CLASS_EXPORT (::geomtools::rotation_3d, "geomtools::rotation_3d")

// Mandatory namespace is datatools::detail::reflection :
namespace datatools {
  namespace detail {
    namespace reflection {

      DR_CLASS_IMPLEMENT_REFLECTION_HEAD(::geomtools::vector_2d, tag_)
      {
        // std::cerr << "****** DEVEL ****** datatools::detail::reflection: "
        //           << "geomtools::vector_2d" << std::endl;

        // Declare and populate the reflection metaclass for the 'geomtools::vector_2d' class:
        DR_CLASS_DECLARE (::geomtools::vector_2d)
          .DR_CLASS_CONSTRUCTOR()
          .DR_CLASS_CONSTRUCTOR1(double)
          .DR_CLASS_CONSTRUCTOR2(double,double)
          .DR_CLASS_PROPERTY_GETTER_SETTER("x",
                                           ::geomtools::vector_2d::x,
                                           ::geomtools::vector_2d::setX)
          .DR_CLASS_PROPERTY_GETTER_SETTER("y",
                                           ::geomtools::vector_2d::y,
                                           ::geomtools::vector_2d::setY)
          .DR_CLASS_PROPERTY_GETTER_SETTER("mag",
                                           ::geomtools::vector_2d::mag,
                                           ::geomtools::vector_2d::setMag)
          .DR_CLASS_PROPERTY_GETTER_SETTER("phi",
                                           ::geomtools::vector_2d::phi,
                                           ::geomtools::vector_2d::setPhi)
          .DR_CLASS_PROPERTY_GETTER_SETTER("r",
                                           ::geomtools::vector_2d::r,
                                           ::geomtools::vector_2d::setR)
          .DR_CLASS_VOID_METHOD2("set",
                                 ::geomtools::vector_2d,
                                 ::geomtools::vector_2d::set,
                                 double, double)
          .DR_CLASS_VOID_METHOD2("setPolar",
                                 ::geomtools::vector_2d,
                                 ::geomtools::vector_2d::setPolar,
                                 double, double)
          .DR_CLASS_METHOD_CONST("mag2",
                                 ::geomtools::vector_2d,
                                 ::geomtools::vector_2d::mag2,
                                 double)
          .DR_CLASS_METHOD1_CONST("howNear",
                                  ::geomtools::vector_2d,
                                  ::geomtools::vector_2d::howNear,
                                  double,
                                  const ::geomtools::vector_2d &)
          .DR_CLASS_METHOD2_CONST("isNear",
                                  ::geomtools::vector_2d,
                                  ::geomtools::vector_2d::isNear,
                                  bool,
                                  const ::geomtools::vector_2d &, double)
          .DR_CLASS_METHOD1_CONST("howParallel",
                                  ::geomtools::vector_2d,
                                  ::geomtools::vector_2d::howParallel,
                                  double,
                                  const ::geomtools::vector_2d &)
          .DR_CLASS_METHOD2_CONST("isParallel",
                                  ::geomtools::vector_2d,
                                  ::geomtools::vector_2d::isParallel,
                                  bool,
                                  const ::geomtools::vector_2d &, double)
          .DR_CLASS_METHOD1_CONST("howOrthogonal",
                                  ::geomtools::vector_2d,
                                  ::geomtools::vector_2d::howOrthogonal,
                                  double,
                                  const ::geomtools::vector_2d &)
          .DR_CLASS_METHOD2_CONST("isOrthogonal",
                                  ::geomtools::vector_2d,
                                  ::geomtools::vector_2d::isOrthogonal,
                                  bool,
                                  const ::geomtools::vector_2d &, double)
          .DR_CLASS_METHOD_CONST("unit",
                                 ::geomtools::vector_2d,
                                 ::geomtools::vector_2d::unit,
                                 ::geomtools::vector_2d)
          .DR_CLASS_METHOD_CONST("orthogonal",
                                 ::geomtools::vector_2d,
                                 ::geomtools::vector_2d::orthogonal,
                                 ::geomtools::vector_2d)
          .DR_CLASS_METHOD1_CONST("dot",
                                  ::geomtools::vector_2d,
                                  ::geomtools::vector_2d::dot,
                                  double,
                                  ::geomtools::vector_2d)
          .DR_CLASS_METHOD1_CONST("angle",
                                  ::geomtools::vector_2d,
                                  ::geomtools::vector_2d::angle,
                                  double,
                                  ::geomtools::vector_2d)
          .DR_CLASS_VOID_METHOD1("rotate",
                                 ::geomtools::vector_2d,
                                 ::geomtools::vector_2d::rotate,
                                 double)

          ;
        return;
      }

      DR_CLASS_IMPLEMENT_REFLECTION_HEAD(::geomtools::vector_3d, tag_)
      {
        // std::cerr << "****** DEVEL ****** datatools::detail::reflection: "
        //        << "geomtools::vector_3d" << std::endl;

        // Unambiguous overloaded methods addressing :
        typedef double (::geomtools::vector_3d::*theta_1_method_type)() const;
        theta_1_method_type theta_1_method = &::geomtools::vector_3d::theta;
        //std::cerr << "******** DEVEL: theta_1_method=" << &theta_1_method << '\n';

        typedef double (::geomtools::vector_3d::*theta_2_method_type)(const geomtools::vector_3d &) const;
        theta_2_method_type theta_2_method = &::geomtools::vector_3d::theta;
        //std::cerr << "******** DEVEL: theta_2_method=" << &theta_2_method << '\n';

        typedef double (::geomtools::vector_3d::*perp_1_method_type)() const;
        perp_1_method_type perp_1_method = &::geomtools::vector_3d::perp;

        typedef double (::geomtools::vector_3d::*perp_2_method_type)(const geomtools::vector_3d &) const;
        perp_2_method_type perp_2_method = &::geomtools::vector_3d::perp;

        typedef double (::geomtools::vector_3d::*perp2_1_method_type)() const;
        perp2_1_method_type perp2_1_method = &::geomtools::vector_3d::perp2;

        typedef double (::geomtools::vector_3d::*perp2_2_method_type)(const geomtools::vector_3d &) const;
        perp2_2_method_type perp2_2_method = &::geomtools::vector_3d::perp2;

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
          .DR_CLASS_PROPERTY_GETTER_SETTER("mag",
                                           ::geomtools::vector_3d::mag,
                                           ::geomtools::vector_3d::setMag)

          .DR_CLASS_PROPERTY_REFGETTER_REFSETTER("theta",
                                           theta_1_method, // ::geomtools::vector_3d::theta
                                           &::geomtools::vector_3d::setTheta)

          .DR_CLASS_PROPERTY_GETTER_SETTER("phi",
                                           ::geomtools::vector_3d::phi,
                                           ::geomtools::vector_3d::setPhi)

          .DR_CLASS_PROPERTY_GETTER_SETTER("rho",
                                           ::geomtools::vector_3d::rho,
                                           ::geomtools::vector_3d::setRho)

          .DR_CLASS_PROPERTY_REFGETTER_REFSETTER("perp",
                                           perp_1_method, // ::geomtools::vector_3d::perp
                                           &::geomtools::vector_3d::setPerp)

          .DR_CLASS_VOID_METHOD3("set",
                                 ::geomtools::vector_3d,
                                 ::geomtools::vector_3d::set,
                                 double, double, double)
          .DR_CLASS_METHOD_CONST("mag2",
                                 ::geomtools::vector_3d,
                                 ::geomtools::vector_3d::mag2,
                                 double)
          .DR_CLASS_METHOD_CONST("cosTheta",
                                 ::geomtools::vector_3d,
                                 ::geomtools::vector_3d::cosTheta,
                                 double)
          .DR_CLASS_METHOD_CONST("cos2Theta",
                                 ::geomtools::vector_3d,
                                 ::geomtools::vector_3d::cos2Theta,
                                 double)
          .DR_CLASS_METHOD_CONST("rho",
                                 ::geomtools::vector_3d,
                                 ::geomtools::vector_3d::rho,
                                 double)
          .DR_CLASS_REFMETHOD_CONST("perp2",
                                 ::geomtools::vector_3d,
                                 perp2_1_method,
                                 double)

          .DR_CLASS_METHOD1_CONST("howNear",
                                  ::geomtools::vector_3d,
                                  ::geomtools::vector_3d::howNear,
                                  double,
                                  const ::geomtools::vector_3d &)
          .DR_CLASS_METHOD2_CONST("isNear",
                                  ::geomtools::vector_3d,
                                  ::geomtools::vector_3d::isNear,
                                  bool,
                                  const ::geomtools::vector_3d &, double)
          .DR_CLASS_METHOD1_CONST("howParallel",
                                  ::geomtools::vector_3d,
                                  ::geomtools::vector_3d::howParallel,
                                  double,
                                  const ::geomtools::vector_3d &)
          .DR_CLASS_METHOD2_CONST("isParallel",
                                  ::geomtools::vector_3d,
                                  ::geomtools::vector_3d::isParallel,
                                  bool,
                                  const ::geomtools::vector_3d &, double)
          .DR_CLASS_METHOD1_CONST("howOrthogonal",
                                  ::geomtools::vector_3d,
                                  ::geomtools::vector_3d::howOrthogonal,
                                  double,
                                  const ::geomtools::vector_3d &)
          .DR_CLASS_METHOD2_CONST("isOrthogonal",
                                  ::geomtools::vector_3d,
                                  ::geomtools::vector_3d::isOrthogonal,
                                  bool,
                                  const ::geomtools::vector_3d &, double)
          .DR_CLASS_METHOD1_CONST("dot",
                                  ::geomtools::vector_3d,
                                  ::geomtools::vector_3d::dot,
                                  double,
                                  const ::geomtools::vector_3d &)
          .DR_CLASS_METHOD1_CONST("angle",
                                  ::geomtools::vector_3d,
                                  ::geomtools::vector_3d::angle,
                                  double,
                                  const ::geomtools::vector_3d &)
          .DR_CLASS_METHOD1_CONST("cross",
                                  ::geomtools::vector_3d,
                                  ::geomtools::vector_3d::cross,
                                  ::geomtools::vector_3d,
                                  const ::geomtools::vector_3d &)
          .DR_CLASS_METHOD1_CONST("diff2",
                                  ::geomtools::vector_3d,
                                  ::geomtools::vector_3d::diff2,
                                  double,
                                  const ::geomtools::vector_3d &)
          .DR_CLASS_METHOD1_CONST("compare",
                                  ::geomtools::vector_3d,
                                  ::geomtools::vector_3d::compare,
                                  int,
                                  const ::geomtools::vector_3d &)
          .DR_CLASS_METHOD_CONST("unit",
                                 ::geomtools::vector_3d,
                                 ::geomtools::vector_3d::unit,
                                 ::geomtools::vector_3d)
          .DR_CLASS_METHOD_CONST("orthogonal",
                                 ::geomtools::vector_3d,
                                 ::geomtools::vector_3d::orthogonal,
                                 ::geomtools::vector_3d)
          ;
        return;
      }

      DR_CLASS_IMPLEMENT_REFLECTION_HEAD(::geomtools::rotation_3d, tag_)
      {
        // Declare and populate the reflection metaclass for the 'geomtools::rotation_3d' class:
        DR_CLASS_DECLARE (::geomtools::rotation_3d)
          .DR_CLASS_CONSTRUCTOR()
          .DR_CLASS_PROPERTY_GETTER("xx", ::geomtools::rotation_3d::xx)
          .DR_CLASS_PROPERTY_GETTER("xy", ::geomtools::rotation_3d::xy)
          .DR_CLASS_PROPERTY_GETTER("xz", ::geomtools::rotation_3d::xz)
          .DR_CLASS_PROPERTY_GETTER("yx", ::geomtools::rotation_3d::yx)
          .DR_CLASS_PROPERTY_GETTER("yy", ::geomtools::rotation_3d::yy)
          .DR_CLASS_PROPERTY_GETTER("yz", ::geomtools::rotation_3d::yz)
          .DR_CLASS_PROPERTY_GETTER("zx", ::geomtools::rotation_3d::zx)
          .DR_CLASS_PROPERTY_GETTER("zy", ::geomtools::rotation_3d::zy)
          .DR_CLASS_PROPERTY_GETTER("zz", ::geomtools::rotation_3d::zz)
          .DR_CLASS_PROPERTY_GETTER("colX", ::geomtools::rotation_3d::colX)
          .DR_CLASS_PROPERTY_GETTER("colY", ::geomtools::rotation_3d::colY)
          .DR_CLASS_PROPERTY_GETTER("colZ", ::geomtools::rotation_3d::colZ)
          .DR_CLASS_PROPERTY_GETTER("rowX", ::geomtools::rotation_3d::rowX)
          .DR_CLASS_PROPERTY_GETTER("rowY", ::geomtools::rotation_3d::rowY)
          .DR_CLASS_PROPERTY_GETTER("rowZ", ::geomtools::rotation_3d::rowZ)
          ;

        return;
      }


    } // end of namespace reflection
  } // end of namespace detail
} // end of namespace datatools

#endif // GEOMTOOLS_CLHEP_REFLECT_H_
