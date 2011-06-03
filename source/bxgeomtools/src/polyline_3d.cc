// -*- mode: c++; -*- 
/* polyline_3d.cc 
 */

#include <geomtools/polyline_3d.h>

namespace geomtools {

  using namespace std;
 
  DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION (polyline_3d,"geomtools::polyline_3d")

  //const string polyline_3d::SERIAL_TAG = "__geomtools::polyline_3d__";

  const string polyline_3d::POLYLINE_3D_LABEL = "line_3d";

  string polyline_3d::get_shape_name () const
  {
    return polyline_3d::POLYLINE_3D_LABEL;
  }

  bool  
  polyline_3d::is_closed () const
  {
    return __closed;
  }
  
  void 
  polyline_3d::set_closed (bool closed_)
  {
    __closed = closed_;
    return;
  }

  polyline_3d::polyline_3d ()
  {
    __closed = DEFAULT_CLOSED;
    return;
  }

  polyline_3d::polyline_3d (bool closed_)
  {
    set_closed (closed_);
    return;
  }

  polyline_3d::~polyline_3d ()
  {
    __points.clear ();
    return;
  }

  void 
  polyline_3d::clear ()
  {
    __closed = DEFAULT_CLOSED;
    __points.clear ();    
    return;
  }
      
  void 
  polyline_3d::add (const vector_3d & p_)
  {
    __points.push_back (p_);
    return;
  }

  bool 
  polyline_3d::is_empty () const
  {
    return __points.size () == 0;
  }

  int 
  polyline_3d::get_number_of_points () const
  {
    return __points.size ();
  }
      
  const vector_3d & 
  polyline_3d::get_point (int i_) const
  {
    if (is_empty ())
      {
	throw runtime_error ("polyline_3d::get_point: Empty point collection!");
      }
    if ((i_ < 0) || (i_ >= (int) __points.size ()))
      {
	throw runtime_error ("polyline_3d::get_point: Invalid point index!");
      }
    int i = 0;
    point_col::const_iterator it = __points.begin ();
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
    int n = __points.size ();
    if ((is_closed ()) && (n > 0)) n++;
    return n;
  }
 
  const vector_3d & 
  polyline_3d::get_vertex (int i_) const
  {
    if (is_empty ())
      {
	throw runtime_error ("polyline_3d::get_vertex: Empty vertex collection!");
      }

    if (! is_closed ())
      {
	return get_point (i_);
      }
    
    if ((i_ < 0) || (i_ >= get_number_of_vertex ()))
      {
	throw runtime_error ("polyline_3d::get_vertex: Invalid vertex index!");
      }
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
    throw runtime_error ("polyline_3d::is_on_curve: Not implemented yet !");
    return on_curve;
  }

  vector_3d polyline_3d::get_direction_on_curve (const vector_3d & position_) const
  {
    vector_3d dir;
    throw runtime_error ("polyline_3d::get_direction_on_curve: Not implemented yet !");
    return dir;
  }

} // end of namespace geomtools

// end of polyline_3d.cc
