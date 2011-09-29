// -*- mode: c++; -*- 
/* line_3d.cc
 */
 
#include <geomtools/line_3d.h>

#include <cmath>
#include <stdexcept>
#include <iostream>
#include <sstream>

namespace geomtools {

  using namespace std;
 
  DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION (line_3d,"geomtools::line_3d")

  const string line_3d::LINE_3D_LABEL = "line_3d";

  bool line_3d::is_normal ()
  {
    return isfinite (__first.x()) &&
      isfinite (__first.y()) &&
      isfinite (__first.z()) &&
      isfinite (__last.x()) &&
      isfinite (__last.y()) &&
      isfinite (__last.z());
  }

  string line_3d::get_shape_name () const
  {
    return line_3d::LINE_3D_LABEL;
  }

  bool 
  line_3d::is_valid () const
  {
    return (geomtools::is_valid (__first) && geomtools::is_valid (__last));
  }
  
  void 
  line_3d::invalidate ()
  {
    geomtools::invalidate (__first);
    geomtools::invalidate (__last);
    return;
  }
  
  const vector_3d & 
  line_3d::get_first () const
  {
    return __first;
  }
  
  void 
  line_3d::set_first (const vector_3d & a_first)
  {
    __first = a_first;
    return;
  }
  
  const vector_3d & 
  line_3d::get_last () const
  {
    return __last;
  }
  
  void 
  line_3d::set_last (const vector_3d & a_last)
  {
    __last = a_last;
    return;
  }
  
  double line_3d::get_length () const
  {
    double l = (__last - __first).mag ();
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
    vector_3d p = __first + (__last - __first) * a_t;
    return vector_3d (p);
  }

  void 
  line_3d::make_vertex_collection (basic_polyline_3d & a_bpl) const
  {
    a_bpl.clear ();
    a_bpl.push_back (__first);
    a_bpl.push_back (__last);
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
    namespace du = datatools::utils;
    string indent;
    if (! indent_.empty ())
      {
        indent = indent_;
      }
    i_object_3d::tree_dump (out_, title_, indent_, true);
    out_ << indent << du::i_tree_dumpable::tag 
	 << "First : " 
	 << __first 
	 << endl;
    out_ << indent << du::i_tree_dumpable::tag
	 << "Last : " 
	 << __last
	 << endl;
    out_ << indent << du::i_tree_dumpable::inherit_tag (inherit_) 
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


  bool line_3d::is_on_curve (const vector_3d & position_, 
			     double tolerance_) const
  {
    bool on_curve = false;
    throw logic_error ("line_3d::is_on_curve: Not implemented yet !");
    return on_curve;
  }

  vector_3d line_3d::get_direction_on_curve (const vector_3d & position_) const
  {
    vector_3d dir;
    throw logic_error ("line_3d::get_direction_on_curve: Not implemented yet !");
    return dir;
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
