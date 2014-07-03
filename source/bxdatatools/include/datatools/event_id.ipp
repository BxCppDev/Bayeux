// -*- mode: c++; -*-
//! \file datatools/event_id.ipp
#ifndef DATATOOLS_EVENT_ID_IPP
#define DATATOOLS_EVENT_ID_IPP

// Ourselves:
#include <datatools/event_id.h>

// Third Party:
// - Boost:
#include <boost/serialization/nvp.hpp>

// This project:
#include <datatools/utils.h>

namespace datatools {
/// Boost serialization template method
DATATOOLS_SERIALIZATION_SERIALIZE_IMPLEMENT_HEADER(event_id, archive, version)
{
  if (version > 0) {
    archive & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
  }
  archive & boost::serialization::make_nvp("run_number", run_number_);
  archive & boost::serialization::make_nvp("event_number", event_number_);
}
} // end of namespace datatools

#include <boost/serialization/version.hpp>
BOOST_CLASS_VERSION(datatools::event_id, 1)

#endif // DATATOOLS_EVENT_ID_IPP
