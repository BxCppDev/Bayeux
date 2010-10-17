// -*- mode: c++; -*- 
/* tube.h
 * Author(s):     F.Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2006-11-28
 * Last modified: 2010-10-13
 * 
 * License: 
 * 
 * Description: 
 *   3D tube description
 * 
 * History: 
 * 
 */

#ifndef __geomtools__tube_h
#define __geomtools__tube_h 1

#include <cmath>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <string>

#include <geomtools/i_shape_3d.h>
#include <geomtools/i_stackable.h>
#include <geomtools/cylinder.h>

namespace geomtools {

  class tube : public i_shape_3d, public i_stackable
  {
  public:
    enum faces_mask_t
      {
	FACE_NONE       = FACE_NONE_BIT,
	FACE_OUTER_SIDE = 0x1,
	FACE_BOTTOM     = 0x2,
	FACE_TOP        = 0x4,
	FACE_INNER_SIDE = 0x8,
	FACE_ALL        = (FACE_OUTER_SIDE 
			   | FACE_BOTTOM 
			   | FACE_TOP 
			   | FACE_INNER_SIDE)
      };  
  
  public:
    static const std::string TUBE_LABEL;
  
  private: 
    double __z;
    double __inner_r;
    double __outer_r;
  
  public: 
    
    double get_xmin () const
    {
      return -__outer_r;
    }
    
    double get_xmax () const
    {
      return +__outer_r;
    }
    
    double get_ymin () const
    {
      return -__outer_r;
    }
    
    double get_ymax () const
    {
      return +__outer_r;
    }
    
    double get_zmin () const
    {
      return -0.5*__z;
    }
    
    double get_zmax () const
    {
      return +0.5*__z;
    }

    double get_z () const;

    void set_z (double);

    double get_half_z() const;

    void set_half_z (double);

    double get_inner_r () const;

    void set_radii (double inner_r_, double outer_r_);

    double get_outer_r () const;

    void set (double inner_r_, double outer_r_, double z_);

    void set_half (double inner_r_, double outer_r_, double half_z_);

    void compute_inner_cylinder (cylinder & ic_);

    void compute_outer_cylinder (cylinder & oc_);

  public: 

    // ctor:
    tube();

    // ctor:
    tube (double inner_radius_, double outer_radius_, double z_);

    // dtor:
    virtual ~tube ();
  
    virtual std::string get_shape_name () const;

    virtual double get_parameter (const std::string &) const;

    bool is_extruded () const;

    bool is_valid () const;

    void reset ();

    double get_surface (int mask_ = FACE_ALL) const;

    double get_volume () const;

    virtual bool is_inside (const vector_3d &, 
			    double skin_ = USING_PROPER_SKIN) const;
    
    virtual bool is_on_surface (const vector_3d &, 
				int mask_    = FACE_ALL, 
				double skin_ = USING_PROPER_SKIN) const;

    virtual vector_3d get_normal_on_surface (const vector_3d & position_) const;
    
    virtual bool find_intercept (const vector_3d & from_, 
				 const vector_3d & direction_,
				 intercept_t & intercept_,
				 double skin_ = USING_PROPER_SKIN) const;

    friend std::ostream & 
    operator<< (std::ostream &, const tube &);

    friend std::istream & 
    operator>> (std::istream &, tube &);

    virtual void tree_dump (ostream & out_         = clog, 
			    const string & title_  = "", 
			    const string & indent_ = "", 
			    bool inherit_          = false) const;

  };

} // end of namespace geomtools

#endif // __geomtools__tube_h

// end of tube.h
