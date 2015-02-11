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
    if (Archive::is_loading::value) {
      if (a_version == 1) {
        /* From version 1 we inherit explicitely from the
         * 'datatools::serialization::i_serializable' abstract class
         */
        a_ar & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
      }
    }
    if (a_version >= 2 || Archive::is_saving::value) {
      /* From version 2 we inherit explicitely from the
       * 'geomtools::i_shape_1d' abstract class
       */
      a_ar & boost::serialization::make_nvp("geomtools__i_shape_1d",
                                            boost::serialization::base_object<geomtools::i_shape_1d>(*this));
    }

    a_ar & boost::serialization::make_nvp ("closed", _closed_);
    a_ar & boost::serialization::make_nvp ("points", _points_);

    return;
  }

} // end of namespace geomtools

#endif // GEOMTOOLS_POLYLINE_3D_IPP
