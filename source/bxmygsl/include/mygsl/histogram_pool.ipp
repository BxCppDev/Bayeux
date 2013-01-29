// -*- mode: c++ ; -*- 
/* mygsl/histogram_pool.ipp */

#ifndef __mygsl__histogram_pool_ipp
#define __mygsl__histogram_pool_ipp 1

#include <mygsl/histogram_pool.h>
#include <string>

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/map.hpp>

#include <datatools/i_serializable.ipp>
#include <datatools/utils.h>
#include <datatools/properties.ipp>
#include <mygsl/histogram.ipp>
#include <mygsl/histogram_2d.ipp>
#include <mygsl/histogram_utils.h>

namespace mygsl {
 
  template<class Archive>
  void histogram_pool::histogram_entry_type::serialize (Archive & ar,                
                                                        const unsigned int version) 
  {
    ar & boost::serialization::make_nvp ("name", name);
    ar & boost::serialization::make_nvp ("title", title);
    ar & boost::serialization::make_nvp ("dimension", dimension);
    if (dimension == histogram_pool::HISTOGRAM_DIM_1D) 
      {
        ar & boost::serialization::make_nvp ("hh1d", hh1d);
      }
    if (dimension == histogram_pool::HISTOGRAM_DIM_2D) 
      {
        ar & boost::serialization::make_nvp ("hh2d", hh2d);
      }
    ar & boost::serialization::make_nvp ("group", group);
    //ar & boost::serialization::make_nvp ("auxiliaries", auxiliaries);
    return;
  }
 
  template<class Archive>
  void histogram_pool::serialize (Archive & ar,                
                                  const unsigned int version) 
  {
    ar & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
    ar & boost::serialization::make_nvp ("description", _description_);
    ar & boost::serialization::make_nvp ("dict", _dict_);
    ar & boost::serialization::make_nvp ("auxiliaries", _auxiliaries_);
    return;
  }
     
} // end of namespace mygsl


#endif // __mygsl__histogram_pool_ipp

// end of mygsl/histogram_pool.ipp
