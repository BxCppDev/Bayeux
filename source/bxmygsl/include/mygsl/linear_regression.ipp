// \file mygsl/linear_regression.ipp

#ifndef MYGSL_LINEAR_REGRESSION_IPP
#define MYGSL_LINEAR_REGRESSION_IPP 1

// Ourselves:
#include <mygsl/linear_regression.h>

// Third party:
// - Boost:
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
// - Bayeux/datatools:
#include <datatools/i_serializable.ipp>

namespace mygsl {

  template<class Archive>
  void linear_regression::fit_data::serialize (Archive & ar,
                                               const unsigned int /*version*/)
  {
    ar & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
    ar & boost::serialization::make_nvp ("n", n);
    ar & boost::serialization::make_nvp ("status", status);
    ar & boost::serialization::make_nvp ("weighted", weighted);
    ar & boost::serialization::make_nvp ("with_constant", with_constant);
    if (is_valid()) {
      ar & boost::serialization::make_nvp ("c1", c1);
      ar & boost::serialization::make_nvp ("c0", c0);
      ar & boost::serialization::make_nvp ("cov00", cov00);
      ar & boost::serialization::make_nvp ("cov01", cov01);
      ar & boost::serialization::make_nvp ("cov11", cov11);
      ar & boost::serialization::make_nvp ("sumsq", sumsq);
      ar & boost::serialization::make_nvp ("chisq", chisq);
    }
    return;
  }

} // namespace mygsl

#endif // MYGSL_LINEAR_REGRESSION_IPP

/* Local Variables: */
/* mode: c++        */
/* coding: utf-8    */
/* End:             */
