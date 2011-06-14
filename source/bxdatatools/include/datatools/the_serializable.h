// -*- mode: c++; -*- 
/* datatools::the_serializable.h */

#ifndef __datatools__the_serializable_h__
#define __datatools__the_serializable_h__ 1

#include <datatools/serialization/archives_instantiation.h>
#include <boost/serialization/export.hpp>

#include <datatools/the_serializable.ipp>

/********************************************
 * datatools::serialization::i_serializable *
 ********************************************/
BOOST_CLASS_EXPORT_IMPLEMENT(datatools::serialization::i_serializable) 


/********************************
 * datatools::utils::properties *
 ********************************/
//DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(datatools::utils::properties)
BOOST_CLASS_EXPORT_IMPLEMENT(datatools::utils::properties) 


/**************************************
 * datatools::utils::multi_properties *
 **************************************/
//DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(datatools::utils::multi_properties)
BOOST_CLASS_EXPORT_IMPLEMENT(datatools::utils::multi_properties) 


/****************************
 * datatools::utils::things *
 ****************************/
//DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(datatools::utils::things)
BOOST_CLASS_EXPORT_IMPLEMENT(datatools::utils::things) 


/*********************************
 * datatools::event::event_id *
 *********************************/
//DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(datatools::event::event_id)
BOOST_CLASS_EXPORT_IMPLEMENT(datatools::event::event_id) 


/*********************************
 * datatools::event::basic_event *
 *********************************/
//DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(datatools::event::basic_event)
BOOST_CLASS_EXPORT_IMPLEMENT(datatools::event::basic_event) 


/****************************
 * datatools::test::data_t *
 ****************************/
//DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(datatools::test::data_t)
BOOST_CLASS_EXPORT_IMPLEMENT(datatools::test::data_t) 


/********************************
 * datatools::test::more_data_t *
 ********************************/
//DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(datatools::test::more_data_t)
BOOST_CLASS_EXPORT_IMPLEMENT(datatools::test::more_data_t) 


#endif // __datatools__the_serializable_h__

/* end of datatools::the_serializable.h */
