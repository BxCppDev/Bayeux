//! \file geomtools/angular_range.ipp

#ifndef GEOMTOOLS_ANGULAR_RANGE_IPP
#define GEOMTOOLS_ANGULAR_RANGE_IPP 1

// Ourselves:
#include <geomtools/angular_range.h>

// Third party:
// - Boost:
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
// - Bayeux/datatools:
#include <datatools/i_serializable.ipp>
#include <datatools/compiler_macros.h>

namespace geomtools {

  //! Boost serialization template method
  template<class Archive>
  void angular_range::serialize(Archive & ar_ , const unsigned int DT_UNUSED(version_))
  {
    ar_ & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
    ar_ & boost::serialization::make_nvp("type",        _type_);
    ar_ & boost::serialization::make_nvp("start_angle", _start_angle_);
    ar_ & boost::serialization::make_nvp("delta_angle", _delta_angle_);
    return;
  }

} // end of namespace geomtools

#endif // GEOMTOOLS_ANGULAR_RANGE_IPP


// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
