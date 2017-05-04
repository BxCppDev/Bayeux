/// \file genvtx/conical_frustrum_random_tools.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2012-09-26
 * Last modified: 2014-04-09
 *
 * License:
 *
 * Description:
 *
 *    Algorithms to randomize points on the surface or volume of a conical_frustrum
 *
 * History:
 *
 *
 */

#ifndef GENVTX_CONICAL_FRUSTRUM_RANDOM_TOOLS_H
#define GENVTX_CONICAL_FRUSTRUM_RANDOM_TOOLS_H 1

// Standard library
#include <cmath>

// Third party
// - geomtools
#include <geomtools/utils.h>
// - mygsl
#include <mygsl/rng.h>

namespace genvtx {

  struct right_circular_conical_frustrum {

    /// Randomize points from the surface of a right circular conical frustrum
    static
    geomtools::vector_3d
    random_surface (mygsl::rng & random_,
                    double radius1_,
                    double radius2_,
                    double length_,
                    double thickness_ = 0.0,
                    double theta1_ = 0.0,
                    double theta2_ = M_PI);

    /// Randomize points from the volume of a right circular conical frustrum
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

#endif // GENVTX_CONICAL_FRUSTRUM_RANDOM_TOOLS_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
