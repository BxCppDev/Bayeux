// -*- mode: c++; -*- 
/* intersection.cc
 */

#include <geomtools/intersection.h>

#include <cmath>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <string>

#include <datatools/utils/utils.h>

namespace geomtools {

  namespace intersection {


    /**
     * Some useful formulae at:
     *
     *  http://local.wasp.uwa.edu.au/~pbourke/geometry/sphereline/
     *
     */
    bool find_intersection_line_circle_2d (const vector_2d & line_origin_, 
					   const vector_2d & line_direction_,
					   const vector_2d & circle_origin_,
					   double circle_radius_,
					   size_t & nsols_,
					   double & u1_,
					   double & u2_)
    {
      // initialize output as invalid:
      nsols_ = 0;
      datatools::utils::invalidate (u1_);
      datatools::utils::invalidate (u2_);

      const vector_2d & P1 = line_origin_;
      const vector_2d & P2 = line_origin_ + line_direction_;
      const vector_2d & P3 = circle_origin_;
      
      double x1 = P1.x ();
      double x2 = P2.x ();
      double x3 = P3.x ();
      double y1 = P1.y ();
      double y2 = P2.y ();
      double y3 = P3.y ();
      double r = circle_radius_;
      double a = pow (x2 - x1, 2) + pow (y2 - y1, 2);
      double b = 2 * ((x2 - x1) * (x1 - x3) + (y2 - y1) * (y1 - y3));
      double c = x3 * x3 + y3 * y3 + x1 * x1 + y1 * y1
	-2 * (x3 * x1 + y3 * y1) - r * r;
	
      double delta = b * b - 4 * a * c;

      double t1 = -b / (2 * a); 
      if (abs (delta) < 1.e-16)
	{
	  nsols_ = 1;
	  u1_ = t1;
	} 
      else if (delta > 1.e-16)
	{
	  nsols_ = 2;
	  double t2 = sqrt (delta) / (2 * a);
	  u1_ = t1 + t2;
	  u2_ = t1 - t2;
	}

      // return success flag:
      return nsols_ > 0;
    }


    bool find_intersection_line_circle_2d (const vector_2d & line_origin_, 
					   const vector_2d & line_direction_,
					   const vector_2d & circle_origin_,
					   double circle_radius_,
					   size_t & nsols_,
					   vector_2d & pos1_,
					   vector_2d & pos2_)
    {
      // initialize output as invalid:
      nsols_ = 0;
      invalidate (pos1_);
      invalidate (pos2_);

      // local solutions:
      double u1, u2;
      find_intersection_line_circle_2d (line_origin_,
					line_direction_,
					circle_origin_,
					circle_radius_,
					nsols_,
					u1,
					u2);
      if (nsols_ >= 1)
	{
	  pos1_ = line_origin_ + u1 * line_direction_;
	}
      if (nsols_ == 2)
	{
	  pos2_ = line_origin_ + u2 * line_direction_;
	}
      // return success flag:
      return nsols_ > 0;
    }
    
    bool find_intersection_segment_disk_2d (const vector_2d & segment_first_, 
					    const vector_2d & segment_last_,
					    const vector_2d & disk_origin_,
					    double disk_radius_,
					    vector_2d & first_,
					    vector_2d & last_)
    {
      vector_2d segment_direction = (segment_last_ - segment_first_).unit ();
      
      // local solutions:
      size_t nsols = 0;
      double u1, u2;
      double umin = 0.0;
      double umax = (segment_last_ - segment_first_).mag ();
      find_intersection_line_circle_2d (segment_first_,
					segment_direction,
					disk_origin_,
					disk_radius_,
					nsols,
					u1,
					u2);
      invalidate (first_);
      invalidate (last_);
      if (nsols == 0)
	{
	  // segment outside the disk => no solution
	  return false;
	}
      else if (nsols == 1)
	{
	  // segment tangential to the disk:
	  if ((u1 < umin) || (u1 > umax))
	    {
	      // tangent contact is outside the segment => no solution
	      return false;
	    }
	  first_ = segment_first_ + u1 * segment_direction;
	  last_  = first_;
	  return true;
	}
      else if (nsols == 2)
	{
	  // segment's line is crossing the disk:
	  
	  // reordering solutions u1 and u2: 
	  if (u1 > u2) swap (u1, u2);
	  
	  if ((u1 < umin) && (u2 < umin))
	    {
	      // both intersections are outside/before the segment 
	      // => no solution
	      return false;
	    }
	  else if ((u1 > umax) && (u2 > umax))
	    {
	      // both intersections are outside/after the segment 
	      //=> no solution
	      return false;
	    }
	  else if ((u1 <= umin) &&  (u2 >= umax))
	    {
	      // segment is fully contained within the disk 
	      // => intersecting segment is the segment itself
	      first_ = segment_first_;
	      last_ = segment_last_;
	      return true;
	    }
	  else if ((u1 <= umax) && (u1 >= umin) && (u2 <= umax) && (u2 >= umin))
	    {
	      // segment is crossing the disk
	      // => intersecting segment is the chord made by intersecting points
	      first_ = segment_first_ + u1 * segment_direction;
	      last_ = segment_first_ + u2 * segment_direction;
	      return true;
	    }
	  else if ((u1 <= umax) && (u1 >= umin) && (u2 > umax))
	    {
	      // segment is partially contained within the disk 
	      // => intersecting segment is:
	      first_ = segment_first_ + u1 * segment_direction;
	      last_ = segment_last_;
	      return true;
	    }
	  else if ((u2 <= umax) && (u2 >= umin) && (u1 < umin))
	    {
	      // segment is partially contained within the disk 
	      // => intersecting segment is:
	      first_= segment_first_;
	      last_ = segment_first_ + u2 * segment_direction;
	      return true;
	    }
	}
        return false;
     }
      
  } // end of namespace intersection
    
} // end of namespace geomtools

// end of intersection.cc
