// -*- mode: c++ ; -*-
/* mygsl/interval.ipp */

#ifndef MYGSL_INTERVAL_IPP_
#define MYGSL_INTERVAL_IPP_ 1

#include <mygsl/interval.h>

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
#include <datatools/i_serializable.ipp>

namespace mygsl {

  template<class Archive>
  void interval::serialize (Archive & ar,
                            const unsigned int /*version*/)
  {
    ar & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
    ar & boost::serialization::make_nvp ("min_included", _min_included_);
    ar & boost::serialization::make_nvp ("max_included", _max_included_);
    ar & boost::serialization::make_nvp ("min",_min_);
    ar & boost::serialization::make_nvp ("max", _max_);
    ar & boost::serialization::make_nvp ("eps", _eps_);
    return;
  }

} // end of namespace mygsl

#endif // MYGSL_INTERVAL_IPP_

// end of mygsl/interval.ipp
