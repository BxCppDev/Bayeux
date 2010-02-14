// -*- mode: c++; -*- 
/* i_object_3d.cc
 */

#include <geomtools/i_object_3d.h>

namespace geomtools {

  using namespace std;

  const double i_object_3d::DEFAULT_TOLERANCE      = constants::DEFAULT_TOLERANCE;
  const double i_object_3d::USING_PROPER_TOLERANCE = 0.0;
  
  const uint32_t i_object_3d::ALL_SURFACES = 0xFFFFFFFF;
  const uint32_t i_object_3d::NO_SURFACES  = 0;
   
  double i_object_3d::get_tolerance () const
  {
    return __tolerance;
  }

  void i_object_3d::set_tolerance (double tolerance_)
  {
    if (tolerance_ <= 0.0)
      {
	throw runtime_error ("i_object_3d::set_tolerance: tolerance value must be positive !");
      }
    __tolerance = tolerance_;
  }

  datatools::utils::properties & i_object_3d::properties ()
  {
    return __properties;
  }
  
  const datatools::utils::properties & i_object_3d::properties () const
  {
    return __properties;
  }

  i_object_3d::i_object_3d (double tolerance_)
  {
    __tolerance = DEFAULT_TOLERANCE;
    set_tolerance (tolerance_);
  }

  i_object_3d::~i_object_3d ()
  {
  }

} // end of namespace geomtools

// end of i_object_3d.cc
