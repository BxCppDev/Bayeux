/// \file genbb_help/primary_event.ipp

#ifndef GENBB_HELP_PRIMARY_EVENT_IPP
#define GENBB_HELP_PRIMARY_EVENT_IPP 1

// Ourselves:
#include <genbb_help/primary_event.h>

// Third party:
// - Boost:
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/string.hpp>
// - Bayeux/datatools:
#include <datatools/i_serializable.ipp>

// This project:
#include <genbb_help/primary_particle.ipp>

namespace genbb {

  /// Boost serialization template method
  template<class Archive>
  void primary_event::serialize(Archive & ar_,
                                const unsigned int version_)
  {
    if (Archive::is_saving::value) {
      // DT_LOG_DEBUG(datatools::logger::PRIO_DEBUG, "saving with version=" << version_);
    } else {
      // DT_LOG_DEBUG(datatools::logger::PRIO_DEBUG, "loading with version=" << version_);
    }
    if (version_ > 0) {
      ar_ & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
    }

    // 2012-06-26 FM : support GENBB 'labelled' MC event :
    if (version_ < 3) {
      if (Archive::is_saving::value) {
        ar_ & boost::serialization::make_nvp("label", _label_);
      } else {
        _label_.clear();
      }
    } else {
      ar_ & boost::serialization::make_nvp("label", _label_);
    }

    // Time:
    ar_ & boost::serialization::make_nvp("time", _time_);

    // List of primary particles:
    ar_ & boost::serialization::make_nvp("particles", _particles_);

    // Classification:
    ar_ & boost::serialization::make_nvp("classification", _classification_);

    // 2012-06-21 FM : support GENBB 'weighted' MC event :
    if (version_ < 2) {
      if (Archive::is_saving::value) {
        ar_ & boost::serialization::make_nvp("genbb_weight", _genbb_weight_);
      } else {
        _genbb_weight_ = 1.0;
      }
    } else {
      ar_ & boost::serialization::make_nvp("genbb_weight", _genbb_weight_);
    }

    // 2013-03-6 FM : support 'auxiliaries' :
    if (version_ < 4) {
      if (Archive::is_saving::value){
        ar_ & boost::serialization::make_nvp("auxiliaries", _auxiliaries_);
      } else {
        _auxiliaries_.clear();
      }
    } else {
      ar_ & boost::serialization::make_nvp("auxiliaries", _auxiliaries_);
    }

    return;
  }

} // end of namespace genbb

#endif // GENBB_HELP_PRIMARY_EVENT_IPP

// Local Variables: --
// mode: c++ --
// End: --
