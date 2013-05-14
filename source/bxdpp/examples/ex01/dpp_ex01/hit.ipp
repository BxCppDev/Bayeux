// -*- mode: c++; -*-
/// \file hit.ipp

#ifndef DPP_EX01_HIT_IPP_
#define DPP_EX01_HIT_IPP_ 1

#include <dpp_ex01/hit.h>

// Support for inheritance from an serializable class :
#include <boost/serialization/base_object.hpp>

// Support for XML 'key-value' based archives:
#include <boost/serialization/nvp.hpp>

// Datatools support for serializable objects :
#include <datatools/i_serializable.ipp>
#include <datatools/properties.ipp>

namespace dpp_ex01 {

  /** The main Boost/Serialization template method for class hit
   *  @arg ar an archive type (ASCII, XML or binary)
   *  @arg version the archive version number (not used)
   */
  template<class Archive>
  void hit::serialize (Archive & ar, const unsigned int version)
  {
    ar & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
    ar & boost::serialization::make_nvp ("id",          _id_);
    ar & boost::serialization::make_nvp ("energy",      _energy_);
    ar & boost::serialization::make_nvp ("auxiliaries", _auxiliaries_);
    return;
  }

} // namespace dpp_ex01

#endif // DPP_EX01_HIT_IPP_
