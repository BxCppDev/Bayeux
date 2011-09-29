// -*- mode: c++; -*- 
//! \file geomtools/geom_id.ipp

#ifndef __geomtools__geom_id_ipp
#define __geomtools__geom_id_ipp 1

#include <boost/archive/archive_exception.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>

#include <boost/serialization/vector.hpp>

#include <datatools/serialization/utils.h>
#include <geomtools/geom_id.h>
#include <datatools/serialization/i_serializable.ipp>
	 
namespace geomtools {
    
  template<class Archive>
  void geom_id::serialize (Archive & a_ar , const unsigned int a_version)
  {
    if (a_version > 0)
      {
	/* from version 1 we inherit explicitely from the
	 * 'datatools::serialization::i_serializable' abstract class
	 */
	a_ar & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
      }
    a_ar & boost::serialization::make_nvp ("type",    __type);
    a_ar & boost::serialization::make_nvp ("address", __address);
    return;
  }

} // end of namespace geomtools 

#include <boost/serialization/version.hpp>
BOOST_CLASS_VERSION(geomtools::geom_id, 1)

#endif // __geomtools__geom_id_ipp

// end of geom_id.ipp
