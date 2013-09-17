// -*- mode: c++; -*-
/* genbb_help::primary_particle.ipp */

#ifndef GENBB_HELP_PRIMARY_PARTICLE_IPP_
#define GENBB_HELP_PRIMARY_PARTICLE_IPP_ 1

#include <genbb_help/primary_particle.h>

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/string.hpp>

//#include <datatools/serialization/utils.h>
#include <datatools/i_serializable.ipp>
#include <geomtools/utils.ipp>

namespace genbb {

  /// Boost serialization template method
  template<class Archive>
  void primary_particle::serialize (Archive            & a_ar,
                                    const unsigned int   a_version)
  {
    if (a_version > 0)
      {
        a_ar & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
      }
    a_ar & boost::serialization::make_nvp ("type", _type_);
    if (_type_ == UNDEF)
      {
        a_ar & boost::serialization::make_nvp ("particle_label", _particle_label_);
      }
    else
      {
        _particle_label_ = get_particle_label_from_type (_type_);
      }
    a_ar & boost::serialization::make_nvp ("time", _time_);
    a_ar & boost::serialization::make_nvp ("momentum", _momentum_);
    if (a_version > 1)
      {
        // vertex attribute was introduced with version 2
        a_ar & boost::serialization::make_nvp ("vertex", _vertex_);
      }
    else
      {
        // if load from version < 2, use an invalidated vertex attribute.
        if (Archive::is_loading::value)
          {
            geomtools::invalidate (_vertex_);
          }
      }
   return;
  }

} // end of namespace genbb

#include <boost/serialization/version.hpp>
BOOST_CLASS_VERSION(genbb::primary_particle, 2)

#endif // GENBB_HELP_PRIMARY_PARTICLE_IPP_

// end of genbb_help::primary_particle.ipp
