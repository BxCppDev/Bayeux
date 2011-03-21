// -*- mode: c++; -*- 
//! \file datatools/event/basic_event.ipp

#ifndef __datatools__event__basic_event_ipp
#define __datatools__event__basic_event_ipp 1

#include <boost/archive/archive_exception.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/version.hpp>

#include <boost/serialization/map.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/vector.hpp> // missing header: fixed 2010-03-16
#include <boost/serialization/string.hpp>

#include <datatools/serialization/utils.h>
#include <datatools/event/basic_event.h>
	 
namespace datatools {
		 
  namespace event {

    template<class Archive>
    void basic_event::serialize (Archive            & ar_, 
				 const unsigned int   version_)
    {
      ar_ & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
      ar_ & boost::serialization::make_nvp ("id", __id);
      ar_ & boost::serialization::make_nvp ("properties",__properties);
      return;
    }

  } // end of namespace event 

} // end of namespace datatools 

#endif // __datatools__event__basic_event_ipp

