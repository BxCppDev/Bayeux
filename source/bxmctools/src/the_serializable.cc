// -*- mode: c++; -*-
/* the_serializable.cc
 */
// Load the Boost/Serialization instantiation and registration code :

// Third party:
// - Boost :
#include <boost/serialization/export.hpp>
// - Bayeux/datatools :
#include <datatools/archives_instantiation.h>

// This project:
#include <mctools/base_step_hit.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(mctools::base_step_hit)
BOOST_CLASS_EXPORT_IMPLEMENT(mctools::base_step_hit)

#include <mctools/signal/base_signal.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(mctools::signal::base_signal)

#include <mctools/signal/signal_data.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(mctools::signal::signal_data)
BOOST_CLASS_EXPORT_IMPLEMENT(mctools::signal::signal_data)

#include <mctools/digitization/sampled_signal.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(mctools::digitization::sampled_signal)

#include <mctools/simulated_data.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(mctools::simulated_data)
BOOST_CLASS_EXPORT_IMPLEMENT(mctools::simulated_data)
