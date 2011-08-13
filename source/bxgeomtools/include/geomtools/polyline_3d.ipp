// -*- mode: c++; -*- 
//! \file geomtools/polyline_3d.ipp

#ifndef __geomtools__polyline_3d_ipp
#define __geomtools__polyline_3d_ipp 1

#include <boost/archive/archive_exception.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
//#include <boost/serialization/version.hpp>
//#include <boost/serialization/tracking.hpp>
//#include <boost/serialization/export.hpp>
#include <boost/serialization/list.hpp>

#include <datatools/serialization/utils.h>
#include <geomtools/polyline_3d.h>
	 
namespace geomtools {
  
  template<class Archive>
  void polyline_3d::serialize (Archive & a_ar , const unsigned int a_version)
  {
    a_ar & boost::serialization::make_nvp ("closed", __closed);
    a_ar & boost::serialization::make_nvp ("points", __points);
    return;
  }

} // end of namespace geomtools 

//BOOST_CLASS_VERSION(geomtools::polyline_3d, 0)
//BOOST_CLASS_TRACKING(geomtools::polyline_3d, boost::serialization::track_always) 
//BOOST_CLASS_EXPORT_KEY2(geomtools::polyline_3d, "geomtools::polyline_3d")

#endif // __geomtools__polyline_3d_ipp

// end of polyline_3d.ipp
