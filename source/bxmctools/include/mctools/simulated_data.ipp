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
  void simulated_data::serialize (Archive & ar, const unsigned int version)
  {
    if (version > 0)
      {
        ar & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
      }
    ar & boost::serialization::make_nvp ("vertex",          _vertex_);
    ar & boost::serialization::make_nvp ("primary_event",   _primary_event_);
    ar & boost::serialization::make_nvp ("properties",      _properties_);
    ar & boost::serialization::make_nvp ("collection_type", _collection_type_);
    if (version < 2)
      {
        ar & boost::serialization::make_nvp ("step_hits_dict",  _step_hits_dict_);
      }
    else
      {
        if (_collection_type_ == HANDLE_HIT_COLLECTION_TYPE)
          {
            ar & boost::serialization::make_nvp ("step_hits_dict",  _step_hits_dict_);
          }
        if (_collection_type_ == PLAIN_HIT_COLLECTION_TYPE)
          {
            ar & boost::serialization::make_nvp ("plain_step_hits_dict",  _plain_step_hits_dict_);
          }
      }
    return;
  }

} // end of namespace mctools

#endif // MCTOOLS_SIMULATED_DATA_IPP

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
