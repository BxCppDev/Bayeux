// -*- mode: c++; -*- 
//! \file geomtools/i_placement.ipp

#ifndef __geomtools__i_placement_ipp
#define __geomtools__i_placement_ipp 1

#include <boost/archive/archive_exception.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>

#include <datatools/serialization/utils.h>
#include <geomtools/i_placement.h>
#include <datatools/serialization/i_serializable.ipp>
         
namespace geomtools {
  
  template<class Archive>
  void i_placement::serialize (Archive & ar_ , const unsigned int version_)
  {
    ar_ & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
    return;
  }

} // end of namespace geomtools 

#endif // __geomtools__i_placement_ipp

