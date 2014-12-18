/// \file genbb_help/primary_particle.ipp

#ifndef GENBB_HELP_PRIMARY_PARTICLE_IPP
#define GENBB_HELP_PRIMARY_PARTICLE_IPP 1

// Ourselves:
#include <genbb_help/primary_particle.h>

// Third party:
// - Boost:
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/string.hpp>
// - Bayeux/datatools:
#include <datatools/i_serializable.ipp>
#include <datatools/properties.ipp>
// - Bayeux/geomtools:
#include <geomtools/utils.ipp>

namespace genbb {

  /// Boost serialization template method
  template<class Archive>
  void primary_particle::serialize(Archive            & ar_,
                                    const unsigned int   version_)
  {
    if (Archive::is_saving::value) {
      // DT_LOG_DEBUG(datatools::logger::PRIO_DEBUG, "saving with version=" << version_);
    } else {
      // DT_LOG_DEBUG(datatools::logger::PRIO_DEBUG, "loading with version=" << version_);
    }

    if (version_ >= 1) {
      ar_ & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
    }

    // Generation Id:
    if (version_ >= 4) {
      ar_ & boost::serialization::make_nvp("generation_id", _generation_id_);
    } else {
      if (Archive::is_loading::value) {
        _generation_id_ = primary_particle::INVALID_GENERATION_ID;
      }
    }

    // Type:
    ar_ & boost::serialization::make_nvp("type", _type_);

    // PDG particle code:
    if (version_ >= 3) {
      // PDG code attribute was introduced with version 3
      ar_ & boost::serialization::make_nvp("pdg_code", _pdg_code_);
    } else {
      // if load from version < 3, use an invalidated PDG code.
      if (Archive::is_loading::value) {
        _pdg_code_ = PDG_CODE_UNDEFINED;
      }
    }

    // Particle label:
    if (version_ < 3) {
      if (_type_ == PARTICLE_UNDEFINED ) {
        ar_ & boost::serialization::make_nvp("particle_label", _particle_label_);
      } else {
        _particle_label_ = particle_label_from_type(_type_);
      }
    } else {
      ar_ & boost::serialization::make_nvp("particle_label", _particle_label_);
    }

    // Time:
    ar_ & boost::serialization::make_nvp("time", _time_);

    // Momentum:
    ar_ & boost::serialization::make_nvp("momentum", _momentum_);

    // Vertex:
    if (version_ >= 2) {
      // vertex attribute was introduced with version 2
      ar_ & boost::serialization::make_nvp("vertex", _vertex_);
    } else {
      // if load from version < 2, use an invalidated vertex attribute.
      if (Archive::is_loading::value) {
        geomtools::invalidate(_vertex_);
      }
    }

    // Auxiliary properties:
    if (version_ >= 3) {
      // auxiliary properties attribute was introduced with version 2
      ar_ & boost::serialization::make_nvp("auxiliaries", _auxiliaries_);
    } else {
      // if load from version < 3, use an invalidated vertex attribute.
      if (Archive::is_loading::value) {
        _auxiliaries_.clear();
      }
    }
    return;
  }

} // end of namespace genbb

#include <boost/serialization/version.hpp>
BOOST_CLASS_VERSION(genbb::primary_particle, 4)

#endif // GENBB_HELP_PRIMARY_PARTICLE_IPP

// Local Variables: --
// mode: c++ --
// End: --
