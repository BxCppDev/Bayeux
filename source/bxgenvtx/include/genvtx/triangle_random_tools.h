// -*- mode: c++ ; -*- 
/* triangle_random_tools.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2012-09-26
 * Last modified: 2013-02-24
 * 
 * License: 
 * 
 * Description: 
 *   
 *    Algorithm to randomize points on the surface of a triangle
 * 
 * History: 
 * 
 *
 */

#ifndef GENVTX_TRIANGLE_RANDOM_TOOLS_H_
#define GENVTX_TRIANGLE_RANDOM_TOOLS_H_ 1

#include <geomtools/utils.h>
#include <mygsl/rng.h>

namespace genvtx {

  geomtools::vector_3d 
  triangle_random_surface (const geomtools::vector_3d & vtx0_,
                           const geomtools::vector_3d & vtx1_,
                           const geomtools::vector_3d & vtx2_, 
                           mygsl::rng & random_);
  
} // end of namespace genvtx

#endif // GENVTX_TRIANGLE_RANDOM_TOOLS_H_

// end of triangle_random_tools.h
