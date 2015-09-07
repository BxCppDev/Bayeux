// -*- mode: c++; -*-
/// \file raw_data.ipp

#ifndef RAW_DATA_IPP
#define RAW_DATA_IPP

// Ourselves:
#include <raw_data.h>

// Third party:
// - Boost:
// Support for inheritance from an serializable class :
#include <boost/serialization/base_object.hpp>
// Support for XML name-value based archives :
#include <boost/serialization/nvp.hpp>
// Support for serialization of ``std::string`` :
#include <boost/serialization/string.hpp>
// Support for serialization of ``std::vector`` :
#include <boost/serialization/vector.hpp>
// - Bayeux/datatools:
// Support for serializable objects :
#include <datatools/i_serializable.ipp>
// Support for serialization of ``datatools::properties`` :
#include <datatools/properties.ipp>

// This project:
// Support for raw_hit
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

#endif // RAW_DATA_IPP
