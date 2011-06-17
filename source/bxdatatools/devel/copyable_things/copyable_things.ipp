// -*- mode: c++; -*- 
//! \file datatools/utils/copyable_things.ipp

#ifndef __datatools__utils__copyable_things_ipp
#define __datatools__utils__copyable_things_ipp 1

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
#include <datatools/utils/copyable_things.h>
#include <datatools/serialization/i_serializable.ipp>
	 
namespace datatools {
		 
  namespace utils {

    template<class Archive>
    void copyable_things::entry_t::serialize (Archive & ar_, const unsigned int version_)
    {
      ar_ & boost::serialization::make_nvp ("description", description);
      ar_ & boost::serialization::make_nvp ("flags",       flags);
      ar_ & boost::serialization::make_nvp ("handle",      handle); 
      return;
    }
    
    template<class Archive>
    void copyable_things::serialize (Archive & ar_, const unsigned int file_version_)
    {
      ar_ & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
      ar_ & boost::serialization::make_nvp ("name",        m_name);
      ar_ & boost::serialization::make_nvp ("description", m_description);
      ar_ & boost::serialization::make_nvp ("copyable_things",      m_copyable_things);
      return;
    }

  } // end of namespace utils 

} // end of namespace datatools 

#endif // __datatools__utils__copyable_things_ipp
