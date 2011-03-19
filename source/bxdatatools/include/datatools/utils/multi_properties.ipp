// -*- mode: c++; -*- 
//! \file datatools/utils/multi_properties.ipp

#ifndef __datatools__utils__multi_properties_ipp
#define __datatools__utils__multi_properties_ipp 1

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
#include <datatools/utils/multi_properties.h>
	 
namespace datatools {
		 
  namespace utils {
 
    template<class Archive>
    void multi_properties::entry::serialize (Archive & ar_, 
					     const unsigned int   version_)
    {
      ar_ & boost::serialization::make_nvp ("key", __key);
      ar_ & boost::serialization::make_nvp ("meta", __meta);
      ar_ & boost::serialization::make_nvp ("properties", __properties);
      return;
    }

      template<class Archive>
      void multi_properties::save (Archive & ar_, const unsigned int version_) const
      {
	ar_ & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
	ar_ & boost::serialization::make_nvp ("description",      __description);
	ar_ & boost::serialization::make_nvp ("key_label",        __key_label);
	ar_ & boost::serialization::make_nvp ("meta_label",       __meta_label);
	ar_ & boost::serialization::make_nvp ("entries",          __entries);
	ar_ & boost::serialization::make_nvp ("ordered_entries",  __ordered_entries);
	return;
      }

      template<class Archive>
      void multi_properties::load (Archive & ar_ , const unsigned int version_)
      {
	if (version_ >= 1)
	  {
	    // from version 1 we inherit explicitely from the
	    // 'datatools::serialization::i_serializable' abstract class
	    //
	    ar_ & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
	  }
	ar_ & boost::serialization::make_nvp ("description",      __description);
	ar_ & boost::serialization::make_nvp ("key_label",        __key_label);
	ar_ & boost::serialization::make_nvp ("meta_label",       __meta_label);
	ar_ & boost::serialization::make_nvp ("entries",          __entries);
	ar_ & boost::serialization::make_nvp ("ordered_entries",  __ordered_entries);
	return;
      }
    
    BOOST_SERIALIZATION_SPLIT_MEMBER_SERIALIZE_IMPLEMENTATION(multi_properties)

  } // end of namespace utils 

} // end of namespace datatools 

BOOST_CLASS_VERSION(datatools::utils::multi_properties, 1)

#endif // __datatools__utils__multi_properties_ipp
