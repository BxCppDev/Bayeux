// -*- mode: c++; -*-
/// \file foo.ipp

#ifndef FOO_IPP
#define FOO_IPP 1

// This example:
#include <foo.h>

// Support for inheritance from an serializable class :
#include <boost/serialization/base_object.hpp>

// Support for XML 'key-value' based archives:
#include <boost/serialization/nvp.hpp>

// Support Boost serialization of STL vector templatized objects:
#include <boost/serialization/vector.hpp>

// Support Boost serialization of STL string objects:
#include <boost/serialization/string.hpp>

// Datatools support for serializable objects :
#include <datatools/i_serializable.ipp>

/** The main Boost/Serialization template method for class foo
 *  @arg ar an archive type (ASCII, XML or binary)
 *  @arg version the archive version number (not used)
 */
template<class Archive>
void foo::serialize (Archive & ar, const unsigned int version)
{
  ar & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
  ar & boost::serialization::make_nvp ("bval",     _bval_);
  ar & boost::serialization::make_nvp ("cval",     _cval_);
  ar & boost::serialization::make_nvp ("ival",     _ival_);
  ar & boost::serialization::make_nvp ("fval",     _fval_);
  ar & boost::serialization::make_nvp ("dval",     _dval_);
  ar & boost::serialization::make_nvp ("sval",     _sval_);
  ar & boost::serialization::make_nvp ("dval_vec", _dval_vec_);
  return;
}

#endif // FOO_IPP
