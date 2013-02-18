/* context_service.cc
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

#include <sstream>
#include <stdexcept>

#include <boost/filesystem.hpp>

#include <dpp/dpp_config.h>

#include <dpp/context_service.h>

#if DPP_DATATOOLS_LEGACY == 1
#include <datatools/utils/multi_properties.h>
#include <datatools/utils/utils.h>
#include <datatools/utils/ioutils.h>
#else
#include <datatools/multi_properties.h>
#include <datatools/utils.h>
#include <datatools/ioutils.h>
#endif

namespace dpp {

  /** Auto-registration of this service class in a central service Db */
  DATATOOLS_SERVICE_REGISTRATION_IMPLEMENT(context_service, "dpp::context_service")
  
  bool context_service::is_debug () const
  {
    return debug_;
  }
    
  void context_service::set_debug (bool a_debug)
  {
    debug_ = a_debug;
    return;
  }
    
  bool context_service::is_verbose () const
  {
    return verbose_;
  }
    
  void context_service::set_verbose (bool a_verbose)
  {
    verbose_ = a_verbose;
    return;
  }
    
  const DPP_DU::multi_properties & 
  context_service::get_store () const     
  {
    if (! is_initialized ())
      {
        std::ostringstream message;
        message << "dpp::context_service::get_store: "
                << "Service '" << get_name () << "' is not initialized ! ";
        throw std::logic_error (message.str ());
      }
    return *store_;
  }

  DPP_DU::multi_properties & 
  context_service::get_store ()     
  {
    if (! is_initialized ())
      {
        std::ostringstream message;
        message << "dpp::context_service::get_store: "
                << "Service '" << get_name () << "' is not initialized ! ";
        throw std::logic_error (message.str ());
      }
    return *store_;
  }

  const DPP_DU::multi_properties &
  context_service::operator () () const     
  {
    return get_store ();
  }
    
  DPP_DU::multi_properties & 
  context_service::operator () ()     
  {
    return get_store ();
  }
  
  bool context_service::is_initialized () const
  {
    return initialized_;
  }
    
  int context_service::initialize (const DPP_DU::properties & a_config,
                                   DPP_DS::service_dict_type & a_service_dict)
  {
    if (is_initialized ())
      {
        std::ostringstream message;
        message << "dpp::context_service::initialize: "
                << "Service '" << get_name () << "' is already initialized ! ";
        throw std::logic_error (message.str ());
      }
      
    if (a_config.has_flag ("debug"))
      {
        set_debug (true);
      }
      
    if (a_config.has_flag ("verbose"))
      {
        set_verbose (true);
      }

    if (load_filename_.empty ())
      {
        if (a_config.has_key ("load.file"))
          {
            load_filename_ = a_config.fetch_string ("load.file");
          }
      }

    if (! load_filename_.empty ())
      {
        if (backup_filename_.empty ())
          {
            if (a_config.has_key ("backup.file"))
              {
                backup_filename_ = a_config.fetch_string ("backup.file");
              }
          }
      }

    if (store_filename_.empty ())
      {
        if (a_config.has_key ("store.file"))
          {
            store_filename_ = a_config.fetch_string ("store.file"); 
          }
      }

    store_ = new DPP_DU::multi_properties;
    store_->set_debug (debug_);
    if (! load_filename_.empty ())
      {
        std::string dummy = load_filename_;
        DPP_DU::fetch_path_with_env (dummy);
        if (boost::filesystem::exists (dummy))
          {         
            if (is_verbose ())
              {
                std::ostringstream message;
                message << "dpp::context_service::initialize: "
                        << "Service '" << get_name () 
                        << "' is loading from file '"
                        << load_filename_ << "' !";
                std::clog << DPP_DU::io::notice 
                          << message.str () << std::endl; 
              }
            store_->read (dummy);
            if (! backup_filename_.empty ())
              {
                std::string dummy2 = backup_filename_;
                DPP_DU::fetch_path_with_env (dummy2);
                if (is_verbose ())
                  {
                    std::ostringstream message;
                    message << "dpp::context_service::initialize: "
                            << "Service '" << get_name () << "' is backuping in file '"
                            << dummy2 << "' !";
                    std::clog << DPP_DU::io::notice 
                              << message.str () << std::endl;
                  }
                store_->write (dummy2);
              }
          }
        else
          {
            std::ostringstream message;
            message << "dpp::context_service::initialize: "
                    << "Service '" << get_name () 
                    << "' cannot load from file '"
                    << dummy << "' !";
            std::clog << DPP_DU::io::warning 
                      << message.str () << std::endl; 
          }
      }
    initialized_ = true;
    return EXIT_SUCCESS;
  }
    
  int context_service::reset ()
  {
    // std::cerr << "DEVEL: " << "dpp::context_service::reset: "
    //     << "Entering..." << std::endl;
    // std::cerr << "DEVEL: " << "dpp::context_service::reset: "
    //     << "initialized_ == " << initialized_ << std::endl;

    if (! is_initialized ())
      {
        std::ostringstream message;
        message << "dpp::context_service::reset: "
                << "Service '" << get_name () << "' is not initialized ! ";
        throw std::logic_error (message.str ());
      }

    if (store_ != 0)
      {
        if (! store_filename_.empty ())
          {
            if (is_verbose ())
              {
                std::ostringstream message;
                message << "dpp::context_service::reset: "
                        << "Service '" << get_name () << "' is storing in file '"
                        << store_filename_ << "' !";
                std::clog << DPP_DU::io::notice 
                          << message.str () << std::endl;
              }
            std::string dummy = store_filename_;
            DPP_DU::fetch_path_with_env (dummy);
            store_->write (dummy);
          }
        delete store_;
        store_ = 0;
      }

    load_filename_.clear ();
    store_filename_.clear ();
    backup_filename_.clear ();
    debug_ = false;
    verbose_ = false;
    initialized_ = false;
    return EXIT_SUCCESS;
  }
    
  // ctor:
  context_service::context_service () 
    : DPP_DS::base_service ("dpp::context_service",
                            "Context service")
  {
    initialized_ = false;
    debug_ = false;
    verbose_ = false;
    store_ = 0;
    return;
  }
    
  // dtor:
  context_service::~context_service ()
  { 
    if (this->context_service::is_initialized ())
      {
        this->context_service::reset ();
      }
    return;
  }

  void context_service::tree_dump (std::ostream & a_out , 
                                   const std::string & a_title,
                                   const std::string & a_indent,
                                   bool a_inherit) const
  {
    namespace du = DPP_DU;
    this->base_service::tree_dump (a_out, a_title, a_indent, true);

    a_out << a_indent << du::i_tree_dumpable::tag
          << "Debug : " << debug_ << " " << std::endl;

    a_out << a_indent << du::i_tree_dumpable::tag
          << "Verbose : " << verbose_ << " " << std::endl;

    a_out << a_indent << du::i_tree_dumpable::tag
          << "Load filename   : '" << load_filename_ << "' " << std::endl;

    a_out << a_indent << du::i_tree_dumpable::tag
          << "Backup filename : '" << backup_filename_ << "' " << std::endl;

    a_out << a_indent << du::i_tree_dumpable::tag
          << "Store filename  : '" << store_filename_ << "' " << std::endl;

    if (store_ != 0)
      {
        a_out << a_indent << du::i_tree_dumpable::inherit_tag (a_inherit)
              << "Store : [" << store_ << "] " << std::endl;
        std::ostringstream indent_ss;
        indent_ss << a_indent << du::i_tree_dumpable::inherit_skip_tag (a_inherit);
        store_->tree_dump (std::clog, "", indent_ss.str ());
      }
    else
      {
        a_out << a_indent << du::i_tree_dumpable::inherit_tag (a_inherit)
              << "Store : <none>" << std::endl;
      }
    return;
  }

}  // end of namespace dpp

// end of context_service.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
