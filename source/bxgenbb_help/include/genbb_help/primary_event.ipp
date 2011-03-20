// -*- mode: c++; -*- 
/* genbb_help::primary_event.ipp */

#ifndef __genbb_help__primary_event_ipp
#define __genbb_help__primary_event_ipp 1

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>

#include <boost/serialization/list.hpp> 
#include <boost/serialization/string.hpp>

#include <datatools/serialization/utils.h>

#include <genbb_help/primary_event.h>
#include <genbb_help/primary_particle.ipp>

namespace genbb {

  using namespace std;

    template<class Archive>
    void primary_event::serialize (Archive            & ar, 
                   		   const unsigned int   version)
    {
      //ar & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;    
      ar & boost::serialization::make_nvp ("time", time);
      ar & boost::serialization::make_nvp ("particles", particles);
      ar & boost::serialization::make_nvp ("classification", classification);
      return;
    }

} // end of namespace genbb

#endif // __genbb_help__primary_event_ipp

// end of genbb_help::primary_event.ipp
