// -*- mode: c++ ; -*-
/* utils_cut.cc
 */

#include <stdexcept>
#include <sstream>

#include <boost/algorithm/string.hpp>

#include <datatools/properties.h>
#include <datatools/ioutils.h>
#include <datatools/things.h>
#include <datatools/exception.h>

#include <dpp/utils_cut.h>

namespace dpp {

  // Registration instantiation macro :
  CUT_REGISTRATION_IMPLEMENT(utils_cut, "dpp::utils_cut");

  void utils_cut::set_mode (int mode_)
  {
    DT_THROW_IF(mode_ != MODE_HAS_BANK,
                std::domain_error,
                "Invalid mode ID " << mode_ << " !");
    _mode_ = mode_;
    return;
  }

  void utils_cut::reset_mode ()
  {
    _mode_ = MODE_UNDEFINED;
  }

  int utils_cut::get_mode () const
  {
    return _mode_;
  }

  bool utils_cut::is_mode_has_bank () const
  {
    return _mode_ == MODE_HAS_BANK;
  }

  void utils_cut::set_has_bank_name (const std::string & bank_name_)
  {
    _bank_names_.push_back (bank_name_);
    return;
  }

  const std::string & utils_cut::get_has_bank_name () const
  {
    return _bank_names_.front ();
  }

  void utils_cut::set_has_bank_type (const std::string & bank_type_)
  {
    _bank_types_.push_back (bank_type_);
    return;
  }

  const std::string & utils_cut::get_has_bank_type () const
  {
    return _bank_types_.front ();
  }

  void utils_cut::set_has_bank (const std::string & bank_name_,
                                const std::string & bank_type_)
  {
    set_has_bank_name (bank_name_);
    set_has_bank_type (bank_type_);
    return;
  }

  CUT_CONSTRUCTOR_IMPLEMENT_HEAD(utils_cut,a_logger_priority)
  {
    _mode_ = MODE_UNDEFINED;
    return;
  }

  CUT_DEFAULT_DESTRUCTOR_IMPLEMENT(utils_cut)

  CUT_INITIALIZE_IMPLEMENT_HEAD(utils_cut,
                                configuration_,
                                service_manager_,
                                cut_dict_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Utils cut named '" << (has_name()?get_name():"?" )
                << "' is already initialized !");

    this->i_cut::_common_initialize (configuration_);

    if (_mode_ == MODE_UNDEFINED)
      {
        DT_THROW_IF(! configuration_.has_key ("mode"),
                    std::logic_error,
                    "Utils cut named '" << (has_name()?get_name():"?" )
                    << "' : missing 'mode' property !");
        const std::string mode_str = configuration_.fetch_string ("mode");

        if (mode_str == "has_bank") {
            this->set_mode (MODE_HAS_BANK);
        } else {
          DT_THROW_IF(true,
                      std::domain_error,
                      "Utils cut named '" << (has_name()?get_name():"?" )
                      << "' has invalid mode '" << mode_str << "' ! ");
        }
        if (is_mode_has_bank ()) {
          if (configuration_.has_key ("has_bank.name")) {
            set_has_bank_name (configuration_.fetch_string ("has_bank.name"));
            if (configuration_.has_key ("has_bank.type")) {
              set_has_bank_type (configuration_.fetch_string ("has_bank.type"));
            }
          } else if (configuration_.has_key ("has_bank.descriptions")) {
            std::vector<std::string> descriptions;
            configuration_.fetch ("has_bank.descriptions", descriptions);
            for (std::vector<std::string>::const_iterator i = descriptions.begin ();
                 i != descriptions.end (); ++i) {
              const std::string & a_description = *i;
              if (!a_description.empty ()) {
                std::vector<std::string> tokens;
                boost::split (tokens, a_description, boost::is_any_of ("@"));
                if (tokens.size () == 1) {
                  _bank_names_.push_back (tokens.front ());
                  _bank_types_.push_back ("");
                } else if (tokens.size () == 2) {
                  _bank_names_.push_back (tokens.front ());
                  _bank_types_.push_back (tokens.back ());
                } else {
                  DT_THROW_IF(true,
                              std::logic_error,
                              "Utils cut named '" << (has_name()?get_name():"?" )
                              << "' : bank descriptions must only have bank name and/or bank type");
                }
              }
            }
          } else {
            DT_THROW_IF(true,
                        std::logic_error,
                        "Utils cut named '" << (has_name()?get_name():"?" )
                        << "' has missing 'has_bank.name' or 'has_bank.descriptions' property !");
          }
        }
      }

    _set_initialized (true);
    return;
  }

  CUT_RESET_IMPLEMENT_HEAD (utils_cut)
  {
    _set_initialized (false);
    _mode_ = MODE_UNDEFINED;
    _bank_names_.clear ();
    _bank_types_.clear ();
    this->i_cut::_reset ();
    return;
  }

  CUT_ACCEPT_IMPLEMENT_HEAD (utils_cut)
  {
    const datatools::things & ER = get_user_data<datatools::things>();
    // Check if the event record has a bank with a specific
    // name (and optionally a serial tag):
    if (is_mode_has_bank ()) {
      DT_LOG_DEBUG(_logging, "Utils cut named '" << (has_name()?get_name():"?" ) << "': running HAS_BANK mode...");
      bool check = false;

      for (size_t i = 0; i != _bank_names_.size (); ++i) {
        if (! _bank_types_.empty ()) {
          check = ER.has_serial_tag(_bank_names_.at (i),
                                    _bank_types_.at (i));
        } else {
          check = ER.has(_bank_names_.at (i));
        }
          if (!check) break;
      }
      if (! check) {
        return cuts::SELECTION_REJECTED;
      }
      return cuts::SELECTION_REJECTED;
    } else {
      DT_THROW_IF(true, std::domain_error, "Utils cut named '" << (has_name()?get_name():"?" ) << "': invalid mode !");
    }
    return cuts::SELECTION_INAPPLICABLE;
  }

  void utils_cut::tree_dump (std::ostream & a_out ,
                             const std::string & a_title,
                             const std::string & a_indent,
                             bool a_inherit) const
  {
    i_cut::tree_dump(a_out, a_title, a_indent, true);
    a_out << a_indent << datatools::i_tree_dumpable::tag
          << "Mode : " << _mode_ << std::endl;
    a_out << a_indent << datatools::i_tree_dumpable::tag
          << "Bank names : " << _bank_names_.size() << std::endl;
    a_out << a_indent << datatools::i_tree_dumpable::inherit_tag (a_inherit)
          << "Bank types : " << _bank_types_.size() << std::endl;
    return;
  }

}  // end of namespace dpp

// end of utils_cut.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** End: --
*/
