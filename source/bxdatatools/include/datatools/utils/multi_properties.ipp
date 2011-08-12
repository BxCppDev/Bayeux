// -*- mode: c++; -*- 
//! \file datatools/utils/multi_properties.ipp

#ifndef __datatools__utils__multi_properties_ipp
#define __datatools__utils__multi_properties_ipp 1

#include <boost/archive/archive_exception.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
//#include <boost/serialization/tracking.hpp>
#include <boost/serialization/version.hpp>

#include <boost/serialization/map.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/vector.hpp> 
#include <boost/serialization/string.hpp>

#include <datatools/serialization/utils.h>

#include <datatools/utils/multi_properties.h>

#include <datatools/serialization/i_serializable.ipp>
#include <datatools/utils/properties.ipp>
	 
namespace datatools {
		 
  namespace utils {
 
    template<class Archive>
    void multi_properties::entry::serialize (Archive & a_ar, 
					     const unsigned int   a_version)
    {
      a_ar & boost::serialization::make_nvp ("key",        key_);
      a_ar & boost::serialization::make_nvp ("meta",       meta_);
      a_ar & boost::serialization::make_nvp ("properties", properties_);
      return;
    }

    /*
    template<class Archive>
    void multi_properties::save (Archive & a_ar, const unsigned int a_version) const
    {
      a_ar & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
      a_ar & boost::serialization::make_nvp ("description",      __description);
      a_ar & boost::serialization::make_nvp ("key_label",        __key_label);
      a_ar & boost::serialization::make_nvp ("meta_label",       __meta_label);
      a_ar & boost::serialization::make_nvp ("entries",          __entries);
      a_ar & boost::serialization::make_nvp ("ordered_entries",  __ordered_entries);
      return;
    }

    template<class Archive>
    void multi_properties::load (Archive & a_ar , const unsigned int a_version)
    {
      if (a_version >= 1)
	{
	  // from version 1 we inherit explicitely from the
	  // 'datatools::serialization::i_serializable' abstract class
	  a_ar & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
	}
      a_ar & boost::serialization::make_nvp ("description",      __description);
      a_ar & boost::serialization::make_nvp ("key_label",        __key_label);
      a_ar & boost::serialization::make_nvp ("meta_label",       __meta_label);
      a_ar & boost::serialization::make_nvp ("entries",          __entries);
      a_ar & boost::serialization::make_nvp ("ordered_entries",  __ordered_entries);
      return;
    }
    
    BOOST_SERIALIZATION_SPLIT_MEMBER_SERIALIZE_IMPLEMENTATION(multi_properties)
    */

    template<class Archive>
    void multi_properties::serialize (Archive & a_ar , const unsigned int a_version)
    {
      if (a_version >= 1)
	{
	  // from version 1 we inherit explicitely from the
	  // 'datatools::serialization::i_serializable' abstract class
	  a_ar & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
	}
      a_ar & boost::serialization::make_nvp ("description",      description_);
      a_ar & boost::serialization::make_nvp ("key_label",        key_label_);
      a_ar & boost::serialization::make_nvp ("meta_label",       meta_label_);
      a_ar & boost::serialization::make_nvp ("entries",          entries_);
      a_ar & boost::serialization::make_nvp ("ordered_entries",  ordered_entries_);
      return;
    }

  } // end of namespace utils 

} // end of namespace datatools 

BOOST_CLASS_VERSION(datatools::utils::multi_properties, 1)
// BOOST_CLASS_TRACKING(datatools::utils::multi_properties, boost::serialization::track_always) 
//BOOST_CLASS_EXPORT_KEY2(datatools::utils::multi_properties, "datatools::utils::multi_properties")

#endif // __datatools__utils__multi_properties_ipp
