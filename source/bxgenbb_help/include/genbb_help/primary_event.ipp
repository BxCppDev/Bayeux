// -*- mode: c++; -*- 
/* genbb_help::primary_event.ipp */

#ifndef __genbb_help__primary_event_ipp
#define __genbb_help__primary_event_ipp 1

#include <genbb_help/primary_event.h>

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>

#include <datatools/serialization/utils.h>
#include <datatools/serialization/i_serializable.ipp>
#include <genbb_help/primary_particle.ipp>
#include <boost/serialization/list.hpp> 
#include <boost/serialization/string.hpp>

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

#include <boost/serialization/version.hpp>
BOOST_CLASS_VERSION(genbb::primary_event, 1)

#endif // __genbb_help__primary_event_ipp

// end of genbb_help::primary_event.ipp
