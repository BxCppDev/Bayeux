#include <iostream>
#include <cmath>
#include <stdexcept>
#include <gsl/gsl_sf.h>
#include <CLHEP/Units/SystemOfUnits.h>

#ifndef __fermi_h
#define __fermi_h

extern "C"
{
  double a_from_z (double z_);

  // Non-relativistic approximation of the Fermi function:
  float fermi_func_nr_approx (float z_, float e_);

  float fermi_func_shape_only (float z_, float e_);
  
  float fermi_func (float z_, float e_, bool use_l0_ = false);

  float fermi_wrap (float * z_, float * e_);

}

#endif

// end
