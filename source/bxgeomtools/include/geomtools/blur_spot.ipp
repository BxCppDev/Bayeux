// -*- mode: c++; -*-
//! \file geomtools/blur_spot.ipp

#ifndef __geomtools__blur_spot_ipp
#define __geomtools__blur_spot_ipp 1

#include <boost/archive/archive_exception.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>

#include <geomtools/blur_spot.h>
#include <datatools/i_serializable.ipp>
#include <datatools/properties.ipp>
#include <geomtools/placement.ipp>

namespace geomtools {

  /// Boost serialization template method
  template<class Archive>
  void blur_spot::serialize (Archive & ar_ , const unsigned int /*version_*/)
  {
    ar_ & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
    ar_ & boost::serialization::make_nvp ("blur_dimension", _blur_dimension_);
    ar_ & boost::serialization::make_nvp ("tolerance",  _tolerance_);
    ar_ & boost::serialization::make_nvp ("placement",  _placement_);
    ar_ & boost::serialization::make_nvp ("auxiliaries",  _auxiliaries_);
    if (is_dimension_two () || is_dimension_three ())
      {
        ar_ & boost::serialization::make_nvp ("x_error",  _x_error_);
        ar_ & boost::serialization::make_nvp ("y_error",  _y_error_);
      }
    if (is_dimension_one () || is_dimension_three ())
      {
        ar_ & boost::serialization::make_nvp ("z_error",  _z_error_);
      }
    return;
  }

} // end of namespace geomtools

#endif // __geomtools__blur_spot_ipp

