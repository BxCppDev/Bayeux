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

namespace geomtools {

  class sphere : public i_shape_3d
  {
  public:
    enum faces_mask_t
      {
	FACE_NONE   = 0x0,
	FACE_SIDE   = 0x1,
	FACE_ALL    = FACE_SIDE
      };  
  
  public:
    static const std::string SPHERE_LABEL;
  
  private: 
    double __r;
  
  public: 
    double 
    get_r() const;

    double 
    get_radius() const;

    void 
    set_r(double);

    void 
    set_radius(double);

    void 
    set(double);
  
    // ctor/dtor:
  public: 

    sphere();

    sphere(double);

    virtual 
    ~sphere();
  
    virtual std::string 
    get_shape_name() const;

    virtual double 
    get_parameter(const std::string &) const;

    bool 
    is_valid() const;

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
    is_on_surface(const vector_3d & , 
		  int mask_    = FACE_ALL , 
		  double skin_ = USING_PROPER_SKIN) const;

    friend std::ostream & 
    operator<<(std::ostream &, const sphere &);

    friend std::istream & 
    operator>>(std::istream &, sphere &);

  };

} // end of namespace geomtools

#endif // ___geomtools__sphere_h

// end of sphere.h
