// -*- mode: c++; -*- 
//! \file geomtools/line_3d.ipp

#ifndef __geomtools__line_3d_ipp
#define __geomtools__line_3d_ipp 1

#include <boost/archive/archive_exception.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/version.hpp>

#include <datatools/serialization/utils.h>
#include <geomtools/line_3d.h>
	 
namespace geomtools {
  
  template<class Archive>
  void line_3d::serialize (Archive & a_ar , const unsigned int a_version)
  {
    a_ar & boost::serialization::make_nvp ("first", __first);
    a_ar & boost::serialization::make_nvp ("last", __last);
    return;
  }

} // end of namespace geomtools 

//BOOST_CLASS_VERSION(geomtools::line_3d, 0)

#endif // __geomtools__line_3d_ipp

// end of line_3d.ipp
