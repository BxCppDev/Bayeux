// -*- mode: c++; -*- 
//! \file geomtools/line_3d.ipp

#ifndef __geomtools__line_3d_ipp
#define __geomtools__line_3d_ipp 1

#include <boost/archive/archive_exception.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/version.hpp>
//#include <boost/serialization/tracking.hpp>
#include <boost/serialization/export.hpp>

#include <datatools/serialization/utils.h>
#include <geomtools/line_3d.h>
#include <datatools/serialization/i_serializable.ipp>
	 
namespace geomtools {
  
  template<class Archive>
  void line_3d::serialize (Archive & a_ar , const unsigned int a_version)
  {
    if (a_version > 0)
      {
	/* from version 1 we inherit explicitely from the
	 * 'datatools::serialization::i_serializable' abstract class
	 */
	a_ar & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
      }
    a_ar & boost::serialization::make_nvp ("first", __first);
    a_ar & boost::serialization::make_nvp ("last", __last);
    return;
  }

} // end of namespace geomtools 

BOOST_CLASS_VERSION(geomtools::line_3d, 1)
//BOOST_CLASS_TRACKING(geomtools::line_3d, boost::serialization::track_always) 
BOOST_CLASS_EXPORT_KEY2(geomtools::line_3d, "geomtools::line_3d")

#endif // __geomtools__line_3d_ipp

// end of line_3d.ipp
