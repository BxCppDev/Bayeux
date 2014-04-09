/* quadrangle_random_tools.cc
 */

// Ourselves:
#include <genvtx/quadrangle_random_tools.h>

// Third party:
// - Bayeux/datatools
#include <datatools/utils.h>

// This project:
#include <genvtx/triangle_random_tools.h>

namespace genvtx {

  geomtools::vector_3d
  quadrangle_random_surface (const geomtools::vector_3d & vtx0_,
                             const geomtools::vector_3d & vtx1_,
                             const geomtools::vector_3d & vtx2_,
                             const geomtools::vector_3d & vtx3_,
                             mygsl::rng & random_,
                             double & area012_, double & area230_)
  {
    //geomtools::vector_3d bidon;
    if (! datatools::is_valid (area012_))
      {
        area012_ = ((vtx1_-vtx0_).cross (vtx2_-vtx0_)).mag ();
      }
    if (! datatools::is_valid (area230_))
      {
        area230_ = ((vtx3_-vtx2_).cross (vtx0_-vtx2_)).mag ();
      }
    double p012 = area012_ / (area012_ + area230_);

    double r = random_.uniform ();
    if (r < p012)
      {
        return ::genvtx::triangle_random_surface (vtx0_, vtx1_, vtx2_, random_);
      }
    return ::genvtx::triangle_random_surface (vtx2_, vtx3_, vtx0_, random_);
  }


} // end of namespace genvtx
