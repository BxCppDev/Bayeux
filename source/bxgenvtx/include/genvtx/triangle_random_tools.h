// -*- mode: c++ ; -*- 
/* triangle_random_tools.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2012-09-26
 * Last modified: 2012-09-26
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

#ifndef __genvtx__triangle_random_tools_h
#define __genvtx__triangle_random_tools_h 1

#include <geomtools/utils.h>
#include <mygsl/rng.h>

namespace genvtx {

  geomtools::vector_3d 
  triangle_random_surface (const geomtools::vector_3d & vtx0_,
                           const geomtools::vector_3d & vtx1_,
                           const geomtools::vector_3d & vtx2_, 
                           mygsl::rng & random_);
  
  
} // end of namespace genvtx

#endif // __genvtx__triangle_random_tools_h

// end of triangle_random_tools.h
