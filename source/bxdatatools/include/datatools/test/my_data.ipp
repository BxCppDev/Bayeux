// -*- mode: c++; -*- 
//! \file datatools/test/my_data.ipp

#ifndef __datatools__test__my_data_ipp
#define __datatools__test__my_data_ipp 1

#include <boost/archive/archive_exception.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/version.hpp>

#include <boost/serialization/map.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/vector.hpp> // missing header: fixed 2010-03-16
#include <boost/serialization/string.hpp>

#include <datatools/serialization/utils.h>
#include <datatools/test/my_data.h>
	 
namespace datatools {
		 
  namespace test {

    template<class Archive>
    void data_t::serialize (Archive            & ar_, 
			    const unsigned int   version_)
    {
      ar_ & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
      ar_ & boost::serialization::make_nvp ("flags", __flags);
      ar_ & boost::serialization::make_nvp ("ival",  __ival);
      ar_ & boost::serialization::make_nvp ("dval",  __dval);
      ar_ & boost::serialization::make_nvp ("dvals", __dvals);
      return;
    }

    template<class Archive>
    void more_data_t::serialize (Archive &          ar_, 
				 const unsigned int version_)
    {
      ar_ & BOOST_SERIALIZATION_BASE_OBJECT_NVP (data_t);
      ar_ & boost::serialization::make_nvp ("name", __name);
      return;
    }

  } // end of namespace test 

} // end of namespace datatools 

#endif // __datatools__test__my_data_ipp

