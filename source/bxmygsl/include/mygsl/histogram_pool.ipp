/// \file mygsl/histogram_pool.ipp

#ifndef MYGSL_HISTOGRAM_POOL_IPP
#define MYGSL_HISTOGRAM_POOL_IPP 1

// Ourselves:
#include <mygsl/histogram_pool.h>

// Standard library:
#include <string>

// Third party:
// - Boost:
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/map.hpp>
// - Bayeux/datatools :
#include <datatools/i_serializable.ipp>
#include <datatools/utils.h>
#include <datatools/properties.ipp>

// This project:
#include <mygsl/histogram.ipp>
#include <mygsl/histogram_2d.ipp>
#include <mygsl/histogram_utils.h>

namespace mygsl {

  template<class Archive>
  void histogram_pool::histogram_entry_type::serialize(Archive & ar,
                                                       const unsigned int /*version*/)
  {
    ar & boost::serialization::make_nvp("name", name);
    ar & boost::serialization::make_nvp("title", title);
    ar & boost::serialization::make_nvp("dimension", dimension);
    if (dimension == histogram_pool::HISTOGRAM_DIM_1D) {
      ar & boost::serialization::make_nvp("hh1d", hh1d);
    }
    if (dimension == histogram_pool::HISTOGRAM_DIM_2D) {
      ar & boost::serialization::make_nvp("hh2d", hh2d);
    }
    ar & boost::serialization::make_nvp("group", group);
    //ar & boost::serialization::make_nvp ("auxiliaries", auxiliaries);
    return;
  }

  template<class Archive>
  void histogram_pool::serialize(Archive & ar,
                                 const unsigned int /*version*/)
  {
    ar & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
    ar & boost::serialization::make_nvp("description", _description_);
    ar & boost::serialization::make_nvp("dict", _dict_);
    ar & boost::serialization::make_nvp("auxiliaries", _auxiliaries_);
    return;
  }

} // end of namespace mygsl

#endif // MYGSL_HISTOGRAM_POOL_IPP

/* Local Variables: */
/* mode: c++        */
/* coding: utf-8    */
/* End:             */
