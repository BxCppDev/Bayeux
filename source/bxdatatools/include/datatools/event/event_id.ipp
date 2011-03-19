// -*- mode: c++; -*- 
//! \file datatools/event/event_id.ipp

#ifndef __datatools__event__event_id_ipp
#define __datatools__event__event_id_ipp 1

#include <boost/serialization/nvp.hpp>

#include <datatools/serialization/utils.h>
#include <datatools/event/event_id.h>
	 
namespace datatools {
		 
  namespace event {
    
    template<class Archive>
    void event_id::serialize (Archive            & ar_ , 
			      const unsigned int   version_)
    {
      ar_ & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
      ar_ & boost::serialization::make_nvp ("run_number",   __run_number);
      ar_ & boost::serialization::make_nvp ("event_number", __event_number);
      return;
    }

  } // end of namespace event 

} // end of namespace datatools 

#endif // __datatools__event__event_id_ipp

