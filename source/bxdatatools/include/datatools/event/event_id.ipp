// -*- mode: c++; -*- 
//! \file datatools/event/event_id.ipp

#ifndef __datatools__event__event_id_ipp
#define __datatools__event__event_id_ipp 1

#include <datatools/event/event_id.h>
#include <boost/serialization/nvp.hpp>

#include <datatools/serialization/utils.h>
	 
namespace datatools {
		 
  namespace event {
    
    template<class Archive>
    void event_id::serialize (Archive            & a_archive , 
			      const unsigned int   a_version)
    {
      a_archive & boost::serialization::make_nvp ("run_number",   _run_number_);
      a_archive & boost::serialization::make_nvp ("event_number", _event_number_);
      return;
    }

  } // end of namespace event 

} // end of namespace datatools 

#endif // __datatools__event__event_id_ipp

