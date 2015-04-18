/// \file datatools/the_serializable.h

#ifndef DATATOOLS_THE_SERIALIZABLE_H
#define DATATOOLS_THE_SERIALIZABLE_H

// Third Party:
// - Boost:
#include <boost/serialization/assume_abstract.hpp>
#include <boost/serialization/export.hpp>

// This project:
#include <datatools/archives_instantiation.h>
#include <datatools/the_serializable.ipp>

/** Macros to automatically instantiate Boost/Serialization-based
 * code associated to serializable classes.
 *
 */

/********************************************
 * datatools::i_serializable                *
 ********************************************/
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(datatools::i_serializable)

/********************************
 * datatools::properties        *
 ********************************/
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(datatools::properties)
BOOST_CLASS_EXPORT_IMPLEMENT(datatools::properties)

/**************************************
 * datatools::multi_properties        *
 **************************************/
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(datatools::multi_properties)
  BOOST_CLASS_EXPORT_IMPLEMENT(datatools::multi_properties)

/****************************
 * datatools::things        *
 ****************************/
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(datatools::things)
BOOST_CLASS_EXPORT_IMPLEMENT(datatools::things)

/*********************************
 * datatools::event_id           *
 *********************************/
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(datatools::event_id)

#endif // DATATOOLS_THE_SERIALIZABLE_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
