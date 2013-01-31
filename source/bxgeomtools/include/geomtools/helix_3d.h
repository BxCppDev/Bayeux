// -*- mode: c++; -*- 
/* helix_3d.h
 * Author(s): Francois Mauger <mauger@lpccaen.in2p3.fr>
 *            Arnaud Chapon   <chapon@lpccaen.in2p3.fr>
 * Creation date: 2008-12-18
 * Last modified: 2010-04-15
 * 
 * License: 
 * 
 * Description: 
 *
 *   Helix 3D model (along Z axis)
 *
 * History: 
 * 
 */

#ifndef GEOMTOOLS_HELIX_3D_H_
#define GEOMTOOLS_HELIX_3D_H_ 1

#include <iostream>
#include <string>

#include <geomtools/i_shape_1d.h>
#include <geomtools/utils.h>

#include <datatools/i_tree_dump.h>
#include <datatools/i_serializable.h>
#include <datatools/bit_mask.h>

namespace geomtools {

  class helix_3d : 
    public i_shape_1d,
    DATATOOLS_SERIALIZABLE_CLASS
  {
  public: 
    static const std::string HELIX_3D_LABEL;

  public: 

    bool is_normal();

    virtual std::string get_shape_name () const;

    bool is_valid () const;

    void invalidate ();

    const vector_3d & get_center () const;

    const vector_3d & get_center_const () const;

    vector_3d & get_center ();

    void   set_center (const vector_3d &);

    double get_radius () const;

    void   set_radius (double);

    double get_step () const;

    void   set_step (double);

    double get_t1 () const;

    void   set_t1 (double);

    double get_t2 () const;

    void   set_t2 (double);

    void   set_angle1 (double);

    void   set_angle2 (double);

    double get_angle1 () const;

    double get_angle2 () const;

    double get_length () const;

    double get_curvilinear_position (double t_) const;

    bool   t_is_valid (double t_) const;

    double get_theta_from_xy (double x_, double y_) const;

    double get_t (vector_3d point_) const;

    double get_t_from_xyz (double x_, double y_, double z_) const;

    double get_t_from_z (double t_) const;

    double get_z_from_t (double t_) const;

    vector_3d get_point (double t_) const;

    vector_3d get_point_xy (double theta_) const;

    void   invalidate_angles ();

    bool   angles_are_valid () const;

    vector_3d get_first () const;

    vector_3d get_last () const;
  
  public: 

    // ctor/dtor:
    helix_3d ();

    virtual ~helix_3d ();

    /* interface i_tree_dumpable */
    virtual void tree_dump (std::ostream & out_         = std::clog, 
                            const std::string & title_  = "",
                            const std::string & indent_ = "",
                            bool inherit_               = false) const;

    void dump () const;

    void make_vertex_collection (basic_polyline_3d &, double angular_step_ = 0.0) const;
    
    basic_polyline_3d make_vertex_collection () const;

  public:

    // convert parametric to angle in radian:
    static double t_to_angle (double t_);

    // convert angle in radian to parametric:
    static double angle_to_t (double angle_);
    
    enum print_flags_type
      {
        PRINT_XYZ_EXPAND1 = datatools::bit_mask::bit00,
        PRINT_XYZ_EXPAND2 = datatools::bit_mask::bit01,
        PRINT_XYZ_CENTER = datatools::bit_mask::bit02
      };

    static void print_xyz (std::ostream & out_, 
                           const helix_3d & helix_, 
                           double step_angle_ = 0.0,
                           unsigned int flags_ = 0);

  public:

    virtual bool is_on_curve (const vector_3d & position_, 
                              double tolerance_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    virtual vector_3d get_direction_on_curve (const vector_3d & position_) const;

  private: 

    double    _radius_;
    vector_3d _center_;
    double    _step_;
    double    _t1_;    // angle1 / (2 pi)
    double    _t2_;    // angle2 / (2 pi)

    /* interface i_serializable */
    DATATOOLS_SERIALIZATION_DECLARATION();

  };

} // end of namespace geomtools

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(geomtools::helix_3d, "geomtools::helix_3d")

#endif // __geomtools__helix_3d_h

// end of helix_3d.h
