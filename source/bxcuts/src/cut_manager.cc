/* cut_manager.cc
 * 
 * Copyright (C) 2011 Francois Mauger <mauger@lpccaen.in2p3.fr>
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

#include <cuts/cut_manager.h>
#include <cuts/cut_factory.h>
#include <cuts/i_cut.h>

#include <datatools/utils/properties.h>
#include <datatools/utils/multi_properties.h>
#include <datatools/utils/utils.h>
#include <datatools/utils/ioutils.h>
#include <datatools/services/service_manager.h>

namespace cuts {
  
      bool cut_manager::is_debug () const
      {
        return flags_ & DEBUG;
      }
  
      bool cut_manager::is_verbose () const
      {
        return flags_ & VERBOSE;
      }
  
      bool cut_manager::is_no_preload () const
      {
        return flags_ & FACTORY_NOPRELOAD;
      }
  
      void cut_manager::set_debug (bool a_debug)
      {
        if (a_debug)
          {
            flags_ |= DEBUG;
          }
        else
          {
            flags_ ^= DEBUG;
          }
        return;
      }
 
      void cut_manager::set_verbose (bool a_verbose)
      {
        if (a_verbose)
          {
            flags_ |= VERBOSE;
          }
        else
          {
            flags_ ^= VERBOSE;
          }
        return;
      }

      // ctor:
      cut_manager::cut_manager (uint32_t the_flags)
      {
        initialized_ = false;
        flags_ = the_flags;
        factory_ = 0;
        service_manager_owner_ = false;
        service_manager_ = 0;
        return;
      }

      bool cut_manager::has_service_manager () const
      {
        return service_manager_ != 0;
      }

      const datatools::service::service_manager & 
      cut_manager::get_service_manager () const
      {
        if (service_manager_ == 0)
          {
            ostringstream message;
            message << "cuts::cut_manager::get_service_manager: "
                    << "Cut manager is no service manager !";
            throw logic_error (message.str ());           
          }
        return *service_manager_;
      }

      datatools::service::service_manager & 
      cut_manager::get_service_manager ()
      {
        if (service_manager_ == 0)
          {
            ostringstream message;
            message << "cuts::cut_manager::get_service_manager: "
                    << "Cut manager is no service manager !";
            throw logic_error (message.str ());           
          }
        return *service_manager_;
      }

      void cut_manager::set_service_manager (datatools::service::service_manager & a_service_manager)
      {
        if (is_initialized ())
          {
            ostringstream message;
            message << "cuts::cut_manager::set_service_manager: "
                    << "Cut manager is already initialized !";
            throw logic_error (message.str ());
          }
        service_manager_owner_ = false;
        service_manager_ = &a_service_manager;
        return;
      }

      // dtor:
      cut_manager::~cut_manager ()
      {
        if (is_initialized ())
          {
            cut_manager::reset ();
          }
        return;
      }

      void cut_manager::install_service_manager (const datatools::utils::properties & a_service_manager_configuration)
      {
        if (is_debug ())
          {
            clog << datatools::utils::io::debug
                 << "cuts::cut_manager::install_service_manager: " 
                 << "Entering..." << endl; 
          }
        if (is_initialized ())
          {
            ostringstream message;
            message << "cuts::cut_manager::install_service_manager: "
                    << "Cut manager is already initialized !";
            throw logic_error (message.str ());
          }
        if (service_manager_ != 0)
          {
            ostringstream message;
            message << "cuts::cut_manager::install_service_manager: "
                    << "Cut manager has already an embedded service manager !";
            throw logic_error (message.str ());           
          }
        service_manager_ = new datatools::service::service_manager;
        if (is_verbose ())
          {
            clog << datatools::utils::io::notice
                 << "cuts::cut_manager::install_service_manager: " 
                 << "Initializing the embedded service manager..." << endl; 
          }
        service_manager_->initialize (a_service_manager_configuration);
        service_manager_owner_ = true;
        if (is_debug ())
          {
            clog << datatools::utils::io::debug
                 << "cuts::cut_manager::install_service_manager: " 
                 << "Exiting." << endl; 
          }
        return;
      }

      bool cut_manager::is_initialized () const
      {
        return initialized_;
      }
      
      void cut_manager::initialize (const datatools::utils::properties & a_setup)
      {
        if (is_initialized ())
          {
            ostringstream message;
            message << "cuts::cut_manager::initialize: "
                    << "Cut manager is already initialized !";
            throw logic_error (message.str ());
          }

        if (a_setup.has_flag ("debug"))
          {
            set_debug (true);
          }

        if (a_setup.has_flag ("verbose"))
          {
            set_verbose (true);
          }

        if (a_setup.has_flag ("factory.no_preload"))
          {
            flags_ |= FACTORY_NOPRELOAD;
          }

        if (a_setup.has_flag ("factory.debug"))
          {
            flags_ |= FACTORY_DEBUG;
          }

        // Embed a  service manager :
        if (! has_service_manager ())
          {
            string service_manager_configuration_file;
            if (a_setup.has_key ("service_manager.configuration"))
              {
                service_manager_configuration_file 
                  = a_setup.fetch_string ("service_manager.configuration");
                if (is_verbose ())
                  {
                    clog << datatools::utils::io::notice
                         << "cuts::cut_manager::initialize: " 
                         << "Embeds a service manager setup from file '"
                         << service_manager_configuration_file << "' !" << endl; 
                  }
                datatools::utils::fetch_path_with_env (service_manager_configuration_file);
                datatools::utils::properties service_manager_configuration;
                datatools::utils::properties::read_config (service_manager_configuration_file,
                                                           service_manager_configuration);      
                install_service_manager (service_manager_configuration);
              }
            else
              {
                clog << datatools::utils::io::warning
                     << "cuts::cut_manager::initialize: " 
                     << "No service manager configuration property !" << endl;              
              }
          }

        // Allocate a cut factory :
        {
          uint32_t factory_flags = cut_factory::blank;
          if (flags_ & FACTORY_NOPRELOAD)
            {
              factory_flags |= cut_factory::no_preload;
            }
          if (flags_ & FACTORY_DEBUG)
            {
              factory_flags |= cut_factory::debug;
            }
          factory_ = new cut_factory (factory_flags);
        }

        // Check if some service manager is available :
        if (! has_service_manager ())
          {
            clog << datatools::utils::io::warning
                 << "cuts::cut_manager::initialize: " 
                 << "No service manager is available ! This could forbid the use of some cuts !" << endl; 
          }
        else
          {
            clog << datatools::utils::io::warning
                 << "cuts::cut_manager::initialize: " 
                 << "We now have a service manager available !" << endl; 
          }

        // Load and create the data processing cuts :
        vector<string> cuts_configurations;
        if (a_setup.has_key ("cuts.configuration_files"))
          {
            a_setup.fetch ("cuts.configuration_files", cuts_configurations);
          }
        for (int i = 0; i < cuts_configurations.size (); ++i)
          {
            string filename = cuts_configurations[i];
            datatools::utils::fetch_path_with_env (filename);
            datatools::utils::multi_properties configs;
            clog << datatools::utils::io::warning
                 << "cuts::cut_manager::load_cuts_: " 
                 << "Loading cuts from file '" 
                 << filename << "'..." << endl; 
            configs.read (filename);
            load_cuts_ (configs);
          }

        initialized_ = true;
        return;
      }

      void cut_manager::load_cuts_ (const datatools::utils::multi_properties & a_config)
      {
        using namespace datatools::utils;
        const multi_properties::entries_ordered_col_t & oe = a_config.ordered_entries ();
        for (multi_properties::entries_ordered_col_t::const_iterator i =
               oe.begin ();
             i != oe.end ();
             ++i)
          {
            const multi_properties::entry & the_entry = **i;
            const string & cut_name = the_entry.get_key ();
            if (has (cut_name))
              {
                ostringstream message;
                message << "cuts::cut_manager::load_cuts_: "
                        << "Cut manager already has a cut named '"
                        << cut_name << "' !";
                throw logic_error (message.str ());             
              }
            const string & cut_id   = the_entry.get_meta ();
            const properties & cut_config = the_entry.get_properties ();  
            cut_handle_type handle = factory_->create_cut (cut_id, 
                                                                 cut_config,
                                                                 *service_manager_,
                                                                 cuts_);
            if (! handle.has_data ())
              {
                ostringstream message;
                message << "cuts::cut_manager::load_cuts_: "
                        << "Cut manager's embedded factory could not create cut named '"
                        << cut_name << "' of type '" << cut_id << "' !";
                throw logic_error (message.str ());                             
              }
            else
              {
                ostringstream message;
                message << "cuts::cut_manager::load_cuts_: "
                        << "Cut manager's embedded factory has created the cut named '"
                        << cut_name << "' of type '" << cut_id << "' !";
                clog << datatools::utils::io::notice
                     << message.str ()
                     << endl;
              }
            cuts_[cut_name] = handle;
          }
        return;
      }
   
      void cut_manager::reset ()
      {
        if (! is_initialized ())
          {
            ostringstream message;
            message << "cuts::cut_manager::reset: "
                    << "Cut manager is not initialized !";
            throw logic_error (message.str ());
          }
 
        cuts_.clear ();

        if (factory_ != 0)
          {
            delete factory_;
            factory_ = 0;
          }

        if (service_manager_owner_)
          {
            if (service_manager_ != 0)
              {
                delete service_manager_;
              }
          }
        service_manager_ = 0;
        service_manager_owner_ = false;
        flags_ = BLANK;
        initialized_ = false;
        return;
      }
      
      bool cut_manager::has (const string & a_cut_name) const
      {
        return cuts_.find (a_cut_name) != cuts_.end ();
      }
     
      void cut_manager::remove (const string & a_cut_name)
      {
        cuts_.erase (a_cut_name);
        return;
      }

      i_cut & 
      cut_manager::get (const string & a_cut_name)
      {
        cut_handle_dict_type::iterator found = cuts_.find (a_cut_name);
        if (found == cuts_.end ())
          {
            ostringstream message;
            message << "cuts::cut_manager::get: "
                    << "No cut named '" << a_cut_name << "' !";
            throw logic_error (message.str ());     
          }       
        return found->second.get ();
      }

      const i_cut & 
      cut_manager::get (const string & a_cut_name) const
      {
        cut_handle_dict_type::const_iterator found 
          = cuts_.find (a_cut_name);
        if (found == cuts_.end ())
          {
            ostringstream message;
            message << "cuts::cut_manager::get: "
                    << "No cut named '" << a_cut_name << "' !";
            throw logic_error (message.str ());     
          }       
        return found->second.get ();
      }

      cut_handle_dict_type & cut_manager::get_cuts ()
      {
        return cuts_;
      }

      const cut_handle_dict_type & cut_manager::get_cuts () const 
      {
        return cuts_;
      }
      
      const cut_factory & cut_manager::get_factory () const 
      {
        if (! is_initialized ())
          {
            ostringstream message;
            message << "cuts::cut_manager::get_factory: "
                    << "Cut manager is not initialized !";
            throw logic_error (message.str ());
          }
        return *factory_;
      }

      void cut_manager::tree_dump (ostream & a_out, 
                                      const string & a_title,
                                      const string & a_indent,
                                      bool a_inherit) const
      {
        string indent;
        if (! a_indent.empty ())
          {
            indent = a_indent;
          }
        if ( ! a_title.empty () ) 
          {
            a_out << indent << a_title << endl;
          }   
       
        namespace du = datatools::utils;
       
        a_out << indent << du::i_tree_dumpable::tag 
              << "Initialized       : " << is_initialized () << endl;
        a_out << indent << du::i_tree_dumpable::tag 
              << "Flags             : " << flags_ << endl;

        if (is_initialized ())
          {
            a_out << indent << du::i_tree_dumpable::tag 
                  << "Factory           : " << factory_ << endl;
            ostringstream indent_ss;
            indent_ss << indent << du::i_tree_dumpable::skip_tag;
            factory_->tree_dump (clog, "", indent_ss.str ());
          }

        if (is_initialized ())
          {
            size_t sz = cuts_.size ();
            a_out << indent << du::i_tree_dumpable::tag 
                  << "Cuts           : " << sz << " element" << (sz < 2? "": "s")  << endl;
            ostringstream indent_ss;
            int count = 0;
            for (cut_handle_dict_type::const_iterator it = cuts_.begin ();
                 it != cuts_.end ();
                 it++) 
              {
                count++;
                a_out << indent;
                a_out << du::i_tree_dumpable::skip_tag;
                if (count == sz) 
                  {
                    a_out << du::i_tree_dumpable::last_tag; 
                  }
                else 
                  {
                    a_out << du::i_tree_dumpable::tag;
                  }
                a_out << "Name='" << it->first 
                      << "' : type='" << it->second.get ().get_name () << "'"
                      << endl;
              }
          }

        a_out << indent << du::i_tree_dumpable::tag 
              << "Service manager   :" ;
        if (has_service_manager ())
          {
            a_out << " @ " << service_manager_ << endl;
            ostringstream indent_ss;
            indent_ss << indent << du::i_tree_dumpable::skip_tag;
            service_manager_->tree_dump (clog, "", indent_ss.str ()); 
          }
        else
          {
            a_out << " <none>" << endl;
          }

        a_out << indent << du::i_tree_dumpable::inherit_tag (a_inherit)  
              << "Owns service manager  : " << service_manager_owner_ << endl;

        return;
      }
  
}  // end of namespace cuts

// end of cut_manager.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
