// -*- mode: c++; -*- 
/* i_shape_3d.cc
 */

#include <geomtools/i_shape_3d.h>

namespace geomtools {
 
  const double i_shape_3d::DEFAULT_SKIN = constants::DEFAULT_TOLERANCE;
  const unsigned int i_shape_3d::ALL_SURFACES = 0xFFFFFFFF;
  const unsigned int i_shape_3d::NO_SURFACES  = 0;
  const double i_shape_3d::USING_PROPER_SKIN = 0.0 * CLHEP::micrometer;

  double
  i_shape_3d::get_skin () const
  {
    return __skin;
  }
  
  void
  i_shape_3d::set_skin (double skin_)
  {
    if (skin_ <= 0.0)
      {
	throw std::runtime_error ("i_shape_3d::set_skin: skin value must be positive!");
      }
    __skin = skin_;
  }

  datatools::utils::properties & 
  i_shape_3d::properties ()
  {
    return __properties;
  }
  
  const datatools::utils::properties & 
  i_shape_3d::properties () const
  {
    return __properties;
  }
     
  i_shape_3d::i_shape_3d (double skin_)
  {
    set_skin (skin_);
  }
  
  i_shape_3d::~i_shape_3d ()
  {
  }

  bool
  i_shape_3d::is_outside (const vector_3d & pos_, 
			  double skin_) const
  {
    return ! is_inside (pos_, skin_) 
      && ! is_on_surface (pos_, ALL_SURFACES, skin_);
  }

} // end of namespace geomtools

// end of i_shape_3d.cc
