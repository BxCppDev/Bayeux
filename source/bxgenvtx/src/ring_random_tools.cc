/* ring_random_tools.cc
 */

// Ourselves
#include <genvtx/ring_random_tools.h>

// Third party
// - GSL
#include <gsl/gsl_math.h>
// - datatools
#include <datatools/exception.h>
// - geomtools
#include <geomtools/utils.h>

namespace genvtx {

  geomtools::vector_3d
  ring_random_surface (mygsl::rng & random_,
                       double outer_radius_,
                       double inner_radius_,
                       double thickness_,
                       double theta1_,
                       double theta2_)
  {
    double r, theta, z;
    DT_THROW_IF(inner_radius_ > outer_radius_,
                std::range_error,
                "Inner radius ('" << inner_radius_
                << "') is larger than outer radius  ('" << outer_radius_ << "') !");
    if (inner_radius_ < outer_radius_) {
      double r1_2 = gsl_pow_2(inner_radius_);
      double r2_2 = gsl_pow_2(outer_radius_);
      double r2 = random_.flat(r1_2, r2_2);
      r = std::sqrt(r2);
    } else {
      r = inner_radius_;
    }
    theta = random_.flat(theta1_, theta2_);
    if (thickness_ > 0.0) {
      double dz = 0.5 * thickness_;
      z = random_.flat(-dz, dz);
    } else {
      z = 0.0;
    }
    geomtools::vector_3d p;
    geomtools::create_polar(p, r, theta, z);
    return p;
  }

} // end of namespace genvtx
