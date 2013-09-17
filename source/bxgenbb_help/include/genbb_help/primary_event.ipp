// -*- mode: c++; -*-
/* genbb_help::primary_event.ipp */

#ifndef GENBB_HELP_PRIMARY_EVENT_IPP_
#define GENBB_HELP_PRIMARY_EVENT_IPP_ 1

#include <genbb_help/primary_event.h>

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>

//#include <datatools/serialization/utils.h>
#include <datatools/i_serializable.ipp>
#include <genbb_help/primary_particle.ipp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/string.hpp>

namespace genbb {

  /// Boost serialization template method
  template<class Archive>
  void primary_event::serialize (Archive            & a_ar,
                                 const unsigned int   a_version)
  {
    if (a_version > 0)
      {
        a_ar & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
      }
    // 2012-06-26 FM : support GENBB 'labelled' MC event :
    if (a_version < 3)
      {
        if (Archive::is_saving::value)
          {
            a_ar & boost::serialization::make_nvp ("label", _label_);
          }
        else
          {
            _label_.clear ();
          }
      }
    else
      {
        a_ar & boost::serialization::make_nvp ("label", _label_);
      }
    a_ar & boost::serialization::make_nvp ("time", _time_);
    a_ar & boost::serialization::make_nvp ("particles", _particles_);
    a_ar & boost::serialization::make_nvp ("classification", _classification_);
    // 2012-06-21 FM : support GENBB 'weighted' MC event :
    if (a_version < 2)
      {
        if (Archive::is_saving::value)
          {
            a_ar & boost::serialization::make_nvp ("genbb_weight", _genbb_weight_);
          }
        else
          {
            _genbb_weight_ = 1.0;
          }
      }
    else
      {
        a_ar & boost::serialization::make_nvp ("genbb_weight", _genbb_weight_);
      }
    // 2013-03-6 FM : support 'auxiliaries' :
    if (a_version < 4)
      {
        if (Archive::is_saving::value)
          {
            a_ar & boost::serialization::make_nvp ("auxiliaries", _auxiliaries_);
          }
        else
          {
            _auxiliaries_.clear ();
          }
      }
    else
      {
        a_ar & boost::serialization::make_nvp ("auxiliaries", _auxiliaries_);
      }
    return;
  }

} // end of namespace genbb

#include <boost/serialization/version.hpp>
BOOST_CLASS_VERSION(genbb::primary_event, 4)

#endif // GENBB_HELP_PRIMARY_EVENT_IPP_

// end of genbb_help::primary_event.ipp
