// -*- mode: c++ ; -*-
/* rectangle.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-14
 * Last modified: 2010-02-14
 *
 * License:
 *
 * Description:
 *   A rectangle in x-y plane
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_RECTANGLE_H_
#define GEOMTOOLS_RECTANGLE_H_ 1

#include <iostream>
#include <string>

#include <geomtools/i_shape_2d.h>
#include <geomtools/polyline_3d.h>
#include <geomtools/placement.h>
#include <geomtools/i_wires_3d_rendering.h>

namespace geomtools {

  class rectangle : public i_shape_2d,
                    public i_wires_3d_rendering
  {

  public:

    static const std::string & rectangle_label();

  public:

    bool is_valid () const;

    double get_x () const;

    double get_y () const;

    void set_x (double);

    void set_y (double);

    double get_surface () const;

    double get_circumference () const;

    double get_diagonal () const;


  public:
    // ctor:
    rectangle ();

    rectangle (double x_, double y_);

    // dtor:
    virtual ~rectangle ();

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

    double _x_;
    double _y_;

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_RECTANGLE_H_

// end of rectangle.h
