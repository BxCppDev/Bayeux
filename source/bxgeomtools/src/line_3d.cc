/// line_3d.cc

// Ourselves:
#include <geomtools/line_3d.h>

// Standard library:
#include <cmath>
#include <stdexcept>
#include <iostream>
#include <sstream>

// - Bayeux/datatools:
#include <datatools/exception.h>

// This project:
#include <geomtools/placement.h>

namespace geomtools {

  DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(line_3d, "geomtools::line_3d")

  const std::string & line_3d::line_3d_label()
  {
    static std::string label;
    if (label.empty()) {
      label = "line_3d";
    }
    return label;
  }

  bool line_3d::is_normal()
  {
    return std::isfinite(_first_.x()) &&
      std::isfinite(_first_.y()) &&
      std::isfinite(_first_.z()) &&
      std::isfinite(_last_.x()) &&
      std::isfinite(_last_.y()) &&
      std::isfinite(_last_.z());
  }

  std::string line_3d::get_shape_name() const
  {
    return line_3d::line_3d_label();
  }

  bool
  line_3d::is_valid() const
  {
    return (geomtools::is_valid(_first_) && geomtools::is_valid(_last_));
  }

  void
  line_3d::invalidate()
  {
    geomtools::invalidate(_first_);
    geomtools::invalidate(_last_);
    return;
  }

  const vector_3d &
  line_3d::get_first() const
  {
    return _first_;
  }

  void
  line_3d::set_first(double x_, double y_, double z_)
  {
    _first_.set(x_, y_, z_);
    return;
  }

  void
  line_3d::set_first(const vector_3d & a_first)
  {
    _first_ = a_first;
    return;
  }

  const vector_3d &
  line_3d::get_last() const
  {
    return _last_;
  }

  void
  line_3d::set_last(double x_, double y_, double z_)
  {
    _last_.set(x_, y_, z_);
    return;
  }

  void
  line_3d::set_last(const vector_3d & a_last)
  {
    _last_ = a_last;
    return;
  }

  double line_3d::get_length(uint32_t /* flags_ */) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid segment!");
    double l = (_last_ - _first_).mag();
    return l;
  }

  line_3d::line_3d() : i_shape_1d()
  {
    invalidate();
    return;
  }

  line_3d::line_3d(const vector_3d & a_first, const vector_3d & a_last)
  {
    set_first(a_first);
    set_last(a_last);
    return;
  }

  line_3d::line_3d(double x0_, double y0_, double z0_,
                   double x1_, double y1_, double z1_)
  {
    set_first(vector_3d(x0_, y0_, z0_));
    set_last(vector_3d(x1_, y1_, z1_));
    return;
  }

  line_3d::line_3d(const segment_type & segment_)
  {
    set_first(segment_.first);
    set_last(segment_.second);
    return;
  }

  line_3d::~line_3d()
  {
    return;
  }

  vector_3d
  line_3d::get_point(double a_t) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid segment!");
    vector_3d p = _first_ + (_last_ - _first_) * a_t;
    return vector_3d(p);
  }

  void
  line_3d::make_vertex_collection(polyline_type & a_bpl) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid segment!");
    a_bpl.clear();
    a_bpl.push_back(_first_);
    a_bpl.push_back(_last_);
    return;
  }

  polyline_type
  line_3d::make_vertex_collection() const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid segment!");
    polyline_type bpl;
    make_vertex_collection(bpl);
    return bpl;
  }

  void line_3d::tree_dump(std::ostream &      out_,
                          const std::string & title_,
                          const std::string & indent_,
                          bool           inherit_) const
  {
    std::string indent;
    if (! indent_.empty()) {
      indent = indent_;
    }
    i_object_3d::tree_dump(out_, title_, indent_, true);
    out_ << indent << datatools::i_tree_dumpable::tag
         << "First : "
         << _first_
         << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag
         << "Last : "
         << _last_
         << std::endl;
    out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_)
         << "Length : "
         << get_length() /* / CLHEP::mm << " mm"*/
         << std::endl;
    return;
  }


  void line_3d::print_xyz(std::ostream & out_,
                          const line_3d & a_line)
  {
    DT_THROW_IF(! a_line.is_valid(), std::logic_error, "Invalid segment!");
    vector_3d first = a_line.get_first();
    vector_3d last  = a_line.get_last();

    out_ << first.x() << " " << first.y() << " " << first.z() << std::endl;
    out_ << last.x() << " " << last.y() << " " << last.z() << std::endl;
    return;
  }

  // static
  void line_3d::orthogonal_projection(const vector_3d & p_,
                                      const vector_3d & m_,
                                      const vector_3d & u_,
                                      vector_3d & h_)
  {
    vector_3d mh = (p_ - m_).dot(u_) * u_ / u_.mag2();
    h_ = m_ + mh;
    return;
  }

  // static
  double line_3d::distance_to_line(const vector_3d & p_,
                                   const vector_3d & m_,
                                   const vector_3d & u_)
  {
    double d;
    datatools::invalidate(d);
    d = ((p_ - m_).cross(u_)).mag() / u_.mag();
    return d;
  }

  bool line_3d::compute_orthogonal_projection(const vector_3d & position_,
                                              vector_3d & proj_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid segment!");
    geomtools::invalidate(proj_);
    vector_3d proj;
    geomtools::line_3d::orthogonal_projection(position_,
                                              get_first(),
                                              get_last() - get_first(),
                                              proj);
    if (geomtools::is_valid(proj)) {
      vector_3d vfl = get_last() - get_first();
      double fl = vfl.mag();
      vector_3d ufl = vfl.unit();
      double fh = (proj - get_first()).dot(ufl);
      if (fh >= 0.0 && fh <= fl) {
        proj_ = proj;
      }
    }
    return geomtools::is_valid(proj_);
  }

  //
  //
  //  P
  //   +
  //    \' .
  //     \   ' .  A
  //   B  \      ' .
  //       \         ' .
  //        +------------+
  //      F       u       L
  //
  //
  double line_3d::get_distance_to_line(const vector_3d & position_ ) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid segment!");
    vector_3d h;
    // geomtools::line_3d::orthogonal_projection(position_,
    //                                        get_first(),
    //                                        get_last() - get_first(),
    //                                        h);

    vector_3d A = position_ - _last_;
    vector_3d B = position_ - _first_;

    vector_3d u = _last_ - _first_;
    vector_3d d = A.cross( position_ );

    double dist = distance_to_line(position_, _first_, _last_ - _first_);
    return dist;
  }

  bool line_3d::is_on_line(const vector_3d & position_,
                           double tolerance_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid segment!");
    bool on_line = false;
    double tolerance = compute_tolerance(tolerance_);
    double d = distance_to_line(position_, _first_, _last_ - _first_);
    if (datatools::is_valid(d)) {
      on_line = (d < 0.5 * tolerance);
    }
    return on_line;
  }

  bool line_3d::is_on_segment(const vector_3d & position_,
                              double tolerance_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid segment!");
    bool on_segment = false;
    double tolerance = compute_tolerance(tolerance_);
    double half_tolerance = 0.5 * tolerance;
    vector_3d proj; // orthogonal projection
    compute_orthogonal_projection(position_, proj);
    if(geomtools::is_valid(proj)) {
      on_segment = ((position_ - proj).mag() <= half_tolerance);
    }
    return on_segment;
  }

  // virtual
  bool line_3d::is_on_curve(const vector_3d & position_,
                            double tolerance_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid segment!");
    bool on_curve = is_on_segment(position_, tolerance_);
    return on_curve;
  }

  vector_3d line_3d::get_direction_on_curve(const vector_3d & position_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid segment!");
    vector_3d dir;
    geomtools::invalidate(dir);
    vector_3d proj; // orthogonal projection
    compute_orthogonal_projection(position_, proj);
    if(geomtools::is_valid(proj)) {
      dir = (_last_ - _first_).unit();
    }
    return dir;
  }

  void line_3d::generate_wires_self(wires_type & wires_,
                                    uint32_t /* options_ */) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid segment!");
    {
      polyline_type dummy;
      wires_.push_back(dummy);
    }
    polyline_type & wire = wires_.back();
    wire.push_back(_first_);
    wire.push_back(_last_);
    return;
  }

} // end of namespace geomtools
