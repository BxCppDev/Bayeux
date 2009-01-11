// -*- mode: c++; -*- 
/* box.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2008-05-23
 * Last modified: 2008-05-23
 * 
 * License: 
 * 
 * Description: 
 *  Interface:
 *
 * History: 
 * 
 */

#ifndef __geomtools__box_h
#define __geomtools__box_h 1

#include <sstream>
#include <stdexcept>

#include <geomtools/i_shape_3d.h>

namespace geomtools {

  class box : public i_shape_3d 
    {

    public:
      static const std::string BOX_LABEL;
      enum faces_mask_t
      {
	FACE_NONE   = face_3d::FACE_NONE_BIT,
	FACE_BACK   = 0x1,
	FACE_FRONT  = 0x2,
	FACE_LEFT   = 0x4,
	FACE_RIGHT  = 0x8,
	FACE_BOTTOM = 0x10,
	FACE_TOP    = 0x20,
	FACE_ALL    = (FACE_BACK 
		       | FACE_FRONT 
		       | FACE_LEFT 
		       | FACE_RIGHT 
		       | FACE_BOTTOM 
		       | FACE_TOP)
      };

    private: 
      double __x;
      double __y;
      double __z;

    public: 
      double get_x () const;

      void set_x (double);

      double get_half_x () const;

      void set_half_x (double);

      double get_y () const;

      void set_y(double);

      double get_half_y () const;

      void set_half_y (double);

      double get_z () const;

      void set_z (double);

      double get_half_z () const;

      void set_half_z (double);

      void set (double, double, double);

      void set_half (double, double, double);
  
  // ctor/dtor:
  public: 
      box ();

      box (double, double, double);

      virtual ~box ();
    
  // methods:
      
      virtual std::string 
	get_shape_name () const;

      double 
	get_parameter(const std::string &) const;

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

      // if 'skin' < 0 no skin is taken into account:
      virtual bool 
	is_on_surface(const vector_3d & , 
		      int mask_    = FACE_ALL , 
		      double skin_ = USING_PROPER_SKIN) const;

      friend std::ostream & operator<<( std::ostream & , const box & );

      friend std::istream & operator>>( std::istream & , box & );
      
      virtual bool find_intercept (const vector_3d & from_, 
				   const vector_3d & direction_,
				   vector_3d & intercept_,
				   int & face_,
				   double skin_ = USING_PROPER_SKIN) const;
       
    };
    
} // end of namespace geomtools

#endif // __geomtools__box_h

// end of box.h

