// -*- mode: c++; -*- 
/* gnuplot_draw.h
 * Author(s):     F. Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: <2006-11-19>
 * Last modified: <2006-11-22>
 * 
 * License: 
 * 
 * Description: 
 *   Gnuplot drawing tools
 * 
 * History: 
 * 
 */

#ifndef __geomtools__gnuplot_draw_h
#define __geomtools__gnuplot_draw_h 1

#include <cmath>
#include <stdexcept>
#include <iostream>
#include <list>

#include <geomtools/utils.h>

namespace geomtools {

  class gnuplot_draw
  {
  public: 
    typedef std::list<vector_3d> polyline_t;
  
  public: 
    static void 
    basic_draw_point (std::ostream &, 
		      const vector_3d &);

    static void 
    basic_draw_polyline (std::ostream &, 
			 const polyline_t &);
    
    static void 
    draw_line (std::ostream &, 
	       const vector_3d &, 
	       const vector_3d &);
    
    static void 
    draw_polyline  (std::ostream &, 
		    const vector_3d &, 
		    const rotation &,  
		    const polyline_t &, 
		    bool = false);
    
    static void 
    draw_segment  (std::ostream &, 
		   const vector_3d &, 
		   const rotation &,  
		   const vector_3d &, 
		   const vector_3d & );
    
    static void 
    draw_rectangle (std::ostream &, 
		    const vector_3d &, 
		    const rotation &,  
		    double, 
		    double, 
		    bool = false);
    
    static void 
    draw_circle (std::ostream &, 
		 const vector_3d &, 
		 const rotation &,
		 double, 
		 size_t = 36 );
    
    static void 
    draw_box (std::ostream &, 
	      const vector_3d &, 
	      const rotation &,
	      double, 
	      double, 
	      double);

    static void 
    draw_cylinder (std::ostream &, 
		   const vector_3d &, 
		   const rotation &,
		   double, 
		   double, 
		   size_t = 36);

    static void 
    draw_tube (std::ostream &, 
	       const vector_3d &, 
	       const rotation &,
	       double, 
	       double, 
	       double, 
	       size_t = 36);

    static void 
    draw_sphere (std::ostream &, 
		 const vector_3d &, 
		 const rotation &,
		 double, 
		 size_t = 36,
		 size_t = 10);

  };

} // end of namespace geomtools

#endif // __geomtools__gnuplot_draw_h

// end of gnuplot_draw.h
