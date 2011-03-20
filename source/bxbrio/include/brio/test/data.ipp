// -*- mode: c++; -*- 
//! \file brio/test/data.ipp

#ifndef __brio__test__data_ipp
#define __brio__test__data_ipp 1

#include <boost/serialization/base_object.hpp>

// Support for XML 'key-value' based archives:
#include <boost/serialization/nvp.hpp>

// Support Boost serialization of STL vector templatized objects:
#include <boost/serialization/vector.hpp> 

// Support Boost serialization of STL string objects:
#include <boost/serialization/string.hpp>

#include <datatools/serialization/utils.h>
#include <brio/test/data.h>
	 
namespace brio {
		 
  namespace test {

    template<class Archive>
    void data::serialize (Archive & ar,                // an archive type (ASCII, XML or binary)
			  const unsigned int version)  // the version number (here not used)
    {
      ar & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
      ar & boost::serialization::make_nvp ("bval",     __bval);
      ar & boost::serialization::make_nvp ("cval",     __cval);
      ar & boost::serialization::make_nvp ("ival",     __ival);
      ar & boost::serialization::make_nvp ("fval",     __fval);
      ar & boost::serialization::make_nvp ("dval",     __dval);
      ar & boost::serialization::make_nvp ("sval",     __sval);
      ar & boost::serialization::make_nvp ("dval_vec", __dval_vec);
      return;
    }

  } // end of namespace test 

} // end of namespace brio 

#endif // __brio__test__data_ipp

