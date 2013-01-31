// -*- mode: c++; -*- 
//! \file geomtools/i_placement.ipp

#ifndef GEOMTOOLS_I_PLACEMENT_IPP_
#define GEOMTOOLS_I_PLACEMENT_IPP_ 1

#include <boost/archive/archive_exception.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>

#include <datatools/utils.h>
#include <geomtools/i_placement.h>
#include <datatools/i_serializable.ipp>
         
namespace geomtools {
  
  template<class Archive>
  void i_placement::serialize (Archive & ar_ , const unsigned int version_)
  {
    ar_ & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
    return;
  }

} // end of namespace geomtools 

#endif // GEOMTOOLS_I_PLACEMENT_IPP_

