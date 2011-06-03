// -*- mode: c++; -*- 
/* intersection.h
 * Author(s):     F.Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2006-11-28
 * Last modified: 2008-05-24
 * 
 * License: 
 * 
 * Description: 
 *   Basic intersection algorithms
 * 
 * History: 
 * 
 */

#ifndef __geomtools__intersection_h
#define __geomtools__intersection_h 1

#include <geomtools/utils.h>

namespace geomtools {

  namespace intersection {


    bool find_intersection_line_circle_2d (const vector_2d & line_origin_, 
					   const vector_2d & line_direction_,
					   const vector_2d & circle_origin_,
					   double circle_radius_,
					   size_t & nsols_,
					   double & u1_,
					   double & u2_);

    bool find_intersection_line_circle_2d (const vector_2d & line_origin_, 
					   const vector_2d & line_direction_,
					   const vector_2d & circle_origin_,
					   double circle_radius_,
					   size_t & nsols_,
					   vector_2d & pos1_,
					   vector_2d & pos2_);


    bool find_intersection_segment_disk_2d (const vector_2d & segment_first_, 
					    const vector_2d & segment_last_,
					    const vector_2d & disk_origin_,
					    double disk_radius_,
					    vector_2d & first_,
					    vector_2d & last_);

  } // end of namespace intersection

} // end of namespace geomtools

#endif // ___geomtools__intersection_h

// end of intersection.h
