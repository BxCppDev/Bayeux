// -*- mode: c++; -*- 
/* any_shape_3d.cc 
 */

#include <geomtools/any_shape_3d.h>

namespace geomtools {

  using namespace std;

  const std::string any_shape_3d::ANY_SHAPE_3D_LABEL = "any_shape_3d";

  void any_shape_3d::compute_bounding_box (box & bb_)
  {
    bb_ = _bounding_box_;
    return;
  }

  void any_shape_3d::set_bounding_box (const box & bounding_box_)
  {
    _bounding_box_ = bounding_box_;
    return;
  }

  const box & any_shape_3d::get_bounding_box () const
  {
    return _bounding_box_;
  }

  string any_shape_3d::get_shape_name () const
  {
    return _shape_name_;
  }

  void any_shape_3d::set_shape_name (const string & shape_name_)
  {
    _shape_name_ = shape_name_;
    return;
  }

  // ctor:
  any_shape_3d::any_shape_3d () : i_shape_3d ()
  {
    _bounding_box_.reset ();
    return;
  }

  any_shape_3d::any_shape_3d (const string & shape_name_) : i_shape_3d (DEFAULT_TOLERANCE)
  {
    _bounding_box_.reset ();
    set_shape_name (shape_name_);
    return;
  }
  
  // dtor:
  any_shape_3d::~any_shape_3d ()
  {
    return;
  }

  bool 
  any_shape_3d::is_valid () const
  {
    return (! _shape_name_.empty ());
  }

  void 
  any_shape_3d::reset ()
  {
    _bounding_box_.reset ();
    _shape_name_= "";
    i_shape_3d::reset ();
    return;
  }

  bool 
  any_shape_3d::is_inside (const vector_3d & point_, double skin_) const
  {
    throw runtime_error ("any_shape_3d::is_inside: Not implemented !");
    return true;
  }

  vector_3d 
  any_shape_3d::get_normal_on_surface (const vector_3d & position_) const
  {
    vector_3d normal;
    throw runtime_error ("any_shape_3d::get_normal_on_surface: Not implemented !");
    geomtools::invalidate (normal);
    return (normal);
  }

  bool 
  any_shape_3d::is_on_surface (const vector_3d & point_ , 
		      int    mask_ ,
		      double skin_) const
  {
    bool debug = false;
    double skin = get_skin ();
    if (skin_ > USING_PROPER_SKIN) skin = 2 * skin_;
    throw runtime_error ("any_shape_3d::is_on_surface: Not implemented !");
    return false;
  }

  bool 
  any_shape_3d::find_intercept (const vector_3d & from_, 
		       const vector_3d & direction_,
		       intercept_t & intercept_,
		       double skin_) const
  {
    bool debug = false;
    intercept_.reset ();
    throw runtime_error ("any_shape_3d::is_on_surface: Not implemented !");
    return intercept_.is_ok ();
  }

} // end of namespace geomtools

// end of any_shape_3d.cc
