// -*- mode: c++; -*- 
//! \file datatools/utils/things.ipp

#ifndef __datatools__utils__things_ipp
#define __datatools__utils__things_ipp 1

#include <datatools/utils/things.h>

#include <boost/archive/archive_exception.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>

#include <datatools/serialization/utils.h>
#include <datatools/serialization/i_serializable.ipp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>
	 
namespace datatools {
		 
  namespace utils {

    template<class Archive>
    void things::entry_t::serialize (Archive & ar_, const unsigned int version_)
    {
      ar_ & boost::serialization::make_nvp ("description", description);
      ar_ & boost::serialization::make_nvp ("flags",       flags);
      ar_ & boost::serialization::make_nvp ("handle",      handle); 
      return;
    }
    
    template<class Archive>
    void things::serialize (Archive & ar_, const unsigned int file_version_)
    {
      ar_ & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
      ar_ & boost::serialization::make_nvp ("name",        _name_);
      ar_ & boost::serialization::make_nvp ("description", _description_);
      ar_ & boost::serialization::make_nvp ("things",      _things_);
      return;
    }

  } // end of namespace utils 

} // end of namespace datatools 

#endif // __datatools__utils__things_ipp
