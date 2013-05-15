// -*- mode: c++; -*-
/* the_serializable.h */
#ifndef THE_SERIALIZABLE_H_
#define THE_SERIALIZABLE_H_ 1

// Third Party
// - Boost
#include <boost/serialization/assume_abstract.hpp>
#include <boost/serialization/export.hpp>

// Datatools
#include <datatools/archives_instantiation.h>

// Include the specific serialization code of serializable classes
#include <raw_hit.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(raw_hit)

// Include the specific serialization code of serializable classes
#include <raw_data.ipp>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(raw_data)
// Special implentation macro to register the class through the Boost/Serialization system :
BOOST_CLASS_EXPORT_IMPLEMENT(raw_data)

#endif // THE_SERIALIZABLE_H_
