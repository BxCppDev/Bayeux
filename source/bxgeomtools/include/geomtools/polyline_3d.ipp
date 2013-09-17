// -*- mode: c++; -*-
//! \file geomtools/polyline_3d.ipp

#ifndef __geomtools__polyline_3d_ipp
#define __geomtools__polyline_3d_ipp 1

#include <boost/archive/archive_exception.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/list.hpp>

#include <datatools/utils.h>
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

#endif // __geomtools__polyline_3d_ipp

// end of polyline_3d.ipp
