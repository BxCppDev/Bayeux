/// \file mctools/simulated_data.ipp

#ifndef MCTOOLS_SIMULATED_DATA_IPP
#define MCTOOLS_SIMULATED_DATA_IPP 1

// Ourselves:
#include <mctools/simulated_data.h>

// Third party:
// - Boost:
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
// - Bayeux/datatools :
#include <datatools/i_serializable.ipp>
#include <datatools/properties.ipp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>
// - Bayeux/geomtools :
#include <geomtools/utils.ipp>
// - Bayeux/genbb_help :
#include <genbb_help/primary_event.ipp>

// This project:
#include <mctools/base_step_hit.ipp>

namespace mctools {

  template<class Archive>
  void simulated_data::serialize(Archive & ar_, const unsigned int version_)
  {
    if (version_ > 0) {
      ar_ & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
    }
    ar_ & boost::serialization::make_nvp("vertex", _vertex_);
    //>>> Handle the new _time_ attribute for version >= 3:
    if (Archive::is_loading::value) {
      // Default time is invalid:
      datatools::invalidate(_time_);
    }
    if (Archive::is_saving::value || version_ >= 3) {
      ar_ & boost::serialization::make_nvp("time", _time_);
    }
    //<<<
    ar_ & boost::serialization::make_nvp("primary_event",   _primary_event_);
    ar_ & boost::serialization::make_nvp("properties",      _properties_);
    ar_ & boost::serialization::make_nvp("collection_type", _collection_type_);
    if (version_ < 2) {
      ar_ & boost::serialization::make_nvp("step_hits_dict", _step_hits_dict_);
    } else {
      if (_collection_type_ == HANDLE_HIT_COLLECTION_TYPE) {
        ar_ & boost::serialization::make_nvp("step_hits_dict", _step_hits_dict_);
      }
      if (_collection_type_ == PLAIN_HIT_COLLECTION_TYPE) {
        ar_ & boost::serialization::make_nvp("plain_step_hits_dict", _plain_step_hits_dict_);
      }
    }
    return;
  }

} // end of namespace mctools

#endif // MCTOOLS_SIMULATED_DATA_IPP

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
