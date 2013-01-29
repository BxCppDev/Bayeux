// -*- mode: c++ ; -*- 
/* mygsl/histogram.ipp */

#ifndef __mygsl__histogram_ipp
#define __mygsl__histogram_ipp 1

#include <mygsl/histogram.h>
#include <string>

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>

#include <datatools/i_serializable.ipp>
#include <datatools/utils.h>
#include <datatools/properties.ipp>

namespace mygsl {
  
  template<class Archive>
  void histogram::serialize (Archive & ar,                
                             const unsigned int version) 
  {
    ar & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
    ar & boost::serialization::make_nvp ("binning_info", _binning_info_);
    ar & boost::serialization::make_nvp ("counts",      _counts_);
    ar & boost::serialization::make_nvp ("underflow",   _underflow_);
    ar & boost::serialization::make_nvp ("overflow",    _overflow_);
    
    if (Archive::is_saving::value)
      {
        uint32_t nbins = _h_->n;
        ar & boost::serialization::make_nvp ("nbins", nbins);
      }
    if  (Archive::is_loading::value)
      { 
        if (_h_ != 0) 
          {
            gsl_histogram_free (_h_);
            _h_ = 0;
          }
        uint32_t nbins = 0;
        ar & boost::serialization::make_nvp ("nbins", nbins);
        _h_ = gsl_histogram_alloc (nbins);
      }
    if (_h_->n > 0)
      {
        for (int ii = 0 ; ii < _h_->n + 1; ii ++ )
          {
            std::string tag = "range_" + boost::lexical_cast<std::string>(ii) ;
            ar & boost::serialization::make_nvp(tag.c_str(), _h_->range[ii]) ; 
          }
        for (int ii = 0 ; ii < _h_->n; ii ++ )
          {
            std::string tag = "bin_" + boost::lexical_cast<std::string>(ii) ;
            ar & boost::serialization::make_nvp(tag.c_str(), _h_->bin[ii]) ; 
          }
      }
    ar & boost::serialization::make_nvp ("auxiliaries", _auxiliaries_);
    return;
  }
     
} // end of namespace mygsl


#endif // __mygsl__histogram_ipp

// end of mygsl/histogram.ipp
