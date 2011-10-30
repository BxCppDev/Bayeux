// -*- mode: c++; -*- 
//! \file datatools/utils/multi_properties.ipp

#ifndef __datatools__utils__multi_properties_ipp
#define __datatools__utils__multi_properties_ipp 1

#include <datatools/utils/multi_properties.h>

#include <boost/archive/archive_exception.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>

#include <datatools/serialization/utils.h>
#include <datatools/serialization/i_serializable.ipp>
#include <datatools/utils/properties.ipp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/vector.hpp> 
#include <boost/serialization/string.hpp>
	 
namespace datatools {
		 
  namespace utils {
 
    template<class Archive>
    void multi_properties::entry::serialize (Archive & a_ar, 
					     const unsigned int a_version)
    {
      a_ar & boost::serialization::make_nvp ("key",        _key_);
      a_ar & boost::serialization::make_nvp ("meta",       _meta_);
      a_ar & boost::serialization::make_nvp ("properties", _properties_);
      return;
    }

    template<class Archive>
    void multi_properties::serialize (Archive & a_ar , const unsigned int a_version)
    {
      if (a_version >= 1)
	{
	  // from version 1 we inherit explicitely from the
	  // 'datatools::serialization::i_serializable' abstract class
	  a_ar & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
	}
      a_ar & boost::serialization::make_nvp ("description",      _description_);
      a_ar & boost::serialization::make_nvp ("key_label",        _key_label_);
      a_ar & boost::serialization::make_nvp ("meta_label",       _meta_label_);
      a_ar & boost::serialization::make_nvp ("entries",          _entries_);
      a_ar & boost::serialization::make_nvp ("ordered_entries",  _ordered_entries_);
      return;
    }

  } // end of namespace utils 

} // end of namespace datatools 

#include <boost/serialization/version.hpp>
BOOST_CLASS_VERSION(datatools::utils::multi_properties, 1)

#endif // __datatools__utils__multi_properties_ipp
