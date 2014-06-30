/// \file mctools/the_serializable.h

#ifndef MCTOOLS_THE_SERIALIZABLE_H
#define MCTOOLS_THE_SERIALIZABLE_H 1

// Third party:
// - Boost :
#include <boost/serialization/export.hpp>
// - Bayeux/datatools :
#include <datatools/archives_instantiation.h>

// This project:
#include <mctools/base_step_hit.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(mctools::base_step_hit)


#include <mctools/simulated_data.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(mctools::simulated_data)
BOOST_CLASS_EXPORT_IMPLEMENT(mctools::simulated_data)

#endif // MCTOOLS_THE_SERIALIZABLE_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
