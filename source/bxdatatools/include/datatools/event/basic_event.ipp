// -*- mode: c++; -*- 
//! \file datatools/event/basic_event.ipp

#ifndef __datatools__event__basic_event_ipp
#define __datatools__event__basic_event_ipp 1

#include <datatools/event/basic_event.h>

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>

#include <datatools/serialization/utils.h>
#include <datatools/serialization/i_serializable.ipp>
#include <datatools/event/event_id.ipp>
#include <datatools/utils/properties.ipp>
	 
namespace datatools {
		 
  namespace event {

    template<class Archive>
    void basic_event::serialize (Archive            & ar_, 
				 const unsigned int   version_)
    {
      ar_ & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
      ar_ & boost::serialization::make_nvp ("id",         id_);
      ar_ & boost::serialization::make_nvp ("properties", properties_);
      return;
    }

  } // end of namespace event 

} // end of namespace datatools 

#endif // __datatools__event__basic_event_ipp

