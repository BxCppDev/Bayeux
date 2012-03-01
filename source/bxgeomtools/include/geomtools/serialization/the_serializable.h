// -*- mode: c++; -*- 
/* geomtools::the_serializable.h */

#ifndef __geomtools__the_serializable_h__
#define __geomtools__the_serializable_h__ 1

#include <datatools/serialization/archives_instantiation.h>
#include <boost/serialization/export.hpp>
#include <geomtools/serialization/the_serializable.ipp>


DATATOOLS_SERIALIZATION_NON_INTRUSIVE_CLASS_SERIALIZE_INSTANTIATE_ALL(geomtools::vector_2d)
//BOOST_CLASS_EXPORT_IMPLEMENT(geomtools::vector_2d) 

DATATOOLS_SERIALIZATION_NON_INTRUSIVE_CLASS_SERIALIZE_INSTANTIATE_ALL(geomtools::vector_3d)
//BOOST_CLASS_EXPORT_IMPLEMENT(geomtools::vector_3d) 

DATATOOLS_SERIALIZATION_NON_INTRUSIVE_CLASS_SERIALIZE_INSTANTIATE_ALL(geomtools::rotation_3d)
//BOOST_CLASS_EXPORT_IMPLEMENT(geomtools::rotation_3d) 

DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(geomtools::geom_id)
BOOST_CLASS_EXPORT_IMPLEMENT(geomtools::geom_id) 

DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(geomtools::line_3d)
BOOST_CLASS_EXPORT_IMPLEMENT(geomtools::line_3d) 

DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(geomtools::helix_3d)
BOOST_CLASS_EXPORT_IMPLEMENT(geomtools::helix_3d) 

DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(geomtools::polyline_3d)
BOOST_CLASS_EXPORT_IMPLEMENT(geomtools::polyline_3d) 

#endif // __geomtools__the_serializable_h__

/* end of geomtools::the_serializable.h */
