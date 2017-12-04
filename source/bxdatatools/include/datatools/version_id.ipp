//! \file datatools/version_id.ipp
#ifndef DATATOOLS_VERSION_ID_IPP
#define DATATOOLS_VERSION_ID_IPP

// Ourselves:
#include <datatools/version_id.h>

// Third Party:
// - Boost:
#include <boost/serialization/nvp.hpp>

// This project:
#include <datatools/utils.h>

namespace datatools {

  /// Boost serialization template method
  template<class Archive>
  void version_id::serialize(Archive & archive_,
                             const unsigned int /* version_ */)
  {
    archive_ & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
    archive_ & boost::serialization::make_nvp("major", _major_);
    archive_ & boost::serialization::make_nvp("minor", _minor_);
    archive_ & boost::serialization::make_nvp("revision", _revision_);
    archive_ & boost::serialization::make_nvp("tag", _tag_);
    archive_ & boost::serialization::make_nvp("tag_number", _tag_number_);
    return;
  }

} // end of namespace datatools

#endif // DATATOOLS_VERSION_ID_IPP

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
