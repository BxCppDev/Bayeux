// -*- mode: c++; -*- 
/* sphere.h
 * Author(s):     F.Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2006-11-28
 * Last modified: 2008-05-24
 * 
 * License: 
 * 
 * Description: 
 *   3D sphere description
 * 
 * History: 
 * 
 */

#ifndef __geomtools__sphere_h
#define __geomtools__sphere_h 1

#include <cmath>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <string>

#include <geomtools/i_shape_3d.h>
#include <geomtools/i_stackable.h>

namespace geomtools {

  class sphere : public i_shape_3d, public i_stackable
  {
  public:
    enum faces_mask_t
      {
	FACE_NONE   = FACE_NONE_BIT,
	FACE_SIDE   = 0x1,
	FACE_ALL    = FACE_SIDE
      };  
  
  public:
    static const std::string SPHERE_LABEL;
  
  private: 
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

    double get_z () const
    {
      return 2. * __r;
    }

    double 
    get_r () const;

    double 
    get_radius () const;

    void 
    set_r (double);

    void 
    set_radius (double);

    void 
    set (double);
  
    // ctor/dtor:
  public: 

    sphere ();

    sphere (double radius_);

    virtual 
    ~sphere ();
  
    virtual std::string 
    get_shape_name () const;

    virtual double 
    get_parameter (const std::string &) const;

    bool 
    is_valid () const;

    void 
    reset ();

    double 
    get_surface (int mask_ = FACE_ALL) const;

    double 
    get_volume () const;

    virtual bool is_inside (const vector_3d &, 
			    double skin_ = USING_PROPER_SKIN) const;
    
    virtual bool is_on_surface (const vector_3d & , 
				int mask_    = FACE_ALL , 
				double skin_ = USING_PROPER_SKIN) const;

    virtual vector_3d get_normal_on_surface (const vector_3d & position_) const;
    
    virtual bool find_intercept (const vector_3d & from_, 
				 const vector_3d & direction_,
				 intercept_t & intercept_,
				 double skin_ = USING_PROPER_SKIN) const;

    friend std::ostream & 
    operator<< (std::ostream &, const sphere &);

    friend std::istream & 
    operator>> (std::istream &, sphere &);

    virtual void tree_dump (ostream & out_         = clog, 
			    const string & title_  = "", 
			    const string & indent_ = "", 
			    bool inherit_          = false) const;
    
  };

} // end of namespace geomtools

#endif // ___geomtools__sphere_h

// end of sphere.h
