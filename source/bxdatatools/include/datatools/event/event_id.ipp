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
    void event_id::serialize (Archive            & a_archive , 
			      const unsigned int   a_version)
    {
      //a_archive & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
      a_archive & boost::serialization::make_nvp ("run_number",   run_number_);
      a_archive & boost::serialization::make_nvp ("event_number", event_number_);
      return;
    }

  } // end of namespace event 

} // end of namespace datatools 

#endif // __datatools__event__event_id_ipp

