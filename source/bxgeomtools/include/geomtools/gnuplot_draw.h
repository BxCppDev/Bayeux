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

#include <geomtools/utils.h>
#include <geomtools/placement.h>
#include <geomtools/box.h>
#include <geomtools/cylinder.h>
#include <geomtools/sphere.h>
#include <geomtools/tube.h>
#include <geomtools/line_3d.h>
#include <geomtools/polyline_3d.h>
#include <geomtools/rectangle.h>
#include <geomtools/circle.h>
#include <geomtools/disk.h>
#include <geomtools/polycone.h>
#include <geomtools/tessellation.h>
#include <geomtools/union_3d.h>
#include <geomtools/subtraction_3d.h>
#include <geomtools/intersection_3d.h>

#include <geomtools/color.h>

namespace geomtools {

  class gnuplot_draw
  {
  public: 
    typedef basic_polyline_3d polyline_t;
  
    enum mode_flags_t
      {
	MODE_NULL = 0x0,
	MODE_WIRED_CYLINDER = 0x1
      };

  public: 

    static bool   g_using_color;
    static double g_current_color;

    static void 
    basic_draw_point_with_color (std::ostream &, 
				 double x_, double y_, double z_, 
				 double color_,
				 bool endl_ = true);

    static void 
    basic_draw_point_with_color (std::ostream & out_, 
				 const vector_3d & point_,
				 double color_,
				 bool   endl_ = true);

    static void 
    basic_draw_point (std::ostream &, 
		      double x_, double y_, double z_,
		      bool endl_);

    static void 
    basic_draw_point (std::ostream & out_, 
		      double x_, double y_, double z_)
    {
      basic_draw_point (out_, x_, y_, z_, true);
    }

    static void 
    basic_draw_point (std::ostream &, 
		      const vector_3d &,
		      bool endl_);

    static void 
    basic_draw_point (std::ostream & out_, 
		      const vector_3d & v_)
    {
      basic_draw_point (out_, v_, true);
    }

    static void 
    basic_draw_facet3 (std::ostream & out_, 
		       const vector_3d & p1_, 
		       const vector_3d & p2_, 
		       const vector_3d & p3_,
		       double color_ = 1.0)
    {
      basic_draw_point_with_color (out_, p1_, color_, true);
      basic_draw_point_with_color (out_, p2_, color_, true);
      out_ << endl;
      basic_draw_point_with_color (out_, p3_, color_, true);
      basic_draw_point_with_color (out_, p3_, color_, true);
      out_ << endl;
      out_ << endl;
    }

    static void 
    basic_draw_facet3 (std::ostream & out_, 
		       const vector_3d & p1_, 
		       const vector_3d & p2_, 
		       const vector_3d & p3_, 
		       const vector_3d & p4_,
		       double color_ = 1.0)
    {
      basic_draw_point_with_color (out_, p1_, color_);
      basic_draw_point_with_color (out_, p2_, color_);
      out_ << endl;
      basic_draw_point_with_color (out_, p4_, color_);
      basic_draw_point_with_color (out_, p3_, color_);
      out_ << endl;
      out_ << endl;
    }

    static void 
    basic_draw_polyline (std::ostream &, 
			 const polyline_t &);
    
    static void 
    draw_line (std::ostream &, 
	       const vector_3d &, 
	       const vector_3d &);
    
    static void 
    draw_line (std::ostream &, 
	       const line_3d &);
    
    static void 
    draw_polyline  (std::ostream &, 
		    const vector_3d &, 
		    const rotation_3d &,  
		    const polyline_t &, 
		    bool = false);
    
    static void 
    draw_polyline  (std::ostream &, 
		    const vector_3d &, 
		    const rotation_3d &,  
		    const polyline_3d &, 
		    bool = false);
    
    static void 
    draw_segment  (std::ostream &, 
		   const vector_3d &, 
		   const rotation_3d &,  
		   const vector_3d &, 
		   const vector_3d &);
    
    static void 
    draw_segment  (std::ostream &, 
		   const vector_3d &, 
		   const rotation_3d &,  
		   const line_3d &);

    static void 
    draw_line  (std::ostream &, 
		   const vector_3d &, 
		   const rotation_3d &,  
		   const line_3d &);
     
    static void 
    draw_rectangle (std::ostream &, 
		    const vector_3d &, 
		    const rotation_3d &,  
		    double, 
		    double, 
		    bool = false);
     
    static void 
    draw_rectangle (std::ostream &, 
		    const vector_3d &, 
		    const rotation_3d &,  
		    const rectangle &,
		    bool = false);
     
    static void 
    draw_circle (std::ostream &, 
		 const vector_3d &, 
		 const rotation_3d &,
		 double, 
		 size_t = 36 );
     
    static void 
    draw_circle (std::ostream &, 
		 const vector_3d &, 
		 const rotation_3d &,
		 const circle & , 
		 size_t = 36 );
      
    static void 
    draw_disk (std::ostream &, 
		 const vector_3d &, 
		 const rotation_3d &,
		 double, 
		 size_t = 36 );
      
    static void 
    draw_disk (std::ostream &, 
		 const vector_3d &, 
		 const rotation_3d &,
		 const disk & , 
		 size_t = 36 );
   
    static void 
    draw_box (std::ostream &, 
	      const vector_3d &, 
	      const rotation_3d &,
	      double, 
	      double, 
	      double);

    static void 
    draw_box (std::ostream &, 
	      const vector_3d &, 
	      const rotation_3d &,
	      const box &);

    static void 
    draw_cylinder (std::ostream &, 
		   const vector_3d &, 
		   const rotation_3d &,
		   double, 
		   double, 
		   size_t = 36);

    static void 
    draw_cylinder (std::ostream &, 
		   const vector_3d &, 
		   const rotation_3d &,
		   const cylinder &, 
		   size_t = 36);

    static void 
    draw_tube (std::ostream &, 
	       const vector_3d &, 
	       const rotation_3d &,
	       double, 
	       double, 
	       double, 
	       size_t = 36);

    static void 
    draw_tube (std::ostream &, 
	       const vector_3d &, 
	       const rotation_3d &,
	       const tube &, 
	       size_t = 36);

    static void 
    draw_sphere (std::ostream &, 
		 const vector_3d &, 
		 const rotation_3d &,
		 double, 
		 size_t = 36,
		 size_t = 16);

    static void 
    draw_sphere (std::ostream &, 
		 const vector_3d &, 
		 const rotation_3d &,
		 const sphere &, 
		 size_t = 36,
		 size_t = 16);

    static void 
    draw_polycone (std::ostream &, 
		   const vector_3d &, 
		   const rotation_3d &,
		   const polycone &, 
		   size_t = 36);

    static void 
    draw_tessellated (std::ostream &, 
		      const vector_3d &, 
		      const rotation_3d &,
		      const tessellated_solid &);

    static void 
    draw_union_3d (std::ostream &, 
		   const vector_3d &, 
		   const rotation_3d &,
		   const union_3d &);

    static void 
    draw_intersection_3d (std::ostream &, 
			  const vector_3d &, 
			  const rotation_3d &,
			  const intersection_3d &);

    static void 
    draw_subtraction_3d (std::ostream &, 
			 const vector_3d &, 
			 const rotation_3d &,
			 const subtraction_3d &);
  
    static void draw (ostream & out_, 
		     const i_placement &, 
		     const i_object_3d &);
 
    static void draw (ostream & out_, 
		     const i_placement &, 
		     const i_object_3d &,
		     unsigned long mode_);

  };

} // end of namespace geomtools

#endif // __geomtools__gnuplot_draw_h

// end of gnuplot_draw.h
