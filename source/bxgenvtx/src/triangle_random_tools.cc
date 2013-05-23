// -*- mode: c++ ; -*-
/* triangle_random_tools.cc
 */

#include <genvtx/triangle_random_tools.h>

namespace genvtx {

  geomtools::vector_3d
  triangle_random_surface (const geomtools::vector_3d & vtx0_,
                           const geomtools::vector_3d & vtx1_,
                           const geomtools::vector_3d & vtx2_,
                           mygsl::rng & random_)
  {
    // Use barycentric coordinates :
    // see: http://adamswaab.wordpress.com/2009/12/11/random-point-in-a-triangle-barycentric-coordinates/
    // or : http://mathworld.wolfram.com/TrianglePointPicking.html
    geomtools::vector_3d p;
    double rx = random_.uniform ();
    double ry = random_.uniform ();
    if (rx+ry >= 1.0) {
      rx = 1 - rx;
      ry = 1 - ry;
    }
    p = vtx0_+ rx * (vtx1_ - vtx0_) + ry * (vtx2_ - vtx0_);
    return p;
  }


} // end of namespace genvtx

// end of triangle_random_tools.cc
