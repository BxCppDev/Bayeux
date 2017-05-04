/// polyline_3d.cc

// Ourselves:
#include <geomtools/polyline_3d.h>

// This project:
#include <geomtools/line_3d.h>

namespace geomtools {

  DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(polyline_3d, "geomtools::polyline_3d")

  // static
  const bool polyline_3d::CLOSED;
  const bool polyline_3d::closed;
  const bool polyline_3d::OPEN;
  const bool polyline_3d::open;
  const bool polyline_3d::DEFAULT_CLOSED;

  const std::string & polyline_3d::polyline_3d_label()
  {
    static std::string label;
    if (label.empty()) {
      label = "polyline_3d";
    }
    return label;
  }

  std::string polyline_3d::get_shape_name () const
  {
    return polyline_3d::polyline_3d_label();
  }

  bool polyline_3d::is_valid() const
  {
    if (_points_.size() < 2) return false;
    if (is_closed() && _points_.size() < 3) return false;
    return true;
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
    while (i < i_) {
      it++;
      i++;
    }
    return *it;
  }

  int
  polyline_3d::get_number_of_vertex() const
  {
    int n = _points_.size ();
    if ((is_closed ()) && (n > 0)) n++;
    return n;
  }

  int
  polyline_3d::get_number_of_vertexes() const
  {
    int n = _points_.size ();
    if ((is_closed ()) && (n > 0)) n++;
    return n;
  }

  const vector_3d &
  polyline_3d::get_vertex (int i_) const
  {
    DT_THROW_IF (is_empty (), std::logic_error, "Empty vertex collection!");

    if (! is_closed ()){
      return get_point (i_);
    }

    DT_THROW_IF ((i_ < 0) || (i_ >= get_number_of_vertex ()), std::domain_error,
                 "Invalid vertex index!");
    return get_point(i_ % get_number_of_points ());
  }

  void
  polyline_3d::make_vertex_collection (basic_polyline_3d & bpl_) const
  {
    bpl_.clear ();
    for (int i = 0; i < get_number_of_vertex (); i++) {
      bpl_.push_back (get_vertex (i));
    }
    return;
  }

  polyline_type
  polyline_3d::make_vertex_collection () const
  {
    polyline_type bpl;
    make_vertex_collection (bpl);
    return bpl;
  }

  // virtual
  double polyline_3d::get_length(uint32_t /* flags_ */) const
  {
    double l(0.0);
    if (_points_.size() > 1) {
      for (point_col::const_iterator i = _points_.begin();
           i != _points_.end();
           i++) {
        point_col::const_iterator j = i;
        j++;
        if (j == _points_.end()) {
          break;
        }
        const vector_3d & A = *i;
        const vector_3d & B = *j;
        l += (B-A).mag();
      }
      if (is_closed()) {
        l +=  (*_points_.begin() - *_points_.end()).mag();
      }
    }
    return l;
  }

  bool polyline_3d::is_on_curve (const vector_3d & position_,
                                 double tolerance_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid polyline!");
    bool devel = false;
    // devel = true;
    if (devel) std::cerr << "DEVEL: polyline_3d::is_on_curve: " << std::endl;
    double tolerance = compute_tolerance(tolerance_);
    bool on_curve = false;
    if (_points_.size() > 1) {
      point_col::const_iterator i = _points_.begin();
      point_col::const_iterator j = i;
      j++;
      for (;
           j != _points_.end();
           j++) {
        const vector_3d & v0 = *i;
        const vector_3d & v1 = *j;
        if (devel) std::cerr << "DEVEL:   v0 = " << v0 << std::endl;
        if (devel) std::cerr << "DEVEL:   v1 = " << v1 << std::endl;
        line_3d segment(v0, v1);
        if (segment.is_on_curve(position_, tolerance)) {
          on_curve = true;
        }
        i = j;
        if (on_curve) {
          break;
        }
      }
      if (is_closed()) {
        j = _points_.begin();
        line_3d segment(*i, *_points_.begin());
        if (segment.is_on_curve(position_, tolerance)) {
          on_curve = true;
        }
      }
    }
    return on_curve;
  }

  vector_3d polyline_3d::get_direction_on_curve (const vector_3d & position_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid polyline!");
    vector_3d dir;
    geomtools::invalidate(dir);
    if (_points_.size() > 1) {
      double dist;
      datatools::invalidate(dist);
      point_col::const_iterator found = _points_.end();
      point_col::const_iterator found2 = _points_.end();
      point_col::const_iterator i = _points_.begin();
      point_col::const_iterator j = i;
      j++;
      for (;
           j != _points_.end();
           j++) {
        const vector_3d & v0 = *i;
        const vector_3d & v1 = *j;
        line_3d segment(v0, v1);
        double d = segment.get_distance_to_line(position_);
        if (!datatools::is_valid(dist) || d < dist) {
          dist = d;
          found = i;
          found2 = j;
        }
        i = j;
      }
      if (is_closed()) {
        j = _points_.begin();
        line_3d segment(*i, *_points_.begin());
        double d = segment.get_distance_to_line(position_);
        if (!datatools::is_valid(dist) || d < dist) {
          dist = d;
          found = i;
          found2 = j;
        }
      }
      if (datatools::is_valid(dist)) {
        line_3d segment(*found, *found2);
        dir = segment.get_direction_on_curve(position_);
      }
    }
    return dir;
  }

  const polyline_type & polyline_3d::get_points() const
  {
    return _points_;
  }

  polyline_type & polyline_3d::grab_points()
  {
    return _points_;
  }

  void polyline_3d::tree_dump(std::ostream &  out_,
                              const std::string & title_,
                              const std::string & indent_,
                              bool                inherit_) const
  {
    i_object_3d::tree_dump(out_, title_, indent_, true);
    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Closed : "
         << _closed_
         << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Number of points : "
         << '[' << _points_.size() << ']'
         << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
         << "Length : "
         << get_length() /* / CLHEP::mm << " mm"*/
         << std::endl;

    return;
  }

  void polyline_3d::generate_wires_self(wires_type & wires_,
                                        uint32_t /* options_ */) const
  {
    {
      polyline_type dummy;
      wires_.push_back(dummy);
    }
    polyline_type & wire = wires_.back();
    wire = _points_;
    if (is_closed() && _points_.size() > 2) {
      wire.push_back(*_points_.begin());
    }
    return;
  }

} // end of namespace geomtools
