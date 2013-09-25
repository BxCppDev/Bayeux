// -*- mode: c++; -*-
/* box.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2008-05-23
 * Last modified: 2012-10-24
 *
 * License:
 *
 * Description:
 *  Interface:
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_BOX_H_
#define GEOMTOOLS_BOX_H_ 1

#include <boost/cstdint.hpp>

// This project
#include <geomtools/i_shape_3d.h>
#include <geomtools/i_stackable.h>
#include <geomtools/polyline_3d.h>
#include <geomtools/placement.h>
#include <geomtools/i_wires_3d_rendering.h>

namespace geomtools {

  class box : public i_shape_3d ,
              public i_stackable,
              public i_wires_3d_rendering
  {

  public:

    static const std::string & box_label();
    //DT_STATIC_CONST_STRING_REF_DECLARATION(box_label);

    enum faces_mask_type
      {
        FACE_NONE   = geomtools::FACE_NONE,
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

    virtual double get_surface (uint32_t a_mask = FACE_ALL_BITS) const;

    virtual double get_volume (uint32_t flags_ = 0) const;

    virtual bool is_inside (const vector_3d &,
                            double a_skin = GEOMTOOLS_PROPER_TOLERANCE) const;

    // if 'skin' < 0 no skin is taken into account:
    virtual bool is_on_surface (const vector_3d & ,
                                int a_mask    = FACE_ALL_BITS ,
                                double a_skin = GEOMTOOLS_PROPER_TOLERANCE) const;

    virtual vector_3d get_normal_on_surface (const vector_3d & a_position) const;

    friend std::ostream & operator<< ( std::ostream & , const box & );

    friend std::istream & operator>> ( std::istream & , box & );

    virtual bool find_intercept (const vector_3d & a_from,
                                 const vector_3d & a_direction,
                                 intercept_t & a_intercept,
                                 double a_skin = GEOMTOOLS_PROPER_TOLERANCE) const;

    virtual void tree_dump (std::ostream & a_out         = std::clog,
                            const std::string & a_title  = "",
                            const std::string & a_indent = "",
                            bool a_inherit          = false) const;

    virtual void generate_wires (std::list<polyline_3d> &,
                                 const placement &,
                                 uint32_t options_ = 0) const;

  private:

    double _x_; //!< Width (in arbitrary units)
    double _y_; //!< Length (in arbitrary units)
    double _z_; //!< Height (in arbitrary units)

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_BOX_H_

// end of box.h
