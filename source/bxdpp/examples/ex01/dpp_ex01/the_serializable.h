// -*- mode: c++; -*-
/// \file the_serializable.h

#ifndef DPP_EX01_THE_SERIALIZABLE_H_
#define DPP_EX01_THE_SERIALIZABLE_H_ 1

// Third Party
// - Boost
#include <boost/serialization/export.hpp>

// Datatools
#include <datatools/archives_instantiation.h>

#include <dpp_ex01/the_serializable.ipp>

/// Serialization code instantiation for class hit
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(dpp_ex01::hit)

/// Serialization code instantiation for class raw_data
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(dpp_ex01::raw_data)

/// Boost/Serialization export definition for class raw_data
BOOST_CLASS_EXPORT_IMPLEMENT(dpp_ex01::raw_data)

#endif // DPP_EX01_THE_SERIALIZABLE_H_
