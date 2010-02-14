// -*- mode: c++; -*- 
/* i_shape_3d.cc
 */

#include <geomtools/i_shape_3d.h>

namespace geomtools {

  const double i_shape_3d::DEFAULT_SKIN = i_object_3d::DEFAULT_TOLERANCE;
  const double i_shape_3d::USING_PROPER_SKIN = i_object_3d::USING_PROPER_TOLERANCE;
  
  double i_shape_3d::get_skin () const
  {
    return get_tolerance ();
  }
   
  void i_shape_3d::set_skin (double skin_)
  {
    set_tolerance (skin_);
  }
 
  int i_shape_3d::get_dimensional () const
  {
    return DIMENSIONAL_3;
  }
     
  i_shape_3d::i_shape_3d (double skin_) : i_object_3d (skin_)
  {
  }
  
  i_shape_3d::~i_shape_3d ()
  {
  }

  bool i_shape_3d::is_outside (const vector_3d & pos_, 
			       double skin_) const
  {
    return ! is_inside (pos_, skin_) 
      && ! is_on_surface (pos_, ALL_SURFACES, skin_);
  }

} // end of namespace geomtools

// end of i_shape_3d.cc
