//! \file datatools/urn.ipp
#ifndef DATATOOLS_URN_IPP
#define DATATOOLS_URN_IPP

// Ourselves:
#include <datatools/urn.h>

// Third Party:
// - Boost:
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/vector.hpp>

namespace datatools {

  /// Boost serialization template method
  template<class Archive>
  void urn::serialize(Archive & archive_, const unsigned int /* version_ */)
  {
    archive_ & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
    archive_ & boost::serialization::make_nvp("segments", _segments_);
    return;
  }

} // end of namespace datatools

#endif // DATATOOLS_URN_IPP

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
