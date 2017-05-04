// -*- mode: c++; -*-
/// \file raw_data.ipp

#ifndef DPP_EX01_RAW_DATA_IPP_
#define DPP_EX01_RAW_DATA_IPP_ 1

#include <dpp_ex01/raw_data.h>

// Support for inheritance from an serializable class :
#include <boost/serialization/base_object.hpp>

// Support for XML 'key-value' based archives:
#include <boost/serialization/nvp.hpp>

// Support for std::vector serialization:
#include <boost/serialization/vector.hpp>

// Datatools support for serializable objects :
#include <datatools/i_serializable.ipp>
#include <datatools/properties.ipp>

#include <dpp_ex01/hit.ipp>

namespace dpp_ex01 {

  /** The main Boost/Serialization template method for class raw_data
   *  @arg ar an archive type (ASCII, XML or binary)
   *  @arg version the archive version number (not used)
   */
  template<class Archive>
  void raw_data::serialize (Archive & ar, const unsigned int version)
  {
    ar & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
    ar & boost::serialization::make_nvp ("hits",        _hits_);
    ar & boost::serialization::make_nvp ("auxiliaries", _auxiliaries_);
    return;
  }

} // namespace dpp_ex01

// Class versioning :
// #include <boost/serialization/version.hpp>
// BOOST_CLASS_VERSION(dpp_ex01::raw_data, 1)

#endif // DPP_EX01_RAW_DATA_IPP_
