// -*- mode: c++; -*-
/// \file raw_hit.ipp

#ifndef RAW_HIT_IPP
#define RAW_HIT_IPP

// Ourselves:
#include <raw_hit.h>

// Third party:
// - Boost:
// Support for inheritance from an serializable class :
#include <boost/serialization/base_object.hpp>
// Support for XML 'key-value' based archives:
#include <boost/serialization/nvp.hpp>
// - Bayeux/datatools:
// Datatools support for serializable objects :
#include <datatools/i_serializable.ipp>
// Support serialization for the ``datatools::properties`` class :
#include <datatools/properties.ipp>

/** The main Boost/Serialization template method for class raw_hit
 *  @arg ar an archive type (ASCII, XML or binary)
 *  @arg version the archive version number (not used)
 */
template<class Archive>
void raw_hit::serialize(Archive & ar, const unsigned int version)
{
  ar & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
  ar & boost::serialization::make_nvp("id",          _id_);
  ar & boost::serialization::make_nvp("simulated",   _simulated_);
  ar & boost::serialization::make_nvp("channel",     _channel_);
  ar & boost::serialization::make_nvp("adc",         _adc_);
  ar & boost::serialization::make_nvp("tdc",         _tdc_);
  ar & boost::serialization::make_nvp("auxiliaries", _auxiliaries_);
  return;
}

#endif // RAW_HIT_IPP
