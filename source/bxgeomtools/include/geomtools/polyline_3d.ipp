// -*- mode: c++; -*- 
//! \file geomtools/polyline_3d.ipp

#ifndef __geomtools__polyline_3d_ipp
#define __geomtools__polyline_3d_ipp 1

#include <boost/archive/archive_exception.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
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

#endif // __geomtools__polyline_3d_ipp

// end of polyline_3d.ipp
