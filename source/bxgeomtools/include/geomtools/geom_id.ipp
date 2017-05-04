/// \file geomtools/geom_id.ipp

#ifndef GEOMTOOLS_GEOM_ID_IPP
#define GEOMTOOLS_GEOM_ID_IPP 1

// Third party:
// - Boost:
#include <boost/archive/archive_exception.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/vector.hpp>
// - Bayeux/datatools :
#include <datatools/utils.h>
#include <datatools/i_serializable.ipp>

// This project :
#include <geomtools/geom_id.h>

namespace geomtools {

  template<class Archive>
  void geom_id::serialize (Archive & a_ar , const unsigned int a_version)
  {
    if (a_version > 0) {
      /* from version 1 we inherit explicitely from the
       * 'datatools::serialization::i_serializable' abstract class
       */
      a_ar & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
    }
    a_ar & boost::serialization::make_nvp ("type",    _type_);
    a_ar & boost::serialization::make_nvp ("address", _addresses_);
    return;
  }

} // end of namespace geomtools

#endif // GEOMTOOLS_GEOM_ID_IPP

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
