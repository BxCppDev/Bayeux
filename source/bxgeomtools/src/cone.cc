// -*- mode: c++ ; -*-
/* cone.cc
 */

// Ourselves:
#include <geomtools/cone.h>

// Standard library
#include <cmath>
#include <iostream>

// Third-party
// - GSL
#include <gsl/gsl_math.h>

// - datatools
#include <datatools/exception.h>

namespace geomtools {

  // static
  double cone::compute_slice_lateral_surface(double z1_, double z2_,
                                             double r1_, double r2_)
  {
    double s = 0.0;
    double z1 = z1_;
    double z2 = z2_;
    double r1 = r1_;
    double r2 = r2_;
    DT_THROW_IF(z1 == z2, std::logic_error,
                "Invalid cone Z dimensions !");
    if (r1 == r2) {
      return 2 * M_PI * r1 * (z2 - z1);
    }
    double h1 = r1 * (z1 - z2) / (r2 - r1);
    double h2 = h1 - (z2 - z1);
    // std::cerr << "*** DEVEL *** h1 = " << h1 << std::endl;
    // std::cerr << "*** DEVEL *** h2 = " << h2 << std::endl;
    double s1 = M_PI * r1 * r1 * std::sqrt(1 + gsl_pow_2(h1/r1));
    double s2 = M_PI * r2 * r2 * std::sqrt(1 + gsl_pow_2(h2/r2));
    // std::cerr << "*** DEVEL *** s1 = " << s1 << std::endl;
    // std::cerr << "*** DEVEL *** s2 = " << s2 << std::endl;
    s = s1 - s2;
    if (s < 0.0) s *= -1.0;
    return s;
  }

  // static
  double cone::compute_slice_volume(double z1_, double z2_,
                                    double r1_, double r2_)
  {
    double v = 0.0;
    double z1 = z1_;
    double z2 = z2_;
    double r1 = r1_;
    double r2 = r2_;
    DT_THROW_IF(z1 == z2, std::logic_error,
                "Invalid cone Z dimensions !");
    if (r1 == r2) {
      return M_PI * r1 * r1 * (z2 - z1);
    }
    double h1 = r1 * (z1 - z2) / (r2 - r1);
    double h2 = h1 - (z2 - z1);
    double v1 = M_PI * r1 * r1 * h1 / 3.0;
    double v2 = M_PI * r2 * r2 * h2 / 3.0;
    // std::cerr << "*** DEVEL *** v1 = " << v1 << std::endl;
    // std::cerr << "*** DEVEL *** v2 = " << v2 << std::endl;
    v = v1 - v2;
    if (v < 0.0) v *= -1.0;
    return v;
  }

} // end of namespace geomtools

  // end of cone.cc
