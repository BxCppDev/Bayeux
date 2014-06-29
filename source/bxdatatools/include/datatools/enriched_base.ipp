// -*- mode: c++; -*-
//! \file datatools/enriched_base.ipp
#ifndef DATATOOLS_ENRICHED_BASE_IPP
#define DATATOOLS_ENRICHED_BASE_IPP

// Ourselves:
#include <datatools/enriched_base.h>

// Third Party:
// - Boost:
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/shared_ptr.hpp>

// This project:
#include <datatools/utils.h>
#include <datatools/properties.ipp>

namespace datatools {
  template<class Archive>
  void enriched_base::serialize (Archive & archive,
                                 const unsigned int /*version*/)
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

// #include <boost/serialization/version.hpp>
// BOOST_CLASS_VERSION(datatools::enriched_base, 0)

#endif // DATATOOLS_ENRICHED_BASE_IPP
