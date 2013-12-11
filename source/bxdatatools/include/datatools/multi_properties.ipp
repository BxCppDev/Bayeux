// -*- mode: c++; -*-
//! \file datatools/multi_properties.ipp
#ifndef DATATOOLS_MULTI_PROPERTIES_IPP_
#define DATATOOLS_MULTI_PROPERTIES_IPP_
// Ourselves
#include <datatools/multi_properties.h>

// Standard Library

// Third Party
// - Boost
#include <boost/archive/archive_exception.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>

// This Project
#include <datatools/i_serializable.ipp>
#include <datatools/utils.h>
#include <datatools/properties.ipp>

namespace datatools {

/// Boost serialization template method
DATATOOLS_SERIALIZATION_SERIALIZE_IMPLEMENT_HEADER(multi_properties::entry, archive, /*version*/)
{
  archive & boost::serialization::make_nvp("key", key_);
  archive & boost::serialization::make_nvp("meta", meta_);
  archive & boost::serialization::make_nvp("properties", properties_);
}


/// Boost serialization template method
DATATOOLS_SERIALIZATION_SERIALIZE_IMPLEMENT_HEADER(multi_properties, archive, version)
{
  if (version == 1) {
    // from version 1 we inherit explicitely from the
    // 'datatools::i_serializable' abstract class
    archive & DATATOOLS_SERIALIZATION_OLD_I_SERIALIZABLE_BASE_OBJECT_NVP;
  }
  else if (version >= 2) {
    archive & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
  }
  archive & boost::serialization::make_nvp("description", description_);
  archive & boost::serialization::make_nvp("key_label", key_label_);
  archive & boost::serialization::make_nvp("meta_label", meta_label_);
  archive & boost::serialization::make_nvp("entries", entries_);
  archive & boost::serialization::make_nvp("ordered_entries", ordered_entries_);
}

} // end of namespace datatools

#include <boost/serialization/version.hpp>
BOOST_CLASS_VERSION(datatools::multi_properties, 2)

#endif // DATATOOLS_MULTI_PROPERTIES_IPP_

