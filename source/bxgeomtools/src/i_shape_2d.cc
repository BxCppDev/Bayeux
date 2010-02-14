// -*- mode: c++; -*- 
/* i_shape_2d.cc
 */

#include <geomtools/i_shape_2d.h>

namespace geomtools {

  int i_shape_2d::get_dimensional () const
  {
    return DIMENSIONAL_2;
  }
      
  i_shape_2d::i_shape_2d (double skin_) : i_object_3d (skin_)
  {
  }
  
  i_shape_2d::~i_shape_2d ()
  {
  }

} // end of namespace geomtools

// end of i_shape_2d.cc
