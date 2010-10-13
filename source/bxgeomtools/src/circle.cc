// -*- mode: c++ ; -*- 
/* circle.cc
 */

#include <geomtools/circle.h>

namespace geomtools {

  using namespace std;

  const string circle::CIRCLE_LABEL = "circle";

  string circle::get_shape_name () const
  {
    return CIRCLE_LABEL;
  }
  
  double circle::get_r () const
  {
    return __r;
  }
  
  double circle::get_radius () const
  {
    return get_r ();
  }
 
  void circle::set_diameter (double new_value_)
  {
    set_r (new_value_ * 0.5);
  }

  double circle::get_diameter () const
  {
    return (__r + __r);
  }
  
  void circle::set_r (double new_value_)
  {
    if (new_value_ < 0.0 )
      {
	ostringstream message;
	message << "circle::set_r: Invalid '" << new_value_ << "' R value!";
	throw logic_error (message.str ());
      }
    __r = new_value_;
  }
   
  double circle::get_surface () const
  {
    return M_PI * __r * __r;
  }

  double circle::get_circumference () const
  {
    return 2 * M_PI * __r;
  }
  
  bool circle::is_valid () const
  {
    return (__r > 0.0);
  }
  
  // ctor:
  circle::circle ()
  {
    __r = -1.0;
  }

  // ctor:
  circle::circle (double r_)
  {
    set_r (r_);
  }
  
  // dtor:
  circle::~circle ()
  {
  }

  void circle::tree_dump (ostream & out_, 
			const string & title_, 
			const string & indent_, 
			bool inherit_) const
  {
    namespace du = datatools::utils;
    string indent;
    if (! indent_.empty ()) indent = indent_;
    i_object_3d::tree_dump (out_, title_, indent_, true);

    out_ << indent << du::i_tree_dumpable::inherit_tag (inherit_)  
	 << "R : " << get_r () / CLHEP::mm << " mm" << endl;
    return;
  }

  bool circle::is_on_curve (const vector_3d &, 
			    double tolerance_) const
  {
    bool on_curve = false;
    throw runtime_error ("circle::is_on_curve: Not implemented yet !");
    return on_curve;
  }

  vector_3d circle::get_direction_on_curve (const vector_3d & position_) const
  {
    vector_3d dir;
    invalidate (dir);
    throw runtime_error ("circle::get_direction_on_curve: Not implemented yet !");
    return dir;
  }
  
} // end of namespace geomtools

// end of circle.cc
