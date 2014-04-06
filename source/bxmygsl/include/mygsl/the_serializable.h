/** \file mygsl/the_serializable.h */

#ifndef MYGSL_THE_SERIALIZABLE_H
#define MYGSL_THE_SERIALIZABLE_H 1

// Third party:
// - Boost:
#include <boost/serialization/export.hpp>
// - Bayeux/datatools:
#include <datatools/archives_instantiation.h>

// This project:
#include <mygsl/the_serializable.ipp>

/*********************
 * mygsl::best_value *
 *********************/
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(mygsl::best_value)
BOOST_CLASS_EXPORT_IMPLEMENT(mygsl::best_value)

/********************
 * mygsl::datapoint *
 ********************/
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(mygsl::datapoint)
BOOST_CLASS_EXPORT_IMPLEMENT(mygsl::datapoint)

/**************************************
 * mygsl::linear_regression::fit_data *
 **************************************/
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(mygsl::linear_regression::fit_data)
BOOST_CLASS_EXPORT_IMPLEMENT(mygsl::linear_regression::fit_data)

/********************
 * mygsl::interval *
 ********************/
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(mygsl::interval)
BOOST_CLASS_EXPORT_IMPLEMENT(mygsl::interval)

/********************
 * mygsl::polynomial *
 ********************/
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(mygsl::polynomial)
BOOST_CLASS_EXPORT_IMPLEMENT(mygsl::polynomial)

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

#endif // MYGSL_THE_SERIALIZABLE_H_

/* end of mygsl/the_serializable.h */
