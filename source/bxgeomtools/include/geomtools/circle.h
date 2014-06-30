// -*- mode: c++ ; -*-
/// \file geomtools/circle.h
/* Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-14
 * Last modified: 2010-02-14
 *
 * License:
 *
 * Description:
 *   A circle in x-y plane
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_CIRCLE_H
#define GEOMTOOLS_CIRCLE_H 1

// Standard library:
#include <iostream>
#include <string>

// This project:
#include <geomtools/i_shape_1d.h>
#include <geomtools/i_wires_3d_rendering.h>

namespace geomtools {

  /// \brief A circle (1D shape)
  class circle : public i_shape_1d,
                 public i_wires_3d_rendering
  {

  public:

    static const std::string & circle_label();

    bool is_valid () const;

    double get_r () const;

    double get_radius () const;

    void set_r (double a_radius);

    void set_diameter (double a_diameter);

    double get_diameter () const;

    double get_surface () const;

    double get_circumference () const;

    virtual unsigned int get_number_of_path() const;

    virtual double get_length(uint32_t flags_ = PATH_ALL_BITS) const;

  public:
    // ctor:
    circle ();

    circle (double a_radius);

    // dtor:
    virtual ~circle ();

    // methods:
    virtual std::string get_shape_name () const;

    virtual void tree_dump (std::ostream & a_out = std::clog,
                            const std::string & a_title = "",
                            const std::string & a_indent = "",
                            bool a_inherit= false) const;

    virtual bool is_on_curve (const vector_3d &,
                              double a_tolerance = GEOMTOOLS_PROPER_TOLERANCE) const;

    virtual vector_3d get_direction_on_curve (const vector_3d & a_tposition) const;

    virtual void generate_wires (std::list<polyline_3d> &,
                                 const placement & ,
                                 uint32_t options_ = 0) const;

  private:

    double _radius_; //!< The radius of the circle (in arbitrary units).

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_CIRCLE_H
