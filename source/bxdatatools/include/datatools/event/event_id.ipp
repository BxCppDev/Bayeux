// -*- mode: c++; -*- 
//! \file datatools/event/event_id.ipp
#ifndef DATATOOLS_EVENT_EVENT_ID_IPP_
#define DATATOOLS_EVENT_EVENT_ID_IPP_

#include <datatools/event/event_id.h>
#include <boost/serialization/nvp.hpp>

#include <datatools/serialization/utils.h>
	 
namespace datatools {
namespace event {
template<class Archive>
void event_id::serialize(Archive& archive, const unsigned int version) {
  archive & boost::serialization::make_nvp("run_number", run_number_);
  archive & boost::serialization::make_nvp("event_number", event_number_);
}
} // end of namespace event 
} // end of namespace datatools 

#endif // DATATOOLS_EVENT_EVENT_ID_IPP_
