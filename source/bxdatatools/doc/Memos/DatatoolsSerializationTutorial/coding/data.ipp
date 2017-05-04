// -*- mode: c++; -*-
#ifndef DATA_IPP_
#define DATA_IPP_ 1

#include "data.h"

// Support for XML 'key-value' based archives (highly recommended):
#include <boost/serialization/nvp.hpp>

// Support Boost serialization of STL string objects :
#include <boost/serialization/string.hpp>

// Support Boost serialization of STL vector containers :
#include <boost/serialization/vector.hpp>

// The implementation of the serialization template method :
template<class Archive>
void data::serialize (Archive & ar_,
                      const unsigned int version_)
{
  ar_ & boost::serialization::make_nvp ("bval", _bval_);
  ar_ & boost::serialization::make_nvp ("cval", _cval_);
  ar_ & boost::serialization::make_nvp ("ival", _ival_);
  ar_ & boost::serialization::make_nvp ("fval", _fval_);
  ar_ & boost::serialization::make_nvp ("dval", _dval_);
  ar_ & boost::serialization::make_nvp ("sval", _sval_);
  ar_ & boost::serialization::make_nvp ("dvec", _dvec_);
  return;
}
#endif // DATA_IPP_
