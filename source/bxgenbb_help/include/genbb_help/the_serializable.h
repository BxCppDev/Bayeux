/// \file genbb_help/the_serializable.h

#ifndef GENBB_HELP_THE_SERIALIZABLE_H
#define GENBB_HELP_THE_SERIALIZABLE_H 1

// Third party:
// - Boost:
#include <boost/serialization/export.hpp>
// - Bayeux/datatools:
#include <datatools/archives_instantiation.h>

// This project:
#include <genbb_help/the_serializable.ipp>

/***************************
 * genbb::primary_particle *
 ***************************/

DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(genbb::primary_particle)

/************************
 * genbb::primary_event *
 ************************/

DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(genbb::primary_event)
BOOST_CLASS_EXPORT_IMPLEMENT(genbb::primary_event)

#endif // GENBB_HELP_THE_SERIALIZABLE_H

// Local Variables: --
// mode: c++ --
// End: --
