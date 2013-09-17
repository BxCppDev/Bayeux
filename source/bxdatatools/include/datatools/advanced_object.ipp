// -*- mode: c++; -*-
//! \file advanced_object.ipp
#ifndef DATATOOLS_ADVANCED_OBJECT_IPP_
#define DATATOOLS_ADVANCED_OBJECT_IPP_

// Ourselves
#include <datatools/advanced_object.h>

// Standard Library

// Third Party
// - Boost
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/shared_ptr.hpp>

// Datatools
#include <datatools/utils.h>
#include <datatools/properties.ipp>

namespace datatools {
  /// Boost serialization template method
DATATOOLS_SERIALIZATION_SERIALIZE_IMPLEMENT_HEADER(advanced_object, archive, version)
{
  archive & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
  archive & boost::serialization::make_nvp("name",              _name_);
  archive & boost::serialization::make_nvp("display_name",      _display_name_);
  archive & boost::serialization::make_nvp("terse_description", _terse_description_);
  archive & boost::serialization::make_nvp("logging_priority",  _logging_priority_);
  archive & boost::serialization::make_nvp("auxiliaries",       _auxiliaries_);
  return;
}
} // end of namespace datatools

//#include <boost/serialization/version.hpp>
//BOOST_CLASS_VERSION(datatools::advanced_object, 1)

#endif // DATATOOLS_ADVANCED_OBJECT_IPP_
