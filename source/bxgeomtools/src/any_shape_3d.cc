// -*- mode: c++; -*- 
/* any_shape_3d.cc 
 */

#include <geomtools/any_shape_3d.h>

namespace geomtools {

  using namespace std;

  const std::string any_shape_3d::ANY_SHAPE_3D_LABEL = "any_shape_3d";

  void any_shape_3d::compute_bounding_box (box & bb_)
  {
    bb_ = __bounding_box;
  }

  void any_shape_3d::set_bounding_box (const box & bounding_box_)
  {
    __bounding_box = bounding_box_;
  }

  const box & any_shape_3d::get_bounding_box () const
  {
    return __bounding_box;
  }

  string any_shape_3d::get_shape_name () const
  {
    return __shape_name;
  }

  void any_shape_3d::set_shape_name (const string & shape_name_)
  {
    __shape_name = shape_name_;
  }

  // ctor:
  any_shape_3d::any_shape_3d () : i_shape_3d ()
  {
    __bounding_box.reset ();
  }

  any_shape_3d::any_shape_3d (const string & shape_name_) : i_shape_3d (DEFAULT_TOLERANCE)
  {
    __bounding_box.reset ();
    set_shape_name (shape_name_);
  }
  
  // dtor:
  any_shape_3d::~any_shape_3d ()
  {
  }

  bool 
  any_shape_3d::is_valid () const
  {
    return (! __shape_name.empty ());
  }

  void 
  any_shape_3d::reset ()
  {
    __bounding_box.reset ();
    __shape_name= "";
    i_shape_3d::reset ();
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
