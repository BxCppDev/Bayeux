// library_query_service.cc

// Ourselves:
#include <datatools/library_query_service.h>

namespace datatools {

  /** Auto-registration of this service class in a central service Db */
  DATATOOLS_SERVICE_REGISTRATION_IMPLEMENT(library_query_service, "datatools::library_query_service")

  library_query_service::library_query_service()
  {
    return;
  }

  library_query_service::~library_query_service()
  {
    if (this->is_initialized()) this->library_query_service::reset();
    return;
  }

  bool library_query_service::is_initialized() const
  {
    return _initialized_;
  }

  int library_query_service::initialize(const datatools::properties & config_,
                                       datatools::service_dict_type& /*services_*/)
  {
    DT_THROW_IF(is_initialized(), std::logic_error,
                "Service is already initialized!");
    base_service::common_initialize(config_);
    _init_();
    _initialized_ = true;
    return datatools::SUCCESS;
  }

  int library_query_service::reset()
  {
    DT_THROW_IF(!is_initialized(), std::logic_error,
                "Service is not initialized!");
    _initialized_ = false;
    _reset_();
    return datatools::SUCCESS;
  }

  void library_query_service::tree_dump(std::ostream& out_,
                                       const std::string& title_,
                                       const std::string& indent_,
                                       bool inherit_) const
  {
    this->base_service::tree_dump(out_, title_, indent_, true);

    out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_)
         << "Library information dictionary : " << std::endl;
    std::ostringstream indent2;
    indent2 << indent_ << i_tree_dumpable::inherit_skip_tag(inherit_);
    _libinfo_.tree_dump(out_, "", indent2.str());

    return;
  }

  void library_query_service::_init_()
  {
    return;
  }

  void library_query_service::_reset_()
  {
    if (_libinfo_.is_initialized()) {
      _libinfo_.reset();
    }
    return;
  }

  library_info & library_query_service::grab_libinfo()
  {
    return _libinfo_;
  }

  const library_info & library_query_service::get_libinfo() const
  {
    return _libinfo_;
  }

} // end of namespace datatools
