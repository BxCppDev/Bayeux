// -*- mode: c++; -*-
/// \file geomtools/polyline_3d.ipp

#ifndef GEOMTOOLS_POLYLINE_3D_IPP
#define GEOMTOOLS_POLYLINE_3D_IPP 1

// Third party:
// - Boost:
#include <boost/archive/archive_exception.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/list.hpp>
// - Bayeux/datatools:
#include <datatools/utils.h>

// This project:
#include <geomtools/polyline_3d.h>

namespace geomtools {

  /// Boost serialization template method
  template<class Archive>
  void polyline_3d::serialize (Archive & a_ar , const unsigned int a_version)
  {
    if (a_version > 0)
      {
        a_ar & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
      }
    a_ar & boost::serialization::make_nvp ("closed", _closed_);
    a_ar & boost::serialization::make_nvp ("points", _points_);
    return;
  }

} // end of namespace geomtools

#include <boost/serialization/version.hpp>
BOOST_CLASS_VERSION(geomtools::polyline_3d, 1)

#endif // GEOMTOOLS_POLYLINE_3D_IPP
