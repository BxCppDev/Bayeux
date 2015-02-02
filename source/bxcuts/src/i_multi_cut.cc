// i_multi_cut.cc

// Ourselves:
#include <cuts/i_multi_cut.h>

// Standard library:
#include <stdexcept>
#include <sstream>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/exception.h>

namespace cuts {

  void i_multi_cut::add_cut(cut_handle_type & a_cut_handle)
  {
    DT_THROW_IF(&a_cut_handle.grab() == this,
                std::logic_error,
                "Self-referenced multi-cut is not allowed !");
    for (cuts_col_type::iterator i = _cuts.begin();
         i != _cuts.end();
         i++) {
      cut_handle_type & ch = *i;
      DT_THROW_IF(&ch.grab() == &a_cut_handle.grab(),
                  std::logic_error,
                  "Cut is already registered in multi-cut !");
    }
    _cuts.push_back(a_cut_handle);
    return;
  }

  void i_multi_cut::_at_set_user_data()
  {
    DT_LOG_TRACE(_logging, "Entering: Multi-cut '" << (has_name()?get_name():"?") << "'...");
    DT_THROW_IF(_cuts.size() == 0,
                std::logic_error,
                "Missing cuts !");
    for (cuts_col_type::iterator i = _cuts.begin();
         i != _cuts.end();
         i++) {
      cut_handle_type & ch = *i;
      DT_LOG_TRACE(_logging,
                   "Export user data to cut named '" << (ch.get().has_name()?ch.get().get_name():"?") << "'");
      _export_user_data_to(ch.grab());
    }
    DT_LOG_TRACE(_logging, "Exiting.");
  }

  void i_multi_cut::_at_reset_user_data()
  {
    DT_LOG_TRACE(_logging, "Entering...");
    for (cuts_col_type::iterator i = _cuts.begin();
         i !=  _cuts.end();
         i++) {
      cut_handle_type & ch = *i;
      ch.grab().reset_user_data();
    }
    DT_LOG_TRACE(_logging, "Exiting.");
  }

  i_multi_cut::i_multi_cut(datatools::logger::priority a_logger_priority)
    : i_cut(a_logger_priority)
  {
    return;
  }

  i_multi_cut::~i_multi_cut()
  {
    if (is_initialized()) {
      this->i_multi_cut::reset();
    }
    return;
  }

  void i_multi_cut::initialize(const datatools::properties & a_configuration,
                               datatools::service_manager & /*a_service_manager*/,
                               cuts::cut_handle_dict_type & a_cut_dict)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Multi-cut '" << get_name() << "' is already initialized ! ");
    _common_initialize(a_configuration);
    _install_cuts(a_configuration,a_cut_dict);
    _set_initialized(true);
    return;
  }

  void i_multi_cut::export_to_config(datatools::properties & config_,
                                 uint32_t flags_,
                                 const std::string & prefix_) const
  {
    this->cuts::i_cut::export_to_config(config_,flags_, prefix_);
    std::vector<std::string> cuts_names;
    for (cuts_col_type::const_iterator i = _cuts.begin();
         i != _cuts.end();
         i++) {
      cuts_names.push_back(i->get().get_name());
    }
    config_.store(prefix_ + "cuts", cuts_names, "Cuts to be combined");
    return;
  }

  void i_multi_cut::reset()
  {
    _set_initialized(false);
    _reset_cuts();
    return;
  }

  void i_multi_cut::_install_cuts(const datatools::properties & a_configuration,
                                  cuts::cut_handle_dict_type & a_cut_dict)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Multi-cut '" << get_name() << "' is already initialized !");
    if (_cuts.size() == 0) {
      std::vector<std::string> cut_names;
      DT_THROW_IF(! a_configuration.has_key("cuts"),
                  std::logic_error,
                  "Missing 'cuts' name property !");
      a_configuration.fetch("cuts", cut_names);
      for (size_t i = 0; i < cut_names.size(); i++) {
        const std::string & cut_name = cut_names[i];
        cut_handle_dict_type::iterator found = a_cut_dict.find(cut_name);
        DT_THROW_IF(found == a_cut_dict.end(),
                    std::domain_error,
                    "Can't find any cut named '" << cut_name
                    << "' from the external dictionnary !");
        this->add_cut(found->second.grab_initialized_cut_handle());
      }
    }
    return;
  }

  void i_multi_cut::_reset_cuts()
  {
    _cuts.clear();
    this->i_cut::_reset();
    return;
  }

} // end of namespace cuts
