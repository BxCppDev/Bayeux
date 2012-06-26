// -*- mode: c++; -*- 
//! \file datatools/utils/properties.ipp
#ifndef DATATOOLS_UTILS_PROPERTIES_IPP_
#define DATATOOLS_UTILS_PROPERTIES_IPP_
// Ourselves
#include <datatools/utils/properties.h>

// Standard Library

// Third Party
// - Boost
#include <boost/archive/archive_exception.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
 
// This Project
#include <datatools/serialization/i_serializable.ipp>
#include <datatools/serialization/utils.h>

namespace datatools {
namespace utils {

template<class Archive>
void properties::data::serialize(Archive& ar, const unsigned int version) {
  ar & boost::serialization::make_nvp("description", _description_);
  ar & boost::serialization::make_nvp("flags",       _flags_);

  if (this->is_boolean()) {
    ar & boost::serialization::make_nvp("boolean_values", _boolean_values_);
  }
  if (this->is_integer()) {
    ar & boost::serialization::make_nvp("integer_values", _integer_values_);
  }
  if (this->is_real()) {
    ar & boost::serialization::make_nvp("real_values", _real_values_);
  }
  if (this->is_string()) {
    ar & boost::serialization::make_nvp("string_values", _string_values_);
  }
}


template<class Archive>
void properties::serialize(Archive& ar, const unsigned int version) {
  if (version > 0) {
    /* from version 1 we inherit explicitely from the
     * 'datatools::serialization::i_serializable' abstract class
     */
    ar & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
  }
  ar & boost::serialization::make_nvp("description", _description_);
  ar & boost::serialization::make_nvp("properties",  _props_);
}

} // end of namespace utils 
} // end of namespace datatools 

#include <boost/serialization/version.hpp>
BOOST_CLASS_VERSION(datatools::utils::properties, 1)

  // Debug :
  // BOOST_CLASS_VERSION(datatools::utils::properties::data, 5)
  // BOOST_CLASS_VERSION(datatools::utils::properties, 7)

#endif // DATATOOLS_UTILS_PROPERTIES_IPP_

