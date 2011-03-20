// -*- mode: c++; -*- 
/* genbb_help::primary_particle.ipp */

#ifndef __genbb_help__primary_particle_ipp
#define __genbb_help__primary_particle_ipp 1

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>

#include <boost/serialization/string.hpp>

#include <datatools/serialization/utils.h>

#include <genbb_help/primary_particle.h>

namespace genbb {

  using namespace std;

  template<class Archive>
  void primary_particle::serialize (Archive            & ar, 
				    const unsigned int   version)
  {
    //ar & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
    ar & boost::serialization::make_nvp ("type", type);
    if (type  == UNDEF)
      {
	ar & boost::serialization::make_nvp ("particle_label", particle_label);
      }
    else
      {
	particle_label = get_particle_label_from_type (type);
      }
    ar & boost::serialization::make_nvp ("time", time);
    ar & boost::serialization::make_nvp ("momentum", momentum);
    return;
  }

} // end of namespace genbb

#endif // __genbb_help__primary_particle_ipp

// end of genbb_help::primary_particle.ipp
