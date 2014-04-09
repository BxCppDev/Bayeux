/// \file genvtx/ring_random_tools.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2012-09-26
 * Last modified: 2014-04-09
 *
 * License:
 *
 * Description:
 *
 *    Algorithm to randomize points on the surface of a ring
 *
 * History:
 *
 *
 */

#ifndef GENVTX_RING_RANDOM_TOOLS_H
#define GENVTX_RING_RANDOM_TOOLS_H 1

// Standard library
#include <cmath>

// Third party
// - geomtools
#include <geomtools/utils.h>
// - mygsl
#include <mygsl/rng.h>

namespace genvtx {

  geomtools::vector_3d
  ring_random_surface (mygsl::rng & random_,
                       double outer_radius_,
                       double inner_radius_ = 0.0,
                       double thickness_ = 0.0,
                       double theta1_ = 0.0,
                       double theta2_ = M_PI);

} // end of namespace genvtx

#endif // GENVTX_RING_RANDOM_TOOLS_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
