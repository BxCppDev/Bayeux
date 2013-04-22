// -*- mode: c++; -*-
#ifndef SDATA_IPP_
#define SDATA_IPP_

#include "sdata.h"

#include <boost/serialization/nvp.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>

template<class Archive>
void sdata::serialize (Archive & ar_,
                       const unsigned int version_)
{
  // Special serialization code for inherited abstract class :
  ar_ & boost::serialization::make_nvp(
    "datatools__serialization__i_serializable",
    boost::serialization::base_object<datatools::i_serializable >
      (*this));
  // Serialization code for class attributes :
  ar_ & boost::serialization::make_nvp ("bval", _bval_);
  ar_ & boost::serialization::make_nvp ("cval", _cval_);
  ar_ & boost::serialization::make_nvp ("ival", _ival_);
  ar_ & boost::serialization::make_nvp ("fval", _fval_);
  ar_ & boost::serialization::make_nvp ("dval", _dval_);
  ar_ & boost::serialization::make_nvp ("sval", _sval_);
  ar_ & boost::serialization::make_nvp ("dvec", _dvec_);
  return;
}
#endif // SDATA_IPP_
