// -*- mode: c++; -*-
/// \file the_serializable.cc

// Third Party:

// - Boost:
#include <boost/serialization/assume_abstract.hpp>
#include <boost/serialization/export.hpp>

// - Bayeux/datatools:
#include <datatools/archives_instantiation.h>

// This project:
#include <datatools/i_serializable.ipp>
#include <dpp_ex01/hit.ipp>
#include <dpp_ex01/raw_data.ipp>

/// Serialization code instantiation for class hit
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(dpp_ex01::hit)

/// Serialization code instantiation for class raw_data
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(dpp_ex01::raw_data)

// namespace {
//   void _trick()
//   {
//     datatools::things t;
//     dpp_ex01::raw_data & RD = t.add<dpp_ex01::raw_data>("RD");
//     {
//       datatools::data_writer writer("/tmp/__dpp_ex01-trick.xml", datatools::using_multi_archives);
//       writer.store(t);
//     }
//     return;
//   }
// }

/// Boost/Serialization export definition for class raw_data
BOOST_CLASS_EXPORT_IMPLEMENT(dpp_ex01::raw_data)
