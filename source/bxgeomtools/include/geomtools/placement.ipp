// -*- mode: c++; -*-
//! \file geomtools/placement.ipp

#ifndef GEOMTOOLS_PLACEMENT_IPP_
#define GEOMTOOLS_PLACEMENT_IPP_ 1

#include <boost/archive/archive_exception.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>

#include <datatools/i_serializable.ipp>
#include <datatools/utils.h>
#include <geomtools/placement.h>
#include <geomtools/utils.ipp>
#include <geomtools/i_placement.ipp>

namespace geomtools {

  /// Boost serialization template method
  template<class Archive>
  void placement::serialize (Archive & ar_ , const unsigned int version_)
  {
    ar_ & BOOST_SERIALIZATION_BASE_OBJECT_NVP (i_placement);
    ar_ & boost::serialization::make_nvp ("translation",    _translation_);
    ar_ & boost::serialization::make_nvp ("rotation_axis",  _rotation_axis_);
    if (is_simple_rotation ())
      {
        ar_ & boost::serialization::make_nvp ("rotation_angle", _rotation_angle_);
      }
    else
      {
        ar_ & boost::serialization::make_nvp ("phi",   _phi_);
        ar_ & boost::serialization::make_nvp ("theta", _theta_);
        ar_ & boost::serialization::make_nvp ("delta", _delta_);
      }
    ar_ & boost::serialization::make_nvp ("rotation", _rotation_);
    ar_ & boost::serialization::make_nvp ("inverse_rotation", _inverse_rotation_);
    return;
  }

} // end of namespace geomtools

#endif // GEOMTOOLS_PLACEMENT_IPP_
