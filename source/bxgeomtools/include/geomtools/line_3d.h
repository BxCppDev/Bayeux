// -*- mode: c++; -*-
/* line_3d.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2008-12-18
 * Last modified: 2008-12-18
 *
 * License:
 *
 * Description:
 *   Line 3D model
 *
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_LINE_3D_H_
#define GEOMTOOLS_LINE_3D_H_ 1

#include <iostream>
#include <string>

#include <geomtools/i_shape_1d.h>
#include <geomtools/utils.h>

#include <datatools/i_tree_dump.h>
#include <datatools/i_serializable.h>

namespace geomtools {

  class polyline_3d;
  class placement;

  class line_3d :
    public i_shape_1d,
    DATATOOLS_SERIALIZABLE_CLASS
  {
  public:

    static const std::string & line_3d_label();

  public:

    bool is_normal();

    virtual std::string get_shape_name () const;

    bool is_valid () const;

    void invalidate ();

    const vector_3d & get_first () const;

    void set_first (const vector_3d &);

    const vector_3d & get_last () const;

    void set_last (const vector_3d &);

    vector_3d get_point (double t_) const;

    double get_length () const;

  public:

    // ctor/dtor:
    line_3d ();

    line_3d (const vector_3d & first_,
             const vector_3d & last_);

    virtual ~line_3d ();

    /* interface i_tree_dumpable */
    virtual void tree_dump (std::ostream & out_         = std::clog,
                            const std::string & title_  = "",
                            const std::string & indent_ = "",
                            bool inherit_          = false) const;

    void dump () const;

    static void print_xyz (std::ostream & out_,
                           const line_3d & line_);

    double get_distance_to_line ( const vector_3d & position_ ) const;

    virtual bool is_on_curve (const vector_3d & position_,
                              double tolerance_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    virtual vector_3d get_direction_on_curve (const vector_3d & position_) const;


    void make_vertex_collection (basic_polyline_3d &) const;

    basic_polyline_3d make_vertex_collection () const;

    void generate_wires (std::list<polyline_3d> &, const placement & )const;

  private:

    vector_3d _first_;
    vector_3d _last_;

    /* interface i_serializable */
    DATATOOLS_SERIALIZATION_DECLARATION();

  };

} // end of namespace geomtools

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(geomtools::line_3d, "geomtools::line_3d")

#endif // GEOMTOOLS_LINE_3D_H_

// end of line_3d.h
