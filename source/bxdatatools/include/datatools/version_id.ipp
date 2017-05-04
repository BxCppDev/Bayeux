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
  void version_id::serialize(Archive & archive,
                             const unsigned int /* version */)
  {
    archive & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
    archive & boost::serialization::make_nvp("major", major_);
    archive & boost::serialization::make_nvp("minor", minor_);
    archive & boost::serialization::make_nvp("revision", revision_);
    archive & boost::serialization::make_nvp("tag", tag_);
    archive & boost::serialization::make_nvp("tag_number", tag_number_);
  }

} // end of namespace datatools

#endif // DATATOOLS_VERSION_ID_IPP

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
