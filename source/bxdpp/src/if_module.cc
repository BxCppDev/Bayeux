/* if_module.cc
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

#include <stdexcept>
#include <sstream>

// Access to the service manager functionnalities:
#include <datatools/service_manager.h>
#include <datatools/things.h>
#include <datatools/ioutils.h>

// Access to the cut service and its embedded cut manager:
#include <cuts/cut_service.h>
#include <cuts/cut_manager.h>
#include <cuts/i_cut.h>

#include <dpp/if_module.h>

namespace dpp {

  // Registration instantiation macro :
  DPP_MODULE_REGISTRATION_IMPLEMENT(if_module, "dpp::if_module");

  const std::string if_module::DEFAULT_CUT_SERVICE_LABEL = "Cuts";

  void if_module::set_cut_service_label (const std::string & label_)
  {
    if (is_initialized ())
      {
        std::ostringstream message;
        message << "dpp::if_module::set_cut_service_label: "
                << "Already initialized !";
        throw std::logic_error (message.str ());
      }
    _cut_service_label_ = label_;
    return;
  }

  const std::string & if_module::get_cut_service_label () const
  {
    return _cut_service_label_;
  }

  void if_module::set_condition_cut (const std::string & label_,
                                     const cuts::cut_handle_type & condition_cut_)
  {
    if (is_initialized ())
      {
        std::ostringstream message;
        message << "dpp::if_module::set_condition_cut: "
                << "Already initialized !";
        throw std::logic_error (message.str ());
      }
    _condition_cut_.label  = label_;
    _condition_cut_.handle = condition_cut_;
    return;
  }

  bool if_module::has_then_status () const
  {
    return _then_status_ != PROCESS_INVALID;
  }

  bool if_module::has_else_status () const
  {
    return _else_status_ != PROCESS_INVALID;
  }

  void if_module::set_then_status (int status_)
  {
    if (is_initialized ())
      {
        std::ostringstream message;
        message << "dpp::if_module::set_then_status: "
                << "Already initialized !";
        throw std::logic_error (message.str ());
      }
    if (status_ != PROCESS_INVALID
        && status_ != PROCESS_CONTINUE
        && status_ != PROCESS_STOP)
      {
        std::ostringstream message;
        message << "dpp::if_module::set_then_status: "
                << "invalid then status !";
        throw std::logic_error (message.str ());           
      }
    _then_status_ = status_;
    if (! has_else_status () && ! _else_module_.handle.has_data ())
      {
        _else_status_ = PROCESS_STOP;
      }
    return;
  }
      
  void if_module::set_else_status (int status_)
  {
    if (is_initialized ())
      {
        std::ostringstream message;
        message << "dpp::if_module::set_else_status: "
                << "Already initialized !";
        throw std::logic_error (message.str ());
      }
    if (status_ != PROCESS_INVALID
        && status_ != PROCESS_CONTINUE
        && status_ != PROCESS_STOP)
      {
        std::ostringstream message;
        message << "dpp::if_module::set_else_status: "
                << "invalid else status !";
        throw std::logic_error (message.str ());           
      }
    _else_status_ = status_;
    if (! has_then_status () && ! _then_module_.handle.has_data ())
      {
        _then_status_ = PROCESS_STOP;
      }
    return;
  }

  bool if_module::has_then_module () const
  {
    return _then_module_.handle.has_data ();
  }

  bool if_module::has_else_module () const
  {
    return _else_module_.handle.has_data ();
  }

  void if_module::set_then_module (const module_entry & then_module_)
  {
    if (is_initialized ())
      {
        std::ostringstream message;
        message << "dpp::if_module::set_then_module: "
                << "Already initialized !";
        throw std::logic_error (message.str ());
      }
    if (has_then_status ())
      {
        _then_status_ = PROCESS_INVALID;
      }
    _then_module_ = then_module_;
    return;
  }

  void if_module::set_else_module (const module_entry & else_module_)
  {
    if (is_initialized ())
      {
        std::ostringstream message;
        message << "dpp::if_module::set_else_module: "
                << "Already initialized !";
        throw std::logic_error (message.str ());
      }
    _else_module_ = else_module_;
    if (has_else_status ())
      {
        _else_status_ = PROCESS_INVALID;
      }
    return;
  }

  void if_module::_set_defaults ()
  {
    _cut_service_label_ = "";
    _then_status_ = PROCESS_INVALID;
    _else_status_ = PROCESS_INVALID;
    return;
  }

  /*** Implementation of the interface ***/

  // Constructor :
  if_module::if_module (int debug_level_)
    : base_module ("dpp::if_module",
                   "A conditional event record processor module",
                   "0.1",
                   debug_level_)
  {
    _set_defaults ();
    return;
  }

  // Destructor :
  if_module::~if_module ()
  {
    // Make sure all internal resources are terminated
    // before destruction :
    if (is_initialized ()) reset ();
    return;
  }

  // Initialization :
  void if_module::initialize (const datatools::properties  & config_,
                              datatools::service_manager & service_manager_,
                              module_handle_dict_type             & module_dict_)
  {
    if (is_initialized ())
      {
        std::ostringstream message;
        message << "dpp::if_module::initialize: "
                << "Module '" << get_name () << "' is already initialized ! ";
        throw std::logic_error (message.str ());
      }

    if (! is_debug ())
      {
        if (config_.has_flag ("debug"))
          {
            set_debug (true);
          }
      }

    if (_cut_service_label_.empty ())
      {
        if (config_.has_key ("cut_service.label"))
          {
            std::string label = config_.fetch_string ("cut_service.label");
            set_cut_service_label (label);
          }
      }

    if (! _condition_cut_.handle.has_data ())
      {

        if (_cut_service_label_.empty ())
          {
            set_cut_service_label (DEFAULT_CUT_SERVICE_LABEL);
          }

        bool check = service_manager_.has (_cut_service_label_)
          && service_manager_.is_a<cuts::cut_service> (_cut_service_label_);
        if (! check)
          {
            std::ostringstream message;
            message << "dpp::if_module::initialize: "
                    << "Can't find any service with label '" << _cut_service_label_
                    << "' from the service manager ! ";
            throw std::logic_error (message.str ());
          }
        cuts::cut_service & the_cut_service
          = service_manager_.grab<cuts::cut_service> (_cut_service_label_);
        cuts::cut_manager & the_cut_manager = the_cut_service.grab_cut_manager ();


        if (config_.has_key ("condition_cut"))
          {
            std::string cut_name = config_.fetch_string ("condition_cut");
            cuts::cut_handle_dict_type::iterator found
              = the_cut_manager.get_cuts ().find (cut_name);
            if (found == the_cut_manager.get_cuts ().end ())
              {
                std::ostringstream message;
                message << "dpp::if_module::initialize: "
                        << "Can't find any cut named '" << cut_name
                        << "' from the cut manager ! ";
                throw std::logic_error (message.str ());
              }
            _condition_cut_.label  = found->first;
            _condition_cut_.handle = found->second.grab_initialized_cut_handle ();
          }
        else
          {
            std::ostringstream message;
            message << "dpp::if_module::initialize: "
                    << "Missing 'condition_cut' property ! ";
            throw std::logic_error (message.str ());
          }
      }

    // 2012 FM: add support for enforced 'then/else status' (without associated modules) :
    if (! has_then_status ())
      {
        if (config_.has_key ("then_status"))
          {
            std::string then_status = config_.fetch_string ("then_status");
            if (then_status == "continue")
              {
                set_then_status (PROCESS_CONTINUE);
              }
            else if (then_status == "stop")
              {
                set_then_status (PROCESS_STOP);
              }
            else
              {
                std::ostringstream message;
                message << "dpp::if_module::initialize: "
                        << "Invalid value \'" << then_status << "' for the 'then_status' property ! ";
                throw std::logic_error (message.str ());
              }
          }
      }

    if (! has_else_status ())
      {
        if (config_.has_key ("else_status"))
          {
            std::string else_status = config_.fetch_string ("else_status");
            if (else_status == "continue")
              {
                set_else_status (PROCESS_CONTINUE);
              }
            else if (else_status == "stop")
              {
                set_else_status (PROCESS_STOP);
              }
            else
              {
                std::ostringstream message;
                message << "dpp::if_module::initialize: "
                        << "Invalid value \'" << else_status << "' for the 'else_status' property ! ";
                throw std::logic_error (message.str ());
              }
          }
      }
 
    // If no then_status is defined, try to associate a processing module :
    if (! has_then_status ())
      {
        if (! _then_module_.handle.has_data ())
          {
            if (config_.has_key ("then_module"))
              {
                std::string module_name = config_.fetch_string ("then_module");
                module_handle_dict_type::iterator found
                  = module_dict_.find (module_name);
                if (found == module_dict_.end ())
                  {
                    std::ostringstream message;
                    message << "dpp::if_module::initialize: "
                            << "Can't find any module named '" << module_name
                            << "' from the external dictionnary ! ";
                    throw std::logic_error (message.str ());
                  }
                _then_module_.label = found->first;
                _then_module_.handle = found->second.grab_initialized_module_handle ();
              }
            else
              {
                std::ostringstream message;
                message << "dpp::if_module::initialize: "
                        << "Missing 'then_module' property ! ";
                throw std::logic_error (message.str ());
              }
          }
      }

    // If no else_status is defined, try to associate a processing module :
    if (! has_else_status ())
      {
        if (! _else_module_.handle.has_data ())
          {
            if (config_.has_key ("else_module"))
              {
                std::string module_name = config_.fetch_string ("else_module");
                module_handle_dict_type::iterator found
                  = module_dict_.find (module_name);
                if (found == module_dict_.end ())
                  {
                    std::ostringstream message;
                    message << "dpp::if_module::initialize: "
                            << "Can't find any module named '" << module_name
                            << "' from the external dictionnary ! ";
                    throw std::logic_error (message.str ());
                  }
                _else_module_.label = found->first;
                _else_module_.handle = found->second.grab_initialized_module_handle ();
              }
                
            if (is_debug ())
              {
                std::ostringstream message;
                message << "snemo::core::processing::if_module::initialize: "
                        << "Missing 'else_module' property ! ";
                std::clog << datatools::io::debug << message.str () << std::endl;
              }
                
          }
      }

    _set_initialized (true);
    return;
  }

  // Reset :
  void if_module::reset ()
  {
    if (! is_initialized ())
      {
        std::ostringstream message;
        message << "dpp:if_module::initialize: "
                << "Module '" << get_name () << "' is not initialized !";
        throw std::logic_error (message.str ());
      }

    // Reset handles :
    _condition_cut_.handle.reset ();
    _then_module_.handle.reset ();
    _else_module_.handle.reset ();
    // Restore default values :
    _set_defaults ();

    _set_initialized (false);
    return;
  }

  // Processing :
  int if_module::process (datatools::things & data_record_)
  {
    if (! is_initialized ())
      {
        std::ostringstream message;
        message << "dpp:if_module::process: "
                << "Module '" << get_name () << "' is not initialized !";
        throw std::logic_error (message.str ());
      }

    this->reset_last_error_message ();

    cuts::i_cut & the_cut = _condition_cut_.handle.get ();
    // Prepare the user data to be checked by the active 'cut':
    the_cut.set_user_data (&data_record_);
    // Apply the cut algorithm:
    if (is_debug ())
      {
        std::clog << datatools::io::debug
                  << "dpp:if_module::process: "
                  << "Checking cut '" << _condition_cut_.label << "'..."
                  << std::endl;
      }
    int cut_status = the_cut.process ();
    // Detach user data.
    the_cut.unset_user_data ();
    // Check the cut's returned value:
    if (cut_status == cuts::i_cut::INAPPLICABLE)
      {
        if (is_debug ())
          {
            std::clog << datatools::io::debug
                      << "dpp:if_module::process: "
                      << "Cut '" << _condition_cut_.label << "' was inapplicable."
                      << std::endl;
          }
        return STOP & ERROR;
      }
    if (cut_status == cuts::i_cut::ACCEPTED)
      {
        if (is_debug ())
          {
            std::clog << datatools::io::debug
                      << "dpp:if_module::process: "
                      << "Cut '" << _condition_cut_.label << "' has been checked."
                      << std::endl;
          }
        int process_status = SUCCESS;
        if (_then_status_ != PROCESS_INVALID)
          {
            if (_then_status_ == PROCESS_CONTINUE)
              {
                process_status = SUCCESS;
              }
            else
              {
                process_status = STOP;
              }
          }
        else
          {
            base_module & the_then_module = _then_module_.handle.get ();
            if (is_debug ())
              {
                std::clog << datatools::io::debug
                          << "dpp:if_module::process: "
                          << "Applying module '" << _then_module_.label << "'..."
                          << std::endl;
              }
            the_then_module.reset_last_error_message ();
            process_status = the_then_module.process (data_record_);
          }
        return process_status;
      }
    else
      {
        if (is_debug ())
          {
            std::clog << datatools::io::debug
                      << "dpp:if_module::process: "
                      << "Cut '" << _condition_cut_.label << "' has NOT been checked."
                      << std::endl;
          }
        int process_status = SUCCESS;
        if (_else_status_ != PROCESS_INVALID)
          {
            if (_else_status_ == PROCESS_CONTINUE)
              {
                process_status = SUCCESS;
              }
            else
              {
                process_status = STOP;
              }
          }
        else
          {
            if (_else_module_.handle.has_data ())
              {
                base_module & the_else_module = _else_module_.handle.get ();
                if (is_debug ())
                  {
                    std::clog << datatools::io::debug
                              << "dpp:if_module::process: "
                              << "Applying module '" << _else_module_.label << "'..."
                              << std::endl;
                  }
                the_else_module.reset_last_error_message ();
                process_status = the_else_module.process (data_record_);
              }
          }
        return process_status;
      }

    return ERROR;
  }

  void if_module::tree_dump (std::ostream      & out_,
                             const std::string & title_,
                             const std::string & indent_,
                             bool inherit_) const
  {
    this->base_module::tree_dump (out_, title_, indent_, true);

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Cut service label : '" << _cut_service_label_ << "'" << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Condition cut     : '" << _condition_cut_.label << "'" << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Then status       : '" << _then_status_ << "'" << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Then module       : '" << _then_module_.label << "'" << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Else status       : '" << _else_status_ << "'" << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::inherit_tag (inherit_)
         << "Else module       : '" << _else_module_.label << "'" <<std:: endl;

    return;
  }

}  // end of namespace dpp

// end of if_module.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
