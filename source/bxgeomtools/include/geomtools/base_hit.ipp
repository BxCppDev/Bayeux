// -*- mode: c++; -*-
/// \file geomtools/base_hit.ipp

#ifndef GEOMTOOOLS_BASE_HIT_IPP
#define GEOMTOOOLS_BASE_HIT_IPP 1

// Ourselves:
#include <geomtools/base_hit.h>

// Third party:
// - Boost:
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
// - Bayeux/datatools:
#include <datatools/i_serializable.ipp>
#include <datatools/properties.ipp>

// This project :
#include <geomtools/geom_id.ipp>

namespace geomtools {

  template<class Archive>
  void base_hit::serialize (Archive & ar,
                            const unsigned int version)
  {
    if (version > 0)
      {
        ar & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
      }
    ar & boost::serialization::make_nvp ("store",       _store);
    if (_store & STORE_HIT_ID)
      {
        ar & boost::serialization::make_nvp ("hit_id",      _hit_id_);
      }
    if (_store & STORE_GEOM_ID)
      {
        ar & boost::serialization::make_nvp ("geom_id",     _geom_id_);
      }
    if (_store & STORE_AUXILIARIES)
      {
        ar & boost::serialization::make_nvp ("auxiliaries", _auxiliaries_);
      }
    return;
  }

} // end of namespace geomtools

#include <boost/serialization/version.hpp>
BOOST_CLASS_VERSION(geomtools::base_hit, 1)

#endif // GEOMTOOOLS_BASE_HIT_IPP
