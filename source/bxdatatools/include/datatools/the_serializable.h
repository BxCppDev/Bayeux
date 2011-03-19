// -*- mode: c++; -*- 
/* datatools::serializables.h */

#ifndef __datatools__serializables_h__
#define __datatools__serializables_h__ 1

#include <boost/serialization/export.hpp>

#include <datatools/serialization/archives_instantiation.h>
#include <datatools/base_serializable.h>

/********************************
 * datatools::utils::properties *
 ********************************/
#include <datatools/utils/properties.ipp>
BOOST_CLASS_EXPORT_KEY2(datatools::utils::properties, "datatools::utils::properties")
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(datatools::utils::properties)
BOOST_CLASS_EXPORT_IMPLEMENT(datatools::utils::properties) 


/**************************************
 * datatools::utils::multi_properties *
 **************************************/
#include <datatools/utils/multi_properties.ipp>
BOOST_CLASS_EXPORT_KEY2(datatools::utils::multi_properties, "datatools::utils::multi_properties")
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(datatools::utils::multi_properties)
BOOST_CLASS_EXPORT_IMPLEMENT(datatools::utils::multi_properties) 


/****************************
 * datatools::utils::things *
 ****************************/
#include <datatools/utils/things.ipp>
BOOST_CLASS_EXPORT_KEY2(datatools::utils::things, "datatools::utils::things")
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(datatools::utils::things)
BOOST_CLASS_EXPORT_IMPLEMENT(datatools::utils::things) 

/*********************************
 * datatools::event::event_id *
 *********************************/
#include <datatools/event/event_id.ipp>
BOOST_CLASS_EXPORT_KEY2(datatools::event::event_id, "datatools::event::event_id")
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(datatools::event::event_id)
BOOST_CLASS_EXPORT_IMPLEMENT(datatools::event::event_id) 


/*********************************
 * datatools::event::basic_event *
 *********************************/
#include <datatools/event/basic_event.ipp>
BOOST_CLASS_EXPORT_KEY2(datatools::event::basic_event, "datatools::event::basic_event")
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(datatools::event::basic_event)
BOOST_CLASS_EXPORT_IMPLEMENT(datatools::event::basic_event) 

#endif // __datatools__serializables_h__

/* end of datatools::serializables.h */
