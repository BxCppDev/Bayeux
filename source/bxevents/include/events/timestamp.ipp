/// \file events/timestamp.ipp

#ifndef EVENTS_TIMESTAMP_IPP
#define EVENTS_TIMESTAMP_IPP 1

// Ourselves:
#include <events/timestamp.h>

// Third party:
// - Boost:
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
// - Bayeux/datatools:
#include <datatools/i_serializable.ipp>

namespace events {

  template<class Archive>
  void timestamp::serialize(Archive & ar_,
                            const unsigned int /* version_ */)
  {
    ar_ & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
    ar_ & boost::serialization::make_nvp("seconds",      _seconds_);
    ar_ & boost::serialization::make_nvp("femtoseconds", _femtoseconds_);
    return;
  }

} // end of namespace events

#endif // EVENTS_TIMESTAMP_IPP

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
