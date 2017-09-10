/// \file mctools/base_step_hit.ipp

#ifndef MCTOOLS_BASE_STEP_HIT_IPP
#define MCTOOLS_BASE_STEP_HIT_IPP 1

// Ourselves:
#include <mctools/base_step_hit.h>

// Third party:
// - Boost:
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/string.hpp>

// - Bayeux/geomtools
#include <geomtools/base_hit.ipp>
#include <geomtools/utils.ipp>

namespace mctools {

  template<class Archive>
  void base_step_hit::serialize (Archive            & ar,
                                 const unsigned int  version)
  {
    // inherit from the 'base_hit' mother class:
    ar & boost::serialization::make_nvp("geomtools__base_hit",
                                        boost::serialization::base_object<geomtools::base_hit>(*this));

    // the '_store' field member from the 'base_hit' mother class
    // knows about the field to be stored/loaded
    // from the archive.
    if (_store & STORE_POSITION_START)
      {
        ar & boost::serialization::make_nvp ("position_start", _position_start_);
      }
    if (_store & STORE_POSITION_STOP)
      {
        ar & boost::serialization::make_nvp ("position_stop",  _position_stop_);
      }
    if (_store & STORE_TIME_START)
      {
        ar & boost::serialization::make_nvp ("time_start",     _time_start_);
      }
    if (_store & STORE_TIME_STOP)
      {
        ar & boost::serialization::make_nvp ("time_stop",      _time_stop_);
      }
    if (_store & STORE_MOMENTUM_START)
      {
        ar & boost::serialization::make_nvp ("momentum_start", _momentum_start_);
      }
    if (_store & STORE_MOMENTUM_STOP)
      {
        ar & boost::serialization::make_nvp ("momentum_stop",  _momentum_stop_);
      }
    if (_store & STORE_ENERGY_DEPOSIT)
      {
        ar & boost::serialization::make_nvp ("energy_deposit", _energy_deposit_);
      }
    if (_store & STORE_PARTICLE_NAME)
      {
        ar & boost::serialization::make_nvp ("particle_name",  _particle_name_);
      }

    if (version>=1) {
      if (_store & STORE_BIASING_WEIGHT) {
        ar & boost::serialization::make_nvp ("biasing_weight",  _biasing_weight_);
      }
    }
    return;
  }

} // end of namespace mctools

#endif // MCTOOLS_BASE_STEP_HIT_IPP

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
