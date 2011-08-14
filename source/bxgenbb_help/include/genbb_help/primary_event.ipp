// -*- mode: c++; -*- 
/* genbb_help::primary_event.ipp */

#ifndef __genbb_help__primary_event_ipp
#define __genbb_help__primary_event_ipp 1

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/version.hpp>
//#include <boost/serialization/export.hpp>

#include <boost/serialization/list.hpp> 
#include <boost/serialization/string.hpp>

#include <datatools/serialization/utils.h>

#include <genbb_help/primary_event.h>
#include <datatools/serialization/i_serializable.ipp>
#include <genbb_help/primary_particle.ipp>

namespace genbb {

  using namespace std;

    template<class Archive>
    void primary_event::serialize (Archive            & a_ar, 
                   		   const unsigned int   a_version)
    {
      if (a_version > 0)
	{
	  a_ar & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;    
	}
      a_ar & boost::serialization::make_nvp ("time", time);
      a_ar & boost::serialization::make_nvp ("particles", particles);
      a_ar & boost::serialization::make_nvp ("classification", classification);
      return;
    }

} // end of namespace genbb

BOOST_CLASS_VERSION(genbb::primary_event, 1)
//BOOST_CLASS_EXPORT_KEY2(genbb::primary_event, "genbb::primary_event")

#endif // __genbb_help__primary_event_ipp

// end of genbb_help::primary_event.ipp
