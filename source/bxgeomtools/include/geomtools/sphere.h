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

#ifndef GEOMTOOLS_SPHERE_H_
#define GEOMTOOLS_SPHERE_H_ 1

#include <string>
#include <iostream>


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
  
  public: 
    
    double get_xmin () const;

    double get_xmax () const;
    
    double get_ymin () const;
    
    double get_ymax () const;
    
    double get_zmin () const;
    
    double get_zmax () const;

    double get_r () const;

    double get_radius () const;

    void set_r (double);

    void set_radius (double);

    void set (double);
  
  public: 

    // ctor:
    sphere ();

    // ctor:
    sphere (double radius_);

    // dtor:
    virtual ~sphere ();
  
    virtual std::string get_shape_name () const;

    virtual double get_parameter (const std::string &) const;

    bool is_valid () const;

    void reset ();

    double get_surface (int mask_ = FACE_ALL) const;

    double get_volume () const;

    virtual bool is_inside (const vector_3d &, 
                            double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const;
    
    virtual bool is_on_surface (const vector_3d & , 
                                int mask_    = FACE_ALL , 
                                double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    virtual vector_3d get_normal_on_surface (const vector_3d & position_) const;
    
    virtual bool find_intercept (const vector_3d & from_, 
                                 const vector_3d & direction_,
                                 intercept_t & intercept_,
                                 double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    friend std::ostream & 
    operator<< (std::ostream &, const sphere &);

    friend std::istream & 
    operator>> (std::istream &, sphere &);

    virtual void tree_dump (std::ostream & out_         = std::clog, 
                            const std::string & title_  = "", 
                            const std::string & indent_ = "", 
                            bool inherit_          = false) const;
  
  private: 

    double _r_;
    
  };

} // end of namespace geomtools

#endif // GEOMTOOLS_SPHERE_H_

// end of sphere.h
