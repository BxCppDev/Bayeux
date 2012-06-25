// -*- mode: c++; -*- 
//! \file datatools/utils/multi_properties.ipp
#ifndef DATATOOLS_UTILS_MULTI_PROPERTIES_IPP_
#define DATATOOLS_UTILS_MULTI_PROPERTIES_IPP_
// Ourselves
#include <datatools/utils/multi_properties.h>

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
#include <datatools/serialization/i_serializable.ipp>
#include <datatools/serialization/utils.h>
#include <datatools/utils/properties.ipp>
 
namespace datatools {
namespace utils {

template<class Archive>
void multi_properties::entry::serialize(Archive& a_ar, 
                                        const unsigned int a_version) {
  a_ar & boost::serialization::make_nvp("key", key_);
  a_ar & boost::serialization::make_nvp("meta", meta_);
  a_ar & boost::serialization::make_nvp("properties", properties_);
}


template<class Archive>
void multi_properties::serialize(Archive& a_ar, 
                                 const unsigned int a_version) {
  if (a_version >= 1) {
    // from version 1 we inherit explicitely from the
    // 'datatools::serialization::i_serializable' abstract class
    a_ar & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
  }
  a_ar & boost::serialization::make_nvp("description", description_);
  a_ar & boost::serialization::make_nvp("key_label", key_label_);
  a_ar & boost::serialization::make_nvp("meta_label", meta_label_);
  a_ar & boost::serialization::make_nvp("entries", entries_);
  a_ar & boost::serialization::make_nvp("ordered_entries", ordered_entries_);
}

} // end of namespace utils 
} // end of namespace datatools 

#include <boost/serialization/version.hpp>
BOOST_CLASS_VERSION(datatools::utils::multi_properties, 1)

#endif // DATATOOLS_UTILS_MULTI_PROPERTIES_IPP_

