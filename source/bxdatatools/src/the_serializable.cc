// the_serializable.cc

// Third Party:
// - Boost:
#include <boost/serialization/assume_abstract.hpp>
#include <boost/serialization/export.hpp>

// This project:
#include <datatools/archives_instantiation.h>

// Headers for serialization defs
#include <datatools/i_serializable.ipp>
#include <datatools/enriched_base.ipp>
#include <datatools/properties.ipp>
#include <datatools/multi_properties.ipp>
#include <datatools/things.ipp>
#include <datatools/event_id.ipp>
#include <datatools/version_id.ipp>
#include <datatools/urn.ipp>
#include <datatools/urn_info.ipp>
#include <datatools/urn_to_path.ipp>

/** Macros to automatically instantiate Boost/Serialization-based
 * code associated to serializable classes.
 *
 */

/********************************************
 * datatools::i_serializable                *
 ********************************************/
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(datatools::i_serializable)

/********************************
 * datatools::enriched_base     *
 ********************************/
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(datatools::enriched_base)
BOOST_CLASS_EXPORT_IMPLEMENT(datatools::enriched_base)

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

/*********************************
 * datatools::version_id         *
 *********************************/
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(datatools::version_id)

/*********************************
 * datatools::urn                *
 *********************************/
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(datatools::urn)

/*********************************
 * datatools::urn_info           *
 *********************************/
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(datatools::urn_info)

/*********************************
 * datatools::urn_to_path        *
 *********************************/
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(datatools::urn_to_path)
