// -*- mode: c++; -*- 
/* genbb_help::the_serializable.h */

#ifndef __genbb_help__the_serializable_h__
#define __genbb_help__the_serializable_h__ 1

#include <boost/serialization/export.hpp>
#include <datatools/serialization/archives_instantiation.h>
#include <genbb_help/the_serializable.ipp>

/** Information:
 *  For now (2011-09-29), only the genbb::primary_event serializable classe
 *  is exported,
 *  All serialization methods are instantiated for all archives.
 *  - genbb::primary_particle
 *  - genbb::primary_event
 */


/***************************
 * genbb::primary_particle *
 ***************************/

DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(genbb::primary_particle)

/************************
 * genbb::primary_event *
 ************************/

DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(genbb::primary_event)
BOOST_CLASS_EXPORT_IMPLEMENT(genbb::primary_event) 

#endif // __genbb_help__the_serializable_h__

/* end of genbb_help::the_serializable.h */
