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
#include <geomtools/i_stackable.h>

namespace geomtools {

  class box : public i_shape_3d ,
	      public i_stackable
  {

  public:
    static const std::string BOX_LABEL;
    enum faces_mask_t
      {
	FACE_NONE   = FACE_NONE_BIT,
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

  public: 
    
    double get_xmin () const;
    
    double get_xmax () const;
    
    double get_ymin () const;
      
    double get_ymax () const;
    
    double get_zmin () const;
    
    double get_zmax () const;
    
    double get_x () const;

    void set_x (double);

    double get_half_x () const;

    void set_half_x (double);

    double get_y () const;

    void set_y (double);

    double get_half_y () const;

    void set_half_y (double);

    double get_z () const;

    void set_z (double);

    double get_half_z () const;

    void set_half_z (double);

    void set (double, double, double);

    void set_half (double, double, double);
  
  public: 

    // ctor:
    box ();

    // ctor:
    box (double a_x, double a_y, double a_z);

    // dtor:
    virtual ~box ();
    
    // methods:
      
    virtual std::string get_shape_name () const;

    double get_parameter (const std::string &) const;

    bool is_valid () const;

    void init ();

    void reset ();

    double get_surface (int a_mask = FACE_ALL) const;

    double get_volume () const;

    virtual bool is_inside (const vector_3d &, 
			    double a_skin = USING_PROPER_SKIN) const;

    // if 'skin' < 0 no skin is taken into account:
    virtual bool is_on_surface (const vector_3d & , 
				int a_mask    = FACE_ALL , 
				double a_skin = USING_PROPER_SKIN) const;

    virtual vector_3d get_normal_on_surface (const vector_3d & a_position) const;

    friend std::ostream & operator<< ( std::ostream & , const box & );

    friend std::istream & operator>> ( std::istream & , box & );
      
    virtual bool find_intercept (const vector_3d & a_from, 
				 const vector_3d & a_direction,
				 intercept_t & a_intercept,
				 double a_skin = USING_PROPER_SKIN) const;

    virtual void tree_dump (ostream & a_out         = clog, 
			    const string & a_title  = "", 
			    const string & a_indent = "", 
			    bool a_inherit          = false) const;

  private: 

    double x_; //!< Width (in arbitrary units)
    double y_; //!< Length (in arbitrary units)
    double z_; //!< Height (in arbitrary units)
       
  };
    
} // end of namespace geomtools

#endif // __geomtools__box_h

// end of box.h

