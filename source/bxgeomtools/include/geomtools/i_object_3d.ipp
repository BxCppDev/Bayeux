// -*- mode: c++; -*-
/// \file geomtools/i_object_3d.ipp

#ifndef GEOMTOOLS_I_OBJECT_3D_IPP
#define GEOMTOOLS_I_OBJECT_3D_IPP 1

// Ourselves:
#include <geomtools/i_object_3d.h>

// Third party:
// - Boost:
#include <boost/archive/archive_exception.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>

namespace geomtools {

  /// Boost serialization template method
  template<class Archive>
  void i_object_3d::serialize(Archive & a_ar , const unsigned int /* a_version */)
  {
    a_ar & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
    a_ar & boost::serialization::make_nvp("tolerance", _tolerance_);
    a_ar & boost::serialization::make_nvp("angular_tolerance", _angular_tolerance_);
    return;
  }

} // end of namespace geomtools

#endif // GEOMTOOLS_I_OBJECT_3D_IPP
