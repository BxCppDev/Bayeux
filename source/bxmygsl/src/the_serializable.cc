// -*- mode: c++; -*-
/* the_serializable.cc
 */

// Third party:
// - Boost:
#include <boost/serialization/export.hpp>
// - Bayeux/datatools:
#include <datatools/archives_instantiation.h>

// This project:
/*********************
 * mygsl::best_value *
 *********************/
#include <mygsl/best_value.ipp>

/********************
 * mygsl::datapoint *
 ********************/
#include <mygsl/datapoint.ipp>

/****************************
 * mygsl::linear_regression *
 ****************************/
#include <mygsl/linear_regression.ipp>

/*******************
 * mygsl::interval *
 *******************/
#include <mygsl/interval.ipp>

/*********************
 * mygsl::polynomial *
 *********************/
#include <mygsl/polynomial.ipp>

/********************
 * mygsl::histogram *
 ********************/
#include <mygsl/histogram.ipp>

/********************
 * mygsl::histogram *
 ********************/
#include <mygsl/histogram.ipp>

/***********************
 * mygsl::histogram_2d *
 ***********************/
#include <mygsl/histogram_2d.ipp>

/*************************
 * mygsl::histogram_pool *
 *************************/
#include <mygsl/histogram_pool.ipp>

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


