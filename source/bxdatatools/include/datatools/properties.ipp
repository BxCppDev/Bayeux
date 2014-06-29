// -*- mode: c++; -*-
//! \file datatools/properties.ipp

#ifndef DATATOOLS_PROPERTIES_IPP
#define DATATOOLS_PROPERTIES_IPP

// Ourselves:
#include <datatools/properties.h>

// Third Party:
// - Boost:
#include <boost/archive/archive_exception.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>

// This project:
#include <datatools/i_serializable.ipp>
#include <datatools/utils.h>

namespace datatools {

/// Boost serialization template method
DATATOOLS_SERIALIZATION_SERIALIZE_IMPLEMENT_HEADER(properties::data, archive, /*version*/)
{
  archive & boost::serialization::make_nvp("description", _description_);
  archive & boost::serialization::make_nvp("flags",       _flags_);

  if (this->is_boolean()) {
    archive & boost::serialization::make_nvp("boolean_values", _boolean_values_);
  }
  if (this->is_integer()) {
    archive & boost::serialization::make_nvp("integer_values", _integer_values_);
  }
  if (this->is_real()) {
    archive & boost::serialization::make_nvp("real_values", _real_values_);
  }
  if (this->is_string()) {
    archive & boost::serialization::make_nvp("string_values", _string_values_);
  }
}


/// Boost serialization template method
DATATOOLS_SERIALIZATION_SERIALIZE_IMPLEMENT_HEADER(properties, archive, version)
{
  if (version ==1 ) {
    /* from version 1 we inherit explicitely from the
     * 'datatools::serialization::i_serializable' abstract class
     */
    archive & DATATOOLS_SERIALIZATION_OLD_I_SERIALIZABLE_BASE_OBJECT_NVP;
  }
  else if (version >= 2) {
    archive & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
  }
  archive & boost::serialization::make_nvp("description", _description_);
  archive & boost::serialization::make_nvp("properties",  _props_);
}

} // end of namespace datatools

#include <boost/serialization/version.hpp>
BOOST_CLASS_VERSION(datatools::properties, 2)

#endif // DATATOOLS_PROPERTIES_IPP
