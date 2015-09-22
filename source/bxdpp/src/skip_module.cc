/* skip_module.cc
 *
 * Copyright (C) 2011-2013 Francois Mauger <mauger@lpccaen.in2p3.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

// Standard library:
#include <stdexcept>
#include <sstream>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/ioutils.h>
#include <datatools/things.h>
#include <datatools/exception.h>
#include <datatools/logger.h>

// This project:
#include <dpp/skip_module.h>

namespace dpp {

  // Registration instantiation macro :
  DPP_MODULE_REGISTRATION_IMPLEMENT(skip_module, "dpp::skip_module");

  int skip_module::get_first () const
  {
    return _first_;
  }

  int skip_module::get_last () const
  {
    return _last_;
  }

  bool skip_module::is_inverted () const
  {
    return _inverted_;
  }

  void skip_module::set_first_last (int a_first,
                                    int a_last,
                                    bool a_inverted)
  {
    _first_    = a_first;
    _last_     = a_last;
    _inverted_ = a_inverted;
    return;
  }

  void skip_module::set_first_number (int a_first,
                                      int a_number,
                                      bool a_inverted)
  {
    _first_    = a_first;
    _last_     = _first_ + a_number - 1;
    _inverted_ = a_inverted;
    return;
  }

  void skip_module::_set_defaults ()
  {
    _first_   = -1;
    _last_    = -1;
    _counter_ = 0;
    _inverted_ = false;
    return;
  }

  /*** Implementation of the interface ***/

  // Constructor :
  skip_module::skip_module(datatools::logger::priority logging_priority_)
    : base_module(logging_priority_)
  {
    _set_defaults ();
    return;
  }

  // Destructor :
  skip_module::~skip_module()
  {
    if (is_initialized()) skip_module::reset();
    return;
  }

  // Initialization :
  void skip_module::initialize(const datatools::properties & a_config,
                               datatools::service_manager & /* a_service_manager */,
                               dpp::module_handle_dict_type & a_module_dict)
  {
    DT_THROW_IF(is_initialized (),
                std::logic_error,
                "Skip module '" << get_name () << "' is already initialized ! ");

    _common_initialize(a_config);

    bool inverted = false;
    int first  = _first_;
    int last   = _last_;
    int number = -1;

    if (first < 0) {
      if (a_config.has_key ("first")) {
        first = a_config.fetch_integer ("first");
      } else {
        // Default :
        first = 0;
      }
    }

    if (last < 0) {
      if (a_config.has_key ("last")) {
        last = a_config.fetch_integer ("last");
        DT_THROW_IF(last < 0,
                    std::domain_error,
                    "Invalid 'last' index value in module named '" << get_name ()  << " !");
      }
    }

    if (last < 0) {
      if (a_config.has_key ("number")) {
        number = a_config.fetch_integer ("number");
        DT_THROW_IF(number <= 0,
                    std::domain_error,
                    "Invalid 'number' value in module named '" << get_name () << " !");
      }
    }

    if (a_config.has_flag ("inverted")) {
      inverted = true;
    }

    if (! _module_.handle) {
      DT_THROW_IF(! a_config.has_key ("module"),
                  std::logic_error,
                  "Missing 'module' property ! ");
      std::string module_name = a_config.fetch_string ("module");
      module_handle_dict_type::iterator found
        = a_module_dict.find (module_name);
      DT_THROW_IF(found == a_module_dict.end (),
                  std::logic_error,
                  "Can't find any module named '" << module_name
                  << "' from the external dictionnary !");
      _module_.label  = found->first;
      _module_.handle = found->second.grab_initialized_module_handle ();
    }

    // Setup :
    if (number < 0) {
      set_first_last (first, last, inverted);
    } else {
      set_first_number (first, number, inverted);
    }
    _counter_ = 0;
    _set_initialized (true);
    return;
  }

  // Reset :
  void skip_module::reset()
  {
    DT_THROW_IF(! is_initialized (),
                std::logic_error,
                "Skip module '" << get_name () << "' is not initialized !");
    _set_initialized(false);
    _module_.handle.reset();
    _set_defaults();
    return;
  }

  // Processing :
  base_module::process_status
  skip_module::process(::datatools::things & the_data_record)
  {
    DT_THROW_IF(! is_initialized (),
                std::logic_error,
                "Skip module '" << get_name () << "' is not initialized !");

    bool process_module = false;
    if (_counter_ >= _first_) {
      process_module = true;
      if (_last_ >= 0) {
        if (_counter_ > _last_){
          process_module = false;
        }
      }
    }
    if (_inverted_) {
      process_module = ! process_module;
    }
    process_status status = PROCESS_SUCCESS;
    if (process_module) {
      base_module & a_module = _module_.handle.grab ();
      try {
        status = a_module.process (the_data_record);
        if (status & PROCESS_ERROR) {
          // Stop the chain if some chained module failed :
          return status;
        } else if (status & PROCESS_STOP) {
          // Stop the chain if some chained module ask for stop :
          return status;
        }
      }
      catch (std::exception & x) {
        DT_LOG_ERROR(_logging,
                     "Module '" << _module_.label << "' failed to process data record ! "
                     << x.what ());
        return PROCESS_ERROR;
      }
    }
    _counter_++;
    return PROCESS_SUCCESS;
  }

}  // end of namespace dpp

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
