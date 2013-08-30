// -*- mode: c++ ; -*-
/* i_multi_cut.cc
 */

#include <cuts/i_multi_cut.h>

#include <stdexcept>
#include <sstream>

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

  CUT_CONSTRUCTOR_IMPLEMENT_HEAD(i_multi_cut,logging_priority_)
  {
    return;
  }

  CUT_DEFAULT_DESTRUCTOR_IMPLEMENT(i_multi_cut)

  CUT_INITIALIZE_IMPLEMENT_HEAD(i_multi_cut,
                                a_configuration,
                                a_service_manager,
                                a_cut_dict)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Multi-cut '" << get_name() << "' is already initialized ! ");
    _common_initialize(a_configuration);
    _install_cuts(a_configuration,a_cut_dict);
    _set_initialized(true);
    return;
  }

  CUT_RESET_IMPLEMENT_HEAD(i_multi_cut)
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

// end of i_multi_cut.cc
