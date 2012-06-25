// -*- mode: c++; -*- 
//! \file datatools/utils/things.ipp
#ifndef DATATOOLS_UTILS_THINGS_IPP_
#define DATATOOLS_UTILS_THINGS_IPP_
// Ourselves
#include <datatools/utils/things.h>

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
void things::entry_t::serialize(Archive& ar, const unsigned int version) {
  ar & boost::serialization::make_nvp("description", description);
  ar & boost::serialization::make_nvp("flags",       flags);
  ar & boost::serialization::make_nvp("handle",      handle); 
}


template<class Archive>
void things::serialize(Archive& ar, const unsigned int file_version) {
  ar & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
  ar & boost::serialization::make_nvp("name",        name_);
  ar & boost::serialization::make_nvp("description", description_);
  ar & boost::serialization::make_nvp("things",      things_);
}

} // end of namespace utils 
} // end of namespace datatools 

#endif // DATATOOLS_UTILS_THINGS_IPP_

