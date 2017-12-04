//! \file datatools/enriched_base.ipp
#ifndef DATATOOLS_ENRICHED_BASE_IPP
#define DATATOOLS_ENRICHED_BASE_IPP

// Ourselves:
#include <datatools/enriched_base.h>

// Third Party:
// - Boost:
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/string.hpp>
// #include <boost/serialization/shared_ptr.hpp>

// This project:
#include <datatools/utils.h>
#include <datatools/properties.ipp>

namespace datatools {

  template<class Archive>
  void enriched_base::serialize (Archive & archive_,
                                 const unsigned int /*version_*/)
  {
    archive_ & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
    archive_ & boost::serialization::make_nvp("name",              _name_);
    archive_ & boost::serialization::make_nvp("display_name",      _display_name_);
    archive_ & boost::serialization::make_nvp("terse_description", _terse_description_);
    archive_ & boost::serialization::make_nvp("logging_priority",  _logging_priority_);
    archive_ & boost::serialization::make_nvp("auxiliaries",       _auxiliaries_);
    return;
  }

} // end of namespace datatools

#endif // DATATOOLS_ENRICHED_BASE_IPP

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
