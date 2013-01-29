// -*- mode: c++; -*- 
/* mygsl::the_serializable.h */

#ifndef __mygsl__the_serializable_h__
#define __mygsl__the_serializable_h__ 1

#include <datatools/archives_instantiation.h>
#include <boost/serialization/export.hpp>
#include <mygsl/the_serializable.ipp>


/********************
 * mygsl::histogram *
 ********************/
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(mygsl::histogram)
BOOST_CLASS_EXPORT_IMPLEMENT(mygsl::histogram) 

/***********************
 * mygsl::histogram_2d *
 ***********************/
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(mygsl::histogram_2d)
BOOST_CLASS_EXPORT_IMPLEMENT(mygsl::histogram_2d) 

/*************************
 * mygsl::histogram_pool *
 *************************/
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(mygsl::histogram_pool)
BOOST_CLASS_EXPORT_IMPLEMENT(mygsl::histogram_pool) 


/* Example for a class that is not exported but 
 * its serialization code is instantiated:
 * Class: mygsl::instantiated_serializable_data 
 * DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(mygsl::instantiated_serializable_data)
 */

#endif // __mygsl__the_serializable_h__

/* end of mygsl::the_serializable.h */
