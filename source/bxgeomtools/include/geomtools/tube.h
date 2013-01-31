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

#ifndef GEOMTOOLS_TUBE_H_
#define GEOMTOOLS_TUBE_H_ 1

#include <iostream>
#include <string>

#include <geomtools/i_shape_3d.h>
#include <geomtools/i_stackable.h>
#include <geomtools/i_wires_3d_rendering.h>

namespace geomtools {

  class cylinder;

  class tube : public i_shape_3d, public i_stackable
  {
  public:
    enum faces_mask_type
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
  
  public: 
    
    double get_xmin () const;
    
    double get_xmax () const;
    
    double get_ymin () const;

    double get_ymax () const;
    
    double get_zmin () const;
    
    double get_zmax () const;

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
                            double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const;
    
    virtual bool is_on_surface (const vector_3d &, 
                                int mask_    = FACE_ALL, 
                                double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    virtual vector_3d get_normal_on_surface (const vector_3d & position_) const;
    
    virtual bool find_intercept (const vector_3d & from_, 
                                 const vector_3d & direction_,
                                 intercept_t & intercept_,
                                 double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    friend std::ostream & 
    operator<< (std::ostream &, const tube &);

    friend std::istream & 
    operator>> (std::istream &, tube &);

    virtual void tree_dump (std::ostream & out_         = std::clog, 
                            const std::string & title_  = "", 
                            const std::string & indent_ = "", 
                            bool inherit_          = false) const;

    virtual void generate_wires (std::list<polyline_3d> &, 
                                 const placement & , 
                                 uint32_t options_ = 0) const;
      
  private: 

    double _z_;
    double _inner_r_;
    double _outer_r_;

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_TUBE_H_

// end of tube.h
