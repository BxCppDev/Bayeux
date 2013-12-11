// -*- mode: c++ ; -*-
/* mygsl/datapoint.ipp */

#ifndef MYGSL_DATAPOINT_IPP_
#define MYGSL_DATAPOINT_IPP_ 1

#include <mygsl/datapoint.h>

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
#include <datatools/i_serializable.ipp>
namespace mygsl {

  template<class Archive>
  void datapoint::serialize (Archive & ar,
                             const unsigned int /*version*/)
  {
    ar & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
    ar & boost::serialization::make_nvp ("x", _x_);
    ar & boost::serialization::make_nvp ("y", _y_);
    ar & boost::serialization::make_nvp ("sigma_y", _sigma_y_);
    return;
  }

} // end of namespace mygsl

#endif // MYGSL_DATAPOINT_IPP_

// end of mygsl/datapoint.ipp
