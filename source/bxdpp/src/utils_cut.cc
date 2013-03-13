// -*- mode: c++ ; -*-
/* utils_cut.cc
 */

#include <stdexcept>
#include <sstream>

#include <boost/algorithm/string.hpp>

#include <datatools/properties.h>
#include <datatools/ioutils.h>
#include <datatools/things.h>

#include <dpp/utils_cut.h>

namespace dpp {

  void utils_cut::set_mode (int mode_)
  {
    if (mode_ == MODE_HAS_BANK)
      {
      }
    else
      {
        std::ostringstream message;
        message << "dpp::utils_cut::set_mode: "
                << "Invalid mode ID " << mode_ << " ! ";
        throw std::logic_error (message.str ());
      }
    _mode_ = mode_;
    return;
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

  // Registration instantiation macro :
  CUT_REGISTRATION_IMPLEMENT(utils_cut, "dpp::utils_cut");

  // ctor:
  utils_cut::utils_cut (int debug_level_) :
    base_cut ("dpp::utils_cut",
              "A general utility event record cut",
              "0.1",
              debug_level_)
  {
    _mode_ = MODE_UNDEFINED;
    return;
  }

  // dtor:
  CUT_DEFAULT_DESTRUCTOR_IMPLEMENT(utils_cut)

  CUT_RESET_IMPLEMENT_HEAD (utils_cut)
  {
    _mode_ = MODE_UNDEFINED;

    _bank_names_.clear ();
    _bank_types_.clear ();

    this->base_cut::reset ();

    _set_initialized (false);
    return;
  }

  CUT_INITIALIZE_IMPLEMENT_HEAD(utils_cut,
                                configuration_,
                                service_manager_,
                                cut_dict_)
  {
    if (is_initialized ())
      {
        std::ostringstream message;
        message << "dpp::utils_cuts::initialize: "
                << "Cut '" << get_name () << "' is already initialized ! ";
        throw std::logic_error (message.str ());
      }

    if (_mode_ == MODE_UNDEFINED)
      {
        if (! configuration_.has_key ("mode"))
          {
            std::ostringstream message;
            message << "dpp::utils_cuts::initialize: "
                    << "Missing 'mode' property !";
            throw std::logic_error (message.str ());
          }
        const std::string mode_str = configuration_.fetch_string ("mode");

        if (mode_str == "has_bank")
          {
            this->set_mode (MODE_HAS_BANK);
          }
        else
          {
            std::ostringstream message;
            message << "dpp::utils_cut::initialize: "
                    << "Invalid mode '" << mode_str << "' ! ";
            throw std::logic_error (message.str ());
          }

        if (is_mode_has_bank ())
          {
            if (configuration_.has_key ("has_bank.name"))
              {
                set_has_bank_name (configuration_.fetch_string ("has_bank.name"));
                if (configuration_.has_key ("has_bank.type"))
                  {
                    set_has_bank_type (configuration_.fetch_string ("has_bank.type"));
                  }
              }
            else if (configuration_.has_key ("has_bank.descriptions"))
              {
                std::vector<std::string> descriptions;
                configuration_.fetch ("has_bank.descriptions", descriptions);
                for (std::vector<std::string>::const_iterator i = descriptions.begin ();
                     i != descriptions.end (); ++i)
                  {
                    const std::string & a_description = *i;
                    if (!a_description.empty ())
                      {
                        std::vector<std::string> tokens;
                        boost::split (tokens, a_description, boost::is_any_of ("@"));
                        if (tokens.size () == 1)
                          {
                            _bank_names_.push_back (tokens.front ());
                            _bank_types_.push_back ("");
                          }
                        else if (tokens.size () == 2)
                          {
                            _bank_names_.push_back (tokens.front ());
                            _bank_types_.push_back (tokens.back ());
                          }
                        else
                          {
                            std::ostringstream message;
                            message << "dpp::utils_cut::initialize: "
                                    << "Bank descriptions must only have bank name and/or bank type";
                            throw std::logic_error (message.str ());
                          }
                      }
                  }
              }
            else
              {
                std::ostringstream message;
                message << "dpp::utils_cut::initialize: "
                        << "Missing 'has_bank.name' or 'has_bank.descriptions' property !";
                throw std::logic_error (message.str ());
              }
          }

      }

    _set_initialized (true);
    return;
  }

  CUT_ACCEPT_IMPLEMENT_HEAD (utils_cut)
  {
    const datatools::things & ER = get_event_record ();

    // Check if the event record has a bank with a specific
    // name (and optionally a serial tag):
    if (is_mode_has_bank ())
      {
        if (is_debug ())
          {
            std::clog << datatools::io::debug
                      << "dpp:utils_cut::_accept: "
                      << "Running HAS_BANK mode..."
                      << std::endl;
          }
        bool check = false;

        for (size_t i = 0; i != _bank_names_.size (); ++i)
          {
            if (! _bank_types_.empty ())
              {
                check = ER.has_serial_tag (_bank_names_.at (i),
                                           _bank_types_.at (i));
              }
            else
              {
                check = ER.has (_bank_names_.at (i));
              }
            if (!check) break;
          }
        if (! check)
          {
            return cuts::i_cut::REJECTED;
          }
        return cuts::i_cut::ACCEPTED;
      }
    else
      {
        std::clog << datatools::io::warning
                  << "dpp:utils_cut::_accept: "
                  << "Unknown mode !"
                  << std::endl;
      }

    return cuts::i_cut::INAPPLICABLE;
  }

}  // end of namespace dpp

// end of utils_cut.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** End: --
*/
