// -*- mode: c++ ; -*-
/* mygsl/polynomial.ipp */

#ifndef MYGSL_POLYNOMIAL_IPP_
#define MYGSL_POLYNOMIAL_IPP_ 1

#include <mygsl/polynomial.h>

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/vector.hpp>
#include <datatools/i_serializable.ipp>

namespace mygsl {

  template<class Archive>
  void polynomial::serialize (Archive & ar,
                              const unsigned int /*version*/)
  {
    ar & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
    ar & boost::serialization::make_nvp ("c", _c_);
    return;
  }

} // end of namespace mygsl

#endif // MYGSL_POLYNOMIAL_IPP_

// end of mygsl/polynomial.ipp
