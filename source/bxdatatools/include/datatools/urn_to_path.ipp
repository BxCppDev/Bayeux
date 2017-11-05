//! \file datatools/urn_to_path.ipp
#ifndef DATATOOLS_URN_TO_PATH_IPP
#define DATATOOLS_URN_TO_PATH_IPP

// Ourselves:
#include <datatools/urn_to_path.h>

// Third Party:
// - Boost:
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/vector.hpp>

namespace datatools {

  /// Boost serialization template method
  template<class Archive>
  void urn_to_path::serialize(Archive & archive_, const unsigned int /* version_ */)
  {
    archive_ & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
    archive_ & boost::serialization::make_nvp("urn",      _urn_);
    archive_ & boost::serialization::make_nvp("category", _category_);
    archive_ & boost::serialization::make_nvp("mime",     _mime_);
    archive_ & boost::serialization::make_nvp("paths",    _paths_);
    return;
  }

} // end of namespace datatools

#endif // DATATOOLS_URN_TO_PATH_IPP

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
