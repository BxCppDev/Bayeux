// -*- mode: c++; -*- 
//! \file datatools/utils/properties.ipp

#ifndef __datatools__utils__properties_ipp
#define __datatools__utils__properties_ipp 1

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
#include <datatools/utils/properties.h>
	 
namespace datatools {
		 
  namespace utils {
 
    template<class Archive>
    void properties::data::serialize (Archive & ar_, 
				      const unsigned int   version_)
    {
      ar_ & boost::serialization::make_nvp ("description", __description);
      ar_ & boost::serialization::make_nvp ("flags",       __flags);
      if (is_boolean ())
	{
	  ar_ & boost::serialization::make_nvp ("boolean_values",
						__boolean_values);
	}
      if (is_integer ())
	{
	  ar_ & boost::serialization::make_nvp ("integer_values",
						__integer_values);
	}
      if (is_real ())
	{
	  ar_ & boost::serialization::make_nvp ("real_values", 
						__real_values);
	}
      if (is_string ())
	{
	  ar_ & boost::serialization::make_nvp ("string_values",
						__string_values);
	}
      return;
    }

    /*
    template<class Archive>
    void properties::save (Archive & ar_, const unsigned int version_) const
    {
      ar_ & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
      ar_ & boost::serialization::make_nvp ("description", __description);
      ar_ & boost::serialization::make_nvp ("properties",  __props);	
      return;
    }
    
    template<class Archive>
    void properties::load (Archive & ar_ , const unsigned int version_)
    {
      if (version_ > 0)
	{
	  // from version 1 we inherit explicitely from the
	  // 'datatools::serialization::i_serializable' abstract class
	  ar_ & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
	}
      ar_ & boost::serialization::make_nvp ("description", __description);
      ar_ & boost::serialization::make_nvp ("properties",  __props);
      return;
    }

    BOOST_SERIALIZATION_SPLIT_MEMBER_SERIALIZE_IMPLEMENTATION(properties)
    */

    template<class Archive>
    void properties::serialize (Archive & ar_ , const unsigned int version_)
    {
      if (version_ > 0)
	{
	  /* from version 1 we inherit explicitely from the
	   * 'datatools::serialization::i_serializable' abstract class
	   */
	  ar_ & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
	}
      ar_ & boost::serialization::make_nvp ("description", __description);
      ar_ & boost::serialization::make_nvp ("properties",  __props);
      return;
    }

  } // end of namespace utils 

} // end of namespace datatools 

BOOST_CLASS_VERSION(datatools::utils::properties, 1)

#endif // __datatools__utils__properties_ipp
