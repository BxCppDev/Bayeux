// -*- mode: c++; -*- 
//! \file datatools/test/my_data.ipp

#ifndef __datatools__test__my_data_ipp
#define __datatools__test__my_data_ipp 1

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/version.hpp>

#include <boost/serialization/vector.hpp> // missing header: fixed 2010-03-16
#include <boost/serialization/string.hpp>

#include <datatools/serialization/utils.h>

#include <datatools_test_my_data.h>

#include <datatools/serialization/i_serializable.ipp>
	 
namespace datatools {
		 
  namespace test {

    template<class Archive>
    void data_t::serialize (Archive            & a_ar, 
			    const unsigned int   a_version)
    {
      a_ar & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
      a_ar & boost::serialization::make_nvp ("flags", __flags);
      a_ar & boost::serialization::make_nvp ("ival",  __ival);
      a_ar & boost::serialization::make_nvp ("dval",  __dval);
      a_ar & boost::serialization::make_nvp ("dvals", __dvals);
      return;
    }

    template<class Archive>
    void more_data_t::serialize (Archive &          a_ar, 
				 const unsigned int a_version)
    {
      a_ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP (data_t);
      a_ar & boost::serialization::make_nvp ("name", __name);
      return;
    }

  } // end of namespace test 

} // end of namespace datatools 

#endif // __datatools__test__my_data_ipp

