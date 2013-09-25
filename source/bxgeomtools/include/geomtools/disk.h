// -*- mode: c++ ; -*-
/* disk.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-14
 * Last modified: 2010-02-14
 *
 * License:
 *
 * Description:
 *   A disk in x-y plane
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_DISK_H
#define GEOMTOOLS_DISK_H 1

#include <geomtools/i_shape_2d.h>
#include <geomtools/i_wires_3d_rendering.h>

namespace geomtools {

  class disk : public i_shape_2d
  {

  public:
    static const std::string & disk_label();

  public:

    bool is_valid () const;

    double get_r () const;

    double get_radius () const;

    void set_r (double);

    void set_diameter (double);

    double get_diameter () const;

    double get_surface () const;

    double get_circumference () const;

  public:
    // ctor:
    disk ();

    disk (double r_);

    // dtor:
    virtual ~disk ();

    // methods:
    virtual std::string get_shape_name () const;

    virtual bool is_on_surface (const vector_3d &,
                                double tolerance_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    virtual vector_3d get_normal_on_surface (const vector_3d & position_,
                                             bool up_ = true) const;

    virtual bool find_intercept (const vector_3d & from_,
                                 const vector_3d & direction_,
                                 intercept_t & intercept_,
                                 double tolerance_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    virtual void tree_dump (std::ostream & out_ = std::clog,
                            const std::string & title_ = "",
                            const std::string & indent_ = "",
                            bool inherit_= false) const;

    virtual void generate_wires (std::list<polyline_3d> &,
                                 const placement & ,
                                 uint32_t options_ = 0) const;

  private:

    double _radius_;

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_DISK_H

// end of disk.h
