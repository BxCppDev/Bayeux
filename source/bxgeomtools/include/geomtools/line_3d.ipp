// -*- mode: c++; -*- 
//! \file geomtools/line_3d.ipp

#ifndef __geomtools__line_3d_ipp
#define __geomtools__line_3d_ipp 1

#include <boost/archive/archive_exception.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>

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
    a_ar & boost::serialization::make_nvp ("first", _first_);
    a_ar & boost::serialization::make_nvp ("last",  _last_);
    return;
  }

} // end of namespace geomtools 

#include <boost/serialization/version.hpp>
BOOST_CLASS_VERSION(geomtools::line_3d, 1)

#endif // __geomtools__line_3d_ipp

// end of line_3d.ipp
