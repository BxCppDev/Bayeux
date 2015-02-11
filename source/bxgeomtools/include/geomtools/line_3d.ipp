// -*- mode: c++; -*-
//! \file geomtools/line_3d.ipp

#ifndef GEOMTOOLS_LINE_3D_IPP
#define GEOMTOOLS_LINE_3D_IPP 1

// Ourselves
#include <geomtools/line_3d.h>

// Third party:
// - Boost:
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
// - Bayeux/datatools:
// #include <datatools/i_serializable.ipp>
#include <datatools/enriched_base.ipp>

// This project:
#include <geomtools/utils.ipp>

namespace geomtools {

  template<class Archive>
  void line_3d::serialize (Archive & a_ar , const unsigned int a_version)
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

    a_ar & boost::serialization::make_nvp("first", _first_);
    a_ar & boost::serialization::make_nvp("last",  _last_);

    return;
  }

} // end of namespace geomtools

#endif // GEOMTOOLS_LINE_3D_IPP
