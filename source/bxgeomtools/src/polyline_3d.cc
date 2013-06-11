// -*- mode: c++; -*-
/* polyline_3d.cc
 */

#include <geomtools/polyline_3d.h>

namespace geomtools {

  using namespace std;

  DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION (polyline_3d,"geomtools::polyline_3d")

  const string polyline_3d::POLYLINE_3D_LABEL = "polyline_3d";

  string polyline_3d::get_shape_name () const
  {
    return polyline_3d::POLYLINE_3D_LABEL;
  }

  bool
  polyline_3d::is_closed () const
  {
    return _closed_;
  }

  void
  polyline_3d::set_closed (bool closed_)
  {
    _closed_ = closed_;
    return;
  }

  polyline_3d::polyline_3d ()
  {
    _closed_ = DEFAULT_CLOSED;
    return;
  }

  polyline_3d::polyline_3d (bool closed_)
  {
    set_closed (closed_);
    return;
  }

  polyline_3d::~polyline_3d ()
  {
    _points_.clear ();
    return;
  }

  void
  polyline_3d::clear ()
  {
    _closed_ = DEFAULT_CLOSED;
    _points_.clear ();
    return;
  }

  void
  polyline_3d::add (const vector_3d & p_)
  {
    _points_.push_back (p_);
    return;
  }

  bool
  polyline_3d::is_empty () const
  {
    return _points_.size () == 0;
  }

  int
  polyline_3d::get_number_of_points () const
  {
    return _points_.size ();
  }

  const vector_3d &
  polyline_3d::get_point (int i_) const
  {
    DT_THROW_IF (is_empty (), std::logic_error, "Empty point collection!");
    DT_THROW_IF ((i_ < 0) || (i_ >= (int) _points_.size ()), std::domain_error, "Invalid point index!");
    int i = 0;
    point_col::const_iterator it = _points_.begin ();
    while (i < i_)
      {
        it++;
        i++;
      }
    return *it;
  }

  int
  polyline_3d::get_number_of_vertex () const
  {
    int n = _points_.size ();
    if ((is_closed ()) && (n > 0)) n++;
    return n;
  }

  const vector_3d &
  polyline_3d::get_vertex (int i_) const
  {
    DT_THROW_IF (is_empty (), std::logic_error, "Empty vertex collection!");

    if (! is_closed ())
      {
        return get_point (i_);
      }

    DT_THROW_IF ((i_ < 0) || (i_ >= get_number_of_vertex ()), std::domain_error,
                 "Invalid vertex index!");
    return get_point (i_ % get_number_of_points ());
  }

  void
  polyline_3d::make_vertex_collection (basic_polyline_3d & bpl_) const
  {
    bpl_.clear ();
    for (int i = 0; i < get_number_of_vertex (); i++)
      {
        bpl_.push_back (get_vertex (i));
      }
    return;
  }

  basic_polyline_3d
  polyline_3d::make_vertex_collection () const
  {
    basic_polyline_3d bpl;
    make_vertex_collection (bpl);
    return bpl;
  }

  bool polyline_3d::is_on_curve (const vector_3d & position_,
                                 double tolerance_) const
  {
    bool on_curve = false;
    DT_THROW_IF (true, std::logic_error, "Not implemented yet !");
    return on_curve;
  }

  vector_3d polyline_3d::get_direction_on_curve (const vector_3d & position_) const
  {
    vector_3d dir;
    DT_THROW_IF (true, std::logic_error, "Not implemented yet !");
    return dir;
  }

} // end of namespace geomtools

// end of polyline_3d.cc
