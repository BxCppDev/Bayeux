// cone.cc

// Ourselves:
#include <geomtools/cone.h>

// Standard library:
#include <cmath>
#include <iostream>

// Third-party:
// - GSL:
#include <gsl/gsl_math.h>
// - Bayeux/datatools:
#include <datatools/exception.h>
#include <datatools/units.h>
// - Bayeux/mygsl:
#include <mygsl/polynomial.h>

// // This project:
// #include <geomtools/circle.h>
// #include <geomtools/disk.h>
// #include <geomtools/line_3d.h>
// #include <geomtools/quadrangle.h>
// #include <geomtools/triangle.h>

namespace geomtools {

  // static
  double cone::compute_frustrum_lateral_surface(double z1_, double z2_,
                                                double r1_, double r2_)
  {
    // See: http://en.wikipedia.org/wiki/Frustum#Surface_Area
    double s = 0.0;
    double z1 = z1_;
    double z2 = z2_;
    double r1 = r1_;
    double r2 = r2_;
    DT_THROW_IF(z1 == z2, std::logic_error,
                "Invalid cone Z dimensions !");
    double h = z2 - z1;
    if (r1 == r2) {
      return 2 * M_PI * r1 * h;
    }
    s = M_PI * (r1 + r2) * std::sqrt(gsl_pow_2(r1-r2)+h*h);
    if (s < 0.0) s *= -1.0;
    return s;
  }

  // static
  double cone::compute_frustrum_volume(double z1_, double z2_,
                                       double r1_, double r2_)
  {
    // See: http://en.wikipedia.org/wiki/Frustum#Volume
    double v = 0.0;
    double z1 = z1_;
    double z2 = z2_;
    double r1 = r1_;
    double r2 = r2_;
    DT_THROW_IF(z1 == z2, std::logic_error,
                "Invalid cone Z dimensions !");
    double h = std::abs(z2 - z1);
    if (r1 == r2) {
      return M_PI * r1 * r1 * h;
    }
    v = M_PI * h * (r1 * r1 + r1 * r2 + r2 * r2) / 3.0;
    return v;
  }

} // end of namespace geomtools
