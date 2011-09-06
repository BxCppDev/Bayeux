// -*- mode: c++; -*- 
/* genbb_help::primary_particle.ipp */

#ifndef __genbb_help__primary_particle_ipp
#define __genbb_help__primary_particle_ipp 1

#include <genbb_help/primary_particle.h>

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/string.hpp>

#include <datatools/serialization/utils.h>
#include <datatools/serialization/i_serializable.ipp>
#include <geomtools/utils.ipp>

namespace genbb {

  using namespace std;

  template<class Archive>
  void primary_particle::serialize (Archive            & a_ar, 
				    const unsigned int   a_version)
  {
    if (a_version > 0)
      {
	a_ar & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;    
      }
    a_ar & boost::serialization::make_nvp ("type", type);
    if (type == UNDEF)
      {
	a_ar & boost::serialization::make_nvp ("particle_label", particle_label);
      }
    else
      {
	particle_label = get_particle_label_from_type (type);
      }
    a_ar & boost::serialization::make_nvp ("time", time);
    a_ar & boost::serialization::make_nvp ("momentum", momentum);
    return;
  }

} // end of namespace genbb

#include <boost/serialization/version.hpp>
BOOST_CLASS_VERSION(genbb::primary_particle, 1)

#endif // __genbb_help__primary_particle_ipp

// end of genbb_help::primary_particle.ipp
