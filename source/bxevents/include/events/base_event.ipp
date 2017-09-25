/// \file events/base_event.ipp

#ifndef EVENTS_BASE_EVENT_IPP
#define EVENTS_BASE_EVENT_IPP 1

// Ourselves:
#include <events/base_event.h>

// Third party:
// - Boost:
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/string.hpp>

// - Bayeux/geomtools
#include <geomtools/base_hit.ipp>
#include <geomtools/utils.ipp>

namespace events {

  template<class Archive>
  void base_event::serialize(Archive & ar_,
                             const unsigned int /* version_ */)
  {
    // Inherit from the 'base_hit' mother class:
    ar_ & boost::serialization::make_nvp("geomtools__base_hit",
                                         boost::serialization::base_object<geomtools::base_hit>(*this));
    // The '_store' field member from the 'base_hit' mother class
    // knows about the field to be stored/loaded from the archive.
    if (_store & STORE_CATEGORY) {
      ar_ & boost::serialization::make_nvp("category", _category_);
    }
    if (_store & STORE_POSITION) {
      ar_ & boost::serialization::make_nvp("position", _position_);
    }
    if (_store & STORE_TIME) {
      ar_ & boost::serialization::make_nvp("time", _time_);
    }
    return;
  }

} // end of namespace events

#endif // EVENTS_BASE_EVENT_IPP

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
