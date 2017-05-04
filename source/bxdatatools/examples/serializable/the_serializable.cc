// -*- mode: c++; -*-
/// \file the_serializable.cc
/// \brief The serialization code for the Boost I/O library

// Third Party
// - Boost:
#include <boost/serialization/assume_abstract.hpp>
#include <boost/serialization/export.hpp>
// - Bayeux/datatools:
#include <datatools/archives_instantiation.h>

// Include the specific serialization code of serializable classes:
#include <raw_hit.ipp>
#include <raw_data.ipp>

// Force instantiation of template code associated to the classes above
// for the Boost archive classes defined in Bayeux/datatools:
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(raw_hit)
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(raw_data)

// Special implentation macro to register the class through the
// Boost/Serialization system :
BOOST_CLASS_EXPORT_IMPLEMENT(raw_data)
