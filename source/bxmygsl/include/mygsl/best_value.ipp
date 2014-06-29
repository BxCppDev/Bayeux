/// \file mygsl/best_value.ipp

#ifndef MYGSL_BEST_VALUE_IPP
#define MYGSL_BEST_VALUE_IPP 1

// Ourselves:
#include <mygsl/best_value.h>

// Third party:
// - Boost:
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
// - Bayeux/datatools:
#include <datatools/i_serializable.ipp>

namespace mygsl {

  template<class Archive>
  void best_value::serialize (Archive & ar,
                              const unsigned int /*version*/)
  {
    ar & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
    ar & boost::serialization::make_nvp ("value", _value_);
    ar & boost::serialization::make_nvp ("error_low", _error_low_);
    ar & boost::serialization::make_nvp ("error_high", _error_high_);
    ar & boost::serialization::make_nvp ("confidence_level", _confidence_level_);
    return;
  }

} // end of namespace mygsl

#endif // MYGSL_BEST_VALUE_IPP

/* Local Variables: */
/* mode: c++        */
/* coding: utf-8    */
/* End:             */
