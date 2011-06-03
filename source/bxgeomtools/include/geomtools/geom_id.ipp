// -*- mode: c++; -*- 
//! \file geomtools/geom_id.ipp

#ifndef __geomtools__geom_id_ipp
#define __geomtools__geom_id_ipp 1

#include <boost/archive/archive_exception.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/version.hpp>

#include <boost/serialization/vector.hpp>

#include <datatools/serialization/utils.h>
#include <geomtools/geom_id.h>
	 
namespace geomtools {
    
  template<class Archive>
  void geom_id::serialize (Archive & a_ar , const unsigned int a_version)
  {
    a_ar & boost::serialization::make_nvp ("type",    __type);
    a_ar & boost::serialization::make_nvp ("address", __address);
    return;
  }

} // end of namespace geomtools 

//BOOST_CLASS_VERSION(geomtools::geom_id, 0)

#endif // __geomtools__geom_id_ipp

// end of geom_id.ipp
