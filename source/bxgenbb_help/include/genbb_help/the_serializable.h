// -*- mode: c++; -*- 
/* genbb_help::the_serializable.h */

#ifndef __genbb_help__the_serializable_h__
#define __genbb_help__the_serializable_h__ 1

/** CAUTION:
 *  For now (2011-03-20), the genbb_help serializable classes are not exported,
 *  only their serialization methods are instantiated for all archives.
 *  - genbb::primary_particle
 *  - genbb::primary_event
 */

//#include <boost/serialization/export.hpp>

#include <datatools/serialization/archives_instantiation.h>
//#include <datatools/base_serializable.h>

/***************************
 * genbb::primary_particle *
 ***************************/

#include <genbb_help/primary_particle.ipp>
//BOOST_CLASS_EXPORT_KEY2(genbb::primary_particle, "genbb::primary_particle")
//DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(genbb::primary_particle)
//BOOST_CLASS_EXPORT_IMPLEMENT(genbb::primary_particle) 

/************************
 * genbb::primary_event *
 ************************/

#include <genbb_help/primary_event.ipp>
//BOOST_CLASS_EXPORT_KEY2(genbb::primary_event, "genbb::primary_event")
//DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(genbb::primary_event)
//BOOST_CLASS_EXPORT_IMPLEMENT(genbb::primary_event) 

#endif // __genbb_help__the_serializable_h__

/* end of genbb_help::the_serializable.h */
