// -*- mode: c++; -*- 
//! \file geomtools/helix_3d.ipp

#ifndef __geomtools__helix_3d_ipp
#define __geomtools__helix_3d_ipp 1

#include <boost/archive/archive_exception.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/version.hpp>
//#include <boost/serialization/tracking.hpp>
#include <boost/serialization/export.hpp>

#include <datatools/serialization/utils.h>
#include <geomtools/helix_3d.h>
#include <datatools/serialization/i_serializable.ipp>
	 
namespace geomtools {
  
  template<class Archive>
  void helix_3d::serialize (Archive & a_ar , const unsigned int a_version)
  {
    if (a_version > 0)
      {
	/* from version 1 we inherit explicitely from the
	 * 'datatools::serialization::i_serializable' abstract class
	 */
	a_ar & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
      }
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

BOOST_CLASS_VERSION(geomtools::helix_3d, 1)
//BOOST_CLASS_TRACKING(geomtools::helix_3d, boost::serialization::track_always) 
BOOST_CLASS_EXPORT_KEY2(geomtools::helix_3d, "geomtools::helix_3d")

#endif // __geomtools__helix_3d_ipp

// end of helix_3d.ipp
