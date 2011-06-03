// -*- mode: c++; -*- 
//! \file geomtools/helix_3d.ipp

#ifndef __geomtools__helix_3d_ipp
#define __geomtools__helix_3d_ipp 1

#include <boost/archive/archive_exception.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/version.hpp>

#include <datatools/serialization/utils.h>
#include <geomtools/helix_3d.h>
	 
namespace geomtools {
  
  template<class Archive>
  void helix_3d::serialize (Archive & a_ar , const unsigned int a_version)
  {
    a_ar & boost::serialization::make_nvp ("radius", __radius);
    if (__radius == __radius)
      {
	a_ar & boost::serialization::make_nvp ("center", __center);
	a_ar & boost::serialization::make_nvp ("step", __step);
	a_ar & boost::serialization::make_nvp ("t1", __t1);
	a_ar & boost::serialization::make_nvp ("t2", __t2);
      }
    return;
  }

} // end of namespace geomtools 

//BOOST_CLASS_VERSION(geomtools::helix_3d, 0)

#endif // __geomtools__helix_3d_ipp

// end of helix_3d.ipp
