// -*- mode: c++ ; -*-
/* conical_frustrum_random_tools.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2012-09-26
 * Last modified: 2013-02-24
 *
 * License:
 *
 * Description:
 *
 *    Algorithm to randomize points on the surface of a conical_frustrum
 *
 * History:
 *
 *
 */

#ifndef GENVTX_CONICAL_FRUSTRUM_RANDOM_TOOLS_H_
#define GENVTX_CONICAL_FRUSTRUM_RANDOM_TOOLS_H_ 1

// Standard library
#include <cmath>

// Third party
// - geomtools
#include <geomtools/utils.h>
// - mygsl
#include <mygsl/rng.h>

namespace genvtx {

  struct right_circular_conical_frustrum {

    static
    geomtools::vector_3d
    random_surface (mygsl::rng & random_,
                    double radius1_,
                    double radius2_,
                    double length_,
                    double thickness_ = 0.0,
                    double theta1_ = 0.0,
                    double theta2_ = M_PI);

    static
    geomtools::vector_3d
    random_volume (mygsl::rng & random_,
                   double a1_,
                   double b1_,
                   double a2_,
                   double b2_,
                   double length_,
                   double thickness_ = 0.0,
                   double theta1_ = 0.0,
                   double theta2_ = M_PI);

  };

} // end of namespace genvtx

#endif // GENVTX_CONICAL_FRUSTRUM_RANDOM_TOOLS_H_

// end of conical_frustrum_random_tools.h
