// -*- mode: c++; -*- 
/* cylinder.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2008-05-24
 * Last modified: 2008-05-24
 * 
 * License: 
 * 
 * Description: 
 *  A cylinder on z-axis
 *
 * History: 
 * 
 */

#ifndef __geomtools__cylinder_h
#define __geomtools__cylinder_h 1

#include <sstream>
#include <stdexcept>

#include <geomtools/i_shape_3d.h>
#include <geomtools/i_stackable.h>

namespace geomtools {

  using namespace std;

  class cylinder : public i_shape_3d , public i_stackable
    {

    public:
      static const string CYLINDER_LABEL;

      enum faces_mask_t
	{
	  FACE_NONE   = FACE_NONE_BIT,
	  FACE_SIDE   = 0x1,
	  FACE_BOTTOM = 0x2,
	  FACE_TOP    = 0x4,
	  FACE_ALL    = (FACE_SIDE
			 | FACE_BOTTOM 
			 | FACE_TOP)
	};  

    private: 
      double __z;
      double __r;
  
    public: 
    
      double get_x () const
      {
	return 2. * __r;
      }
      
      double get_y () const
      {
	return 2. * __r;
      }
      
      double 
	get_z() const;

      void 
	set_z(double);

      void 
	set_diameter(double);

      double 
	get_half_z() const;

      void 
	set_half_z(double);

      double 
	get_r() const;

      double 
	get_radius() const;

      double 
	get_diameter() const;

      void 
	set_r(double);

      void 
	set(double, double);

      void 
	set_alternative(double, double);

  
      // ctor/dtor:
    public: 

      cylinder();

      cylinder(double r_, double z_);

      virtual ~cylinder();
  
      // methods:
      virtual string 
	get_shape_name() const;

      virtual double 
	get_parameter(const string &) const;

      bool 
	is_valid() const;

      void 
	init();

      void 
	reset();

      double 
	get_surface(int mask_ = FACE_ALL) const;

      double 
	get_volume() const;

      virtual bool 
	is_inside(const vector_3d &, 
		  double skin_ = USING_PROPER_SKIN) const;

      virtual bool 
	is_on_surface(const vector_3d &, 
		      int mask_    = FACE_ALL, 
		      double skin_ = USING_PROPER_SKIN) const;

      virtual vector_3d get_normal_on_surface (const vector_3d & position_) const;

      virtual bool find_intercept (const vector_3d & from_, 
				   const vector_3d & direction_,
				   intercept_t & intercept_,
				   double skin_ = USING_PROPER_SKIN) const;

      friend ostream & 
	operator<<(ostream &, const cylinder &);

      friend istream & 
	operator>>(istream &, cylinder &);

      virtual void tree_dump (ostream & out_         = clog, 
			      const string & title_  = "", 
			      const string & indent_ = "", 
			      bool inherit_          = false) const;
       

    };
    
} // end of namespace geomtools

#endif // __geomtools__cylinder_h

// end of cylinder.h

