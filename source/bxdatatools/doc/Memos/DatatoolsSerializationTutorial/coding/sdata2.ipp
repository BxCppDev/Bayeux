// -*- mode: c++; -*-
#include "sdata2.h"

#include <boost/serialization/nvp.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>

template<class Archive>
void sdata::serialize (Archive & ar_, 
		       const unsigned int version_) 
{
  // Shortcut macro to implement serialization code for 
  // the inherited abstract class :
  ar_ & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;

  ar_ & boost::serialization::make_nvp ("bval", _bval_); 
  ar_ & boost::serialization::make_nvp ("cval", _cval_);
  ar_ & boost::serialization::make_nvp ("ival", _ival_);
  ar_ & boost::serialization::make_nvp ("fval", _fval_);
  ar_ & boost::serialization::make_nvp ("dval", _dval_);
  ar_ & boost::serialization::make_nvp ("sval", _sval_);
  ar_ & boost::serialization::make_nvp ("dvec", _dvec_);
  return;
}
