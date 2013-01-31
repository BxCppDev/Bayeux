// -*- mode: c++; -*- 
/* line_3d.cc
 */
 
#include <geomtools/line_3d.h>

#include <cmath>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <geomtools/polyline_3d.h>
#include <geomtools/placement.h>

namespace geomtools {

  using namespace std;
 
  DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION (line_3d,"geomtools::line_3d")

  const string line_3d::LINE_3D_LABEL = "line_3d";

  bool line_3d::is_normal ()
  {
    return isfinite (_first_.x()) &&
      isfinite (_first_.y()) &&
      isfinite (_first_.z()) &&
      isfinite (_last_.x()) &&
      isfinite (_last_.y()) &&
      isfinite (_last_.z());
  }

  string line_3d::get_shape_name () const
  {
    return line_3d::LINE_3D_LABEL;
  }

  bool 
  line_3d::is_valid () const
  {
    return (geomtools::is_valid (_first_) && geomtools::is_valid (_last_));
  }
  
  void 
  line_3d::invalidate ()
  {
    geomtools::invalidate (_first_);
    geomtools::invalidate (_last_);
    return;
  }
  
  const vector_3d & 
  line_3d::get_first () const
  {
    return _first_;
  }
  
  void 
  line_3d::set_first (const vector_3d & a_first)
  {
    _first_ = a_first;
    return;
  }
  
  const vector_3d & 
  line_3d::get_last () const
  {
    return _last_;
  }
  
  void 
  line_3d::set_last (const vector_3d & a_last)
  {
    _last_ = a_last;
    return;
  }
  
  double line_3d::get_length () const
  {
    double l = (_last_ - _first_).mag ();
    return l;
  }
  
  // ctor/dtor:
  line_3d::line_3d () : i_shape_1d ()
  {
    invalidate ();
    return;
  }

  // ctor/dtor:
  line_3d::line_3d (const vector_3d & a_first, const vector_3d & a_last)
  {
    set_first (a_first);
    set_last (a_last);
    return;
  }
  
  line_3d::~line_3d ()
  {
    return;
  }

  vector_3d 
  line_3d::get_point (double a_t) const
  {
    vector_3d p = _first_ + (_last_ - _first_) * a_t;
    return vector_3d (p);
  }

  void 
  line_3d::make_vertex_collection (basic_polyline_3d & a_bpl) const
  {
    a_bpl.clear ();
    a_bpl.push_back (_first_);
    a_bpl.push_back (_last_);
    return;
  }

  basic_polyline_3d 
  line_3d::make_vertex_collection () const
  {
    basic_polyline_3d bpl;
    make_vertex_collection (bpl);
    return bpl;
  }

  void line_3d::dump () const
  {
    tree_dump (clog);
    return;
  }
  
  void line_3d::tree_dump (ostream &      out_, 
                           const string & title_,
                           const string & indent_,
                           bool           inherit_) const
  {
    string indent;
    if (! indent_.empty ())
      {
        indent = indent_;
      }
    i_object_3d::tree_dump (out_, title_, indent_, true);
    out_ << indent << datatools::i_tree_dumpable::tag 
         << "First : " 
         << _first_ 
         << endl;
    out_ << indent << datatools::i_tree_dumpable::tag
         << "Last : " 
         << _last_
         << endl;
    out_ << indent << datatools::i_tree_dumpable::inherit_tag (inherit_) 
         << "Length : " 
         << get_length() /* / CLHEP::mm << " mm"*/
         << endl;
    return;
  }


  void line_3d::print_xyz (ostream & out_, 
                           const line_3d & a_line)
  {
    vector_3d first = a_line.get_first();
    vector_3d last  = a_line.get_last();

    out_ << first.x() << " " << first.y() << " " << first.z() << endl;
    out_ << last.x() << " " << last.y() << " " << last.z() << endl;
    return;
  }

  double line_3d::get_distance_to_line ( const vector_3d & position_ ) const
  {
    vector_3d A = position_ - _last_;
    vector_3d B = position_ - _first_;
    
    vector_3d u = _last_ - _first_;
    vector_3d d = A.cross( position_ );
    
    double dist = d.mag() / u.mag();
    
    if ( dist < A.mag() && dist < B.mag() )
      return dist;
    else if ( A.mag() < dist && A.mag() < B.mag() )
      return A.mag();
    else
      return B.mag();
  }
 
  bool line_3d::is_on_curve ( const vector_3d & position_, 
                              double tolerance_ ) const
  {
    return get_distance_to_line ( position_ ) <= tolerance_;
  }

  vector_3d line_3d::get_direction_on_curve (const vector_3d & position_) const
  {
    vector_3d dir;
    throw logic_error ("line_3d::get_direction_on_curve: Not implemented yet !");
    return dir;
  }

  void line_3d::generate_wires (std::list<polyline_3d> & lpl_, 
                                const placement & p_) const
  {
    {
      polyline_3d dummy;
      lpl_.push_back (dummy);
    }
    polyline_3d & pl = lpl_.back ();
    pl.set_closed (false);
    vector_3d v;
    p_.child_to_mother (_first_, v);
    pl.add (v);
    p_.child_to_mother (_last_, v);
    pl.add (v);
    return;
  }

  /*
    void 
    line_3d::print_xyz (ostream & out_, 
    const line_3d & a_line,
    double step_)
    {
    double delta_t = 0.01;
    if (step_ > 0.0) delta_t = step_;
    for (double t = 0;
    t < 1.0 + 0.001 * delta_t;
    t += delta_t)
    {
    vector_3d v = a_line.get_point (t);
    vector_3d::print_xyz (out_, v);
    }
    out_ << endl << endl;
    }
  */
  
} // end of namespace geomtools

// end of line_3d.cc
