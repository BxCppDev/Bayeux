// the_serializable.cc

// Third party:
// - Boost:
#include <boost/serialization/export.hpp>
// - Bayeux/datatools:
#include <datatools/archives_instantiation.h>

// This project:
#include <genbb_help/primary_particle.ipp>
#include <genbb_help/primary_event.ipp>

/***************************
 * genbb::primary_particle *
 ***************************/

DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(genbb::primary_particle)

/************************
 * genbb::primary_event *
 ************************/

DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(genbb::primary_event)
BOOST_CLASS_EXPORT_IMPLEMENT(genbb::primary_event)

