// -*- mode: c++; -*-
/// \file raw_data.ipp

#ifndef RAW_DATA_IPP_
#define RAW_DATA_IPP_ 1

#include <raw_data.h>

// Support for inheritance from an serializable class :
#include <boost/serialization/base_object.hpp>

// Support for XML 'key-value' based archives:
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>

// Datatools support for serializable objects :
#include <datatools/i_serializable.ipp>

// Support serialization for the ``datatools::properties`` class :
#include <datatools/properties.ipp>

#include <raw_hit.ipp>

/** The main Boost/Serialization template method for class raw_data
 *  @arg ar an archive type (ASCII, XML or binary)
 *  @arg version the archive version number (not used)
 */
template<class Archive>
void raw_data::serialize(Archive & ar, const unsigned int version)
{
  ar & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
  ar & boost::serialization::make_nvp("locked",        _locked_);
  ar & boost::serialization::make_nvp("lock_password", _lock_password_);
  ar & boost::serialization::make_nvp("hits",          _hits_);
  ar & boost::serialization::make_nvp("auxiliaries",   _auxiliaries_);
  return;
}

#endif // RAW_DATA_IPP_

