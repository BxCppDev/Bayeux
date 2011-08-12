// -*- mode: c++; -*- 
//! \file datatools/utils/properties.ipp

#ifndef __datatools__utils__properties_ipp
#define __datatools__utils__properties_ipp 1

#include <boost/archive/archive_exception.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/version.hpp>
//#include <boost/serialization/tracking.hpp>

#include <boost/serialization/map.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>

#include <datatools/serialization/utils.h>
#include <datatools/utils/properties.h>

#include <datatools/serialization/i_serializable.ipp>
	 
namespace datatools {
		 
  namespace utils {
 
    template<class Archive>
    void properties::data::serialize (Archive & a_ar, 
				      const unsigned int a_version)
    {
      a_ar & boost::serialization::make_nvp ("description", description_);
      a_ar & boost::serialization::make_nvp ("flags",       flags_);
      if (is_boolean ())
	{
	  a_ar & boost::serialization::make_nvp ("boolean_values",
						boolean_values_);
	}
      if (is_integer ())
	{
	  a_ar & boost::serialization::make_nvp ("integer_values",
						integer_values_);
	}
      if (is_real ())
	{
	  a_ar & boost::serialization::make_nvp ("real_values", 
						real_values_);
	}
      if (is_string ())
	{
	  a_ar & boost::serialization::make_nvp ("string_values",
						string_values_);
	}
      return;
    }
    
    template<class Archive>
    void properties::serialize (Archive & a_ar , const unsigned int a_version)
    {
      if (a_version > 0)
	{
	  /* from version 1 we inherit explicitely from the
	   * 'datatools::serialization::i_serializable' abstract class
	   */
	  a_ar & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
	}
      a_ar & boost::serialization::make_nvp ("description", description_);
      a_ar & boost::serialization::make_nvp ("properties",  props_);
      return;
    }

  } // end of namespace utils 

} // end of namespace datatools 

BOOST_CLASS_VERSION(datatools::utils::properties, 1)
// BOOST_CLASS_TRACKING(datatools::utils::properties, boost::serialization::track_always) 
//BOOST_CLASS_EXPORT_KEY2(datatools::utils::properties, "datatools::utils::properties")

#endif // __datatools__utils__properties_ipp
