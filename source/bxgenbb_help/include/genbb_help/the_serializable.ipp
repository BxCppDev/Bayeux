// -*- mode: c++; -*- 
/* genbb_help::the_serializable.ipp */

#ifndef __genbb_help__the_serializable_ipp__
#define __genbb_help__the_serializable_ipp__ 1

/** CAUTION:
 *  For now (2011-03-20), the genbb_help serializable classes are not exported,
 *  only their serialization methods are instantiated for all archives.
 *  - genbb::primary_particle
 *  - genbb::primary_event
 */

//#include <boost/serialization/export.hpp>
//#include <datatools/serialization/archives_instantiation.h>
//#include <datatools/serialization/i_serializable.ipp>

/***************************
 * genbb::primary_particle *
 ***************************/

#include <genbb_help/primary_particle.ipp>

/************************
 * genbb::primary_event *
 ************************/

#include <genbb_help/primary_event.ipp>

#endif // __genbb_help__the_serializable_ipp__

/* end of genbb_help::the_serializable.ipp */
