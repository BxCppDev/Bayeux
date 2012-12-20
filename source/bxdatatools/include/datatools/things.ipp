// -*- mode: c++; -*- 
//! \file datatools/things.ipp
#ifndef DATATOOLS_THINGS_IPP_
#define DATATOOLS_THINGS_IPP_
// Ourselves
#include <datatools/things.h>

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
#include <datatools/i_serializable.ipp>
#include <datatools/utils.h>

namespace datatools {


DATATOOLS_SERIALIZATION_SERIALIZE_IMPLEMENT_HEADER(things::entry_type, archive, version)
{
  archive & boost::serialization::make_nvp("description", description);
  archive & boost::serialization::make_nvp("flags",       flags);
  archive & boost::serialization::make_nvp("handle",      handle); 
}

DATATOOLS_SERIALIZATION_SERIALIZE_IMPLEMENT_HEADER(things, archive, version)
{
  if (version == 0) {
    archive & DATATOOLS_SERIALIZATION_OLD_I_SERIALIZABLE_BASE_OBJECT_NVP;
  }
  else {
    archive & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
  }
  archive & boost::serialization::make_nvp("name",        name_);
  archive & boost::serialization::make_nvp("description", description_);
  archive & boost::serialization::make_nvp("things",      things_);
}

} // end of namespace datatools 

#include <boost/serialization/version.hpp>
BOOST_CLASS_VERSION(datatools::things, 1)

#endif // DATATOOLS_THINGS_IPP_

