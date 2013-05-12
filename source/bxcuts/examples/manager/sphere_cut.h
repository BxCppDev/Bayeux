// -*- mode: c++ ; -*-
/* sphere_cut.h
 * Author (s) : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2013-05-10
 * Last modified: 2013-05-10
 */

/**
 * Description:
 *   A cut to test the position of a 3D-point
 *   relatively to a sphere.
 *
 */

#ifndef SPHERE_CUT_H_
#define SPHERE_CUT_H_ 1

#include <iostream>

#include <cuts/i_cut.h>
#include <cuts/cut_macros.h>

#include <data.h>

/// \brief A cut to test the position of a 3D-point relatively to a sphere.
CUT_CLASS_DECLARE(sphere_cut)
{
 public:

  CUT_INTERFACE_CTOR_DTOR(sphere_cut);

 protected:

  void set_radius(double a_radius);

  void set_center(double a_x0, double a_y0, double a_z0);

  void set_reversed(bool a_reversed);

 private:

  double _x0_;
  double _y0_;
  double _z0_;
  double _radius_;
  bool   _reversed_;

  // Macro to automate the registration of the cut :
  CUT_REGISTRATION_INTERFACE(sphere_cut);

};

#endif // SPHERE_CUT_H_

// end of sphere_cut.h
