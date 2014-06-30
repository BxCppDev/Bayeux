// -*- mode: c++; -*-
//! \file geomtools/i_placement.ipp

#ifndef GEOMTOOLS_I_PLACEMENT_IPP
#define GEOMTOOLS_I_PLACEMENT_IPP 1

// Third party:
// - Boost:
#include <boost/archive/archive_exception.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/i_serializable.ipp>

// This project:
#include <geomtools/i_placement.h>

namespace geomtools {

  /// Boost serialization template method
  template<class Archive>
  void i_placement::serialize (Archive & ar_ , const unsigned int /*version_*/)
  {
    ar_ & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
    return;
  }

} // end of namespace geomtools

#endif // GEOMTOOLS_I_PLACEMENT_IPP
