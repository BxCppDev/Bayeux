// -*- mode: c++; -*- 
/* i_shape_1d.cc
 */
 
#include <geomtools/i_shape_1d.h>

namespace geomtools {

  int i_shape_1d::get_dimensional () const
  {
    return DIMENSIONAL_1;
  }
     
  i_shape_1d::i_shape_1d (double tolerance_) : i_object_3d (tolerance_)
  {
  }
  
  i_shape_1d::~i_shape_1d ()
  {
  }

} // end of namespace geomtools

// end of i_shape_1d.cc
