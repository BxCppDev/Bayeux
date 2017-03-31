//! \file datatools/urn_info.ipp
#ifndef DATATOOLS_URN_INFO_IPP
#define DATATOOLS_URN_INFO_IPP

// Ourselves:
#include <datatools/urn_info.h>

// Third Party:
// - Boost:
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/vector.hpp>

namespace datatools {

  /// Boost serialization template method
  template<class Archive>
  void urn_info::serialize(Archive & archive_, const unsigned int /* version_ */)
  {
    archive_ & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
    archive_ & boost::serialization::make_nvp("locked", _locked_);
    archive_ & boost::serialization::make_nvp("component_locked", _component_locked_);
    archive_ & boost::serialization::make_nvp("urn", _urn_);
    archive_ & boost::serialization::make_nvp("category", _category_);
    archive_ & boost::serialization::make_nvp("description", _description_);
    archive_ & boost::serialization::make_nvp("components", _components_);
    archive_ & boost::serialization::make_nvp("display_valid", _display_.valid);
    if (_display_.valid) {
      archive_ & boost::serialization::make_nvp("display_x",     _display_.x);
      archive_ & boost::serialization::make_nvp("display_y",     _display_.y);
      archive_ & boost::serialization::make_nvp("display_layer", _display_.layer);
      archive_ & boost::serialization::make_nvp("display_meta", _display_.meta);
    }
  }

} // end of namespace datatools

#endif // DATATOOLS_URN_INFO_IPP

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
