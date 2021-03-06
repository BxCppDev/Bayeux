// i_binary_cut.cc

// Ourselves:
#include <cuts/i_binary_cut.h>

// Standard library:
#include <stdexcept>
#include <sstream>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/exception.h>

namespace cuts {

  void i_binary_cut::set_cut_1(cut_handle_type & a_cut_handle)
  {
    DT_THROW_IF(&a_cut_handle.grab() == this,
                std::logic_error,
                "Self-referenced binary cut is not allowed !");
    _handle_1 = a_cut_handle;
    return;
  }

  void i_binary_cut::set_cut_2(cut_handle_type & a_cut_handle)
  {
    DT_THROW_IF(&a_cut_handle.grab() == this,
                std::logic_error,
                "Self-referenced binary cut is not allowed !");
    _handle_2 = a_cut_handle;
    return;
  }

  void i_binary_cut::set_cuts(cut_handle_type & a_cut_handle_1,
                              cut_handle_type & a_cut_handle_2)
  {
    set_cut_1(a_cut_handle_1);
    set_cut_2(a_cut_handle_2);
    return;
  }

  void i_binary_cut::_at_set_user_data()
  {
    DT_LOG_TRACE(_logging, "Entering...");
    _export_user_data_to(_handle_1.grab());
    _export_user_data_to(_handle_2.grab());
    DT_LOG_TRACE(_logging, "Exiting.");
  }

  void i_binary_cut::_at_reset_user_data()
  {
    DT_LOG_TRACE(_logging, "Entering...");
    _handle_1.grab().reset_user_data();
    _handle_2.grab().reset_user_data();
    DT_LOG_TRACE(_logging, "Exiting.");
  }

  i_binary_cut::i_binary_cut(datatools::logger::priority a_logger_priority)
    : i_cut(a_logger_priority)
  {
    return;
  }


  // dtor:
  i_binary_cut::~i_binary_cut()
  {
    if (is_initialized()) {
      this->i_binary_cut::reset();
    }
    return;
  }

  void i_binary_cut::initialize(const datatools::properties & a_configuration,
                                datatools::service_manager & /*a_service_manager*/,
                                cuts::cut_handle_dict_type & a_cut_dict)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Binary cut '" << get_name() << "' is already initialized ! ");
    _common_initialize(a_configuration);
    _install_cuts(a_configuration,a_cut_dict);
    _set_initialized(true);
    return;
  }

  void i_binary_cut::export_to_config(datatools::properties & config_,
                                      uint32_t flags_,
                                      const std::string & prefix_) const
  {
    this->cuts::i_cut::export_to_config(config_,flags_, prefix_);
    config_.store_string(prefix_ + "cut1", _handle_1.get().get_name(), "First cut to be combined");
    config_.store_string(prefix_ + "cut2", _handle_2.get().get_name(), "Second cut to be combined");

    return;
  }


  void i_binary_cut::reset()
  {
    _set_initialized(false);
    _reset_cuts();
    return;
  }

  void i_binary_cut::_install_cuts(const datatools::properties & a_configuration,
                                   cuts::cut_handle_dict_type & a_cut_dict)
  {
    DT_LOG_TRACE(_logging,"Entering...");
    if (! _handle_1) {
      std::string cut_name;
      DT_THROW_IF(!a_configuration.has_key("cut_1"),
                  std::logic_error,
                  "Missing ``cut_1`` name property !");
      cut_name = a_configuration.fetch_string("cut_1");

      cut_handle_dict_type::iterator found = a_cut_dict.find(cut_name);
      DT_THROW_IF(found == a_cut_dict.end(),
                  std::logic_error,
                  "Can't find any cut named '" << cut_name
                  << "' from the external dictionnary ! ");
      this->set_cut_1(found->second.grab_initialized_cut_handle());
    }

    if (! _handle_2) {
      std::string cut_name;
      DT_THROW_IF(!a_configuration.has_key("cut_2"),
                  std::logic_error,
                  "Missing ``cut_2`` name property !");
      cut_name = a_configuration.fetch_string("cut_2");

      cut_handle_dict_type::iterator found = a_cut_dict.find(cut_name);
      DT_THROW_IF(found == a_cut_dict.end(),
                  std::logic_error,
                  "Can't find any cut named '" << cut_name
                  << "' from the external dictionnary ! ");
      this->set_cut_2(found->second.grab_initialized_cut_handle());
    }

    DT_THROW_IF(! _handle_1,
                std::logic_error,
                "No 'cut 1' !");
    DT_THROW_IF(! _handle_2,
                std::logic_error,
                "No 'cut 2' !");
    DT_LOG_TRACE(_logging,"Exiting.");
    return;
  }

  void i_binary_cut::_reset_cuts()
  {
    _handle_1.reset();
    _handle_2.reset();
    this->i_cut::_reset();
    return;
  }

  void i_binary_cut::tree_dump (std::ostream & a_out ,
                         const std::string & a_title,
                         const std::string & a_indent,
                         bool a_inherit) const
  {
    i_cut::tree_dump(a_out, a_title, a_indent, true);
    a_out << a_indent << datatools::i_tree_dumpable::tag
          << "Cut #1 : '" << (_handle_1 ?_handle_1.get().get_name() : "?") << "'" << std::endl;
    a_out << a_indent << datatools::i_tree_dumpable::inherit_tag (a_inherit)
          << "Cut #2 : '" << (_handle_2 ?_handle_2.get().get_name() : "?") << "'" << std::endl;
    return;
  }

} // end of namespace cuts
