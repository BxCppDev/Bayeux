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

//#include <datatools/serialization/archives_instantiation.h>
//#include <boost/serialization/export.hpp>

/***************************
 * genbb::primary_particle *
 ***************************/

//BOOST_CLASS_EXPORT_IMPLEMENT(genbb::primary_particle) 

/************************
 * genbb::primary_event *
 ************************/

#include <genbb_help/primary_event.ipp>
//BOOST_CLASS_EXPORT_IMPLEMENT(genbb::primary_event) 

#endif // __genbb_help__the_serializable_h__

/* end of genbb_help::the_serializable.h */
