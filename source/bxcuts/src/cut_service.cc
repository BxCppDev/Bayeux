/* cut_service.cc
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

#include <cuts/cut_service.h>

#include <sstream>
#include <stdexcept>

#include <cuts/cut_manager.h>
#include <datatools/utils/utils.h>

namespace cuts {
  
  /** Auto-registration of this service class in a central service Db */
  DATATOOLS_SERVICE_REGISTRATION_IMPLEMENT(cut_service, "cuts::cut_service")
             
	bool cut_service::is_debug () const
	{
		return _debug_;
	}
	
	void cut_service::set_debug (bool a_debug)
	{
		_debug_ = a_debug;
		return;
	}
	
  bool cut_service::owns_cut_manager () const
  {
    return _cut_manager_ != 0 && _owns_manager_;
  }

	cut_manager & cut_service::grab_cut_manager ()
  {
    if (_cut_manager_ == 0)
      {
        ostringstream message;
        message << "cuts::cut_service::grab_cut_manager: "
                << "No embedded cut manager is defined !"
                << endl;
        throw logic_error (message.str ());
      }
    return *_cut_manager_;
  }

  const cut_manager & cut_service::get_cut_manager () const
  {
    if (_cut_manager_ == 0)
      {
        ostringstream message;
        message << "cuts::cut_service::get_cut_manager: "
                << "No embedded cut manager is defined !"
                << endl;
        throw logic_error (message.str ());
      }
    return *_cut_manager_;
  }
  
  void cut_service::set_cut_manager (const cut_manager & a_cut_manager)
  {
    if (&a_cut_manager == _cut_manager_)
      {
        ostringstream message;
        message << "cuts::cut_service::set_cut_manager: "
                << "Self-referenced manager is not allowed !"
                << endl;
        throw logic_error (message.str ());
      }
    if (_cut_manager_ != 0 && _owns_manager_)
      {
        delete _cut_manager_;
      }
    _cut_manager_ = 0;
    _owns_manager_ = false; 
    _cut_manager_ = const_cast<cut_manager*> (&a_cut_manager);
    return;
  }

  bool cut_service::is_initialized () const
  {
    return _cut_manager_ != 0;
  }

  int cut_service::initialize (const datatools::utils::properties & a_config,
                               datatools::service::service_dict_type & a_service_dict)
  {
		if (is_initialized ())
			{
				ostringstream message;
				message << "cuts::cut_service::initialize: "
								<< "Service '" << get_name () << "' is already initialized ! ";
				throw logic_error (message.str ());
			}
		
		if (a_config.has_flag ("debug"))
			{
				set_debug (true);
			}

    if (_cut_manager_ == 0)
      {
        if (a_config.has_key ("cut_manager.config"))
          {
            string config_filename = a_config.fetch_string ("cut_manager.config");
            datatools::utils::properties cut_manager_config;
            datatools::utils::fetch_path_with_env (config_filename);
      
            datatools::utils::properties::read_config (config_filename,
                                                       cut_manager_config);
            _cut_manager_ = new cut_manager;
            _owns_manager_ = true;
            _cut_manager_->set_debug (is_debug ());
            _cut_manager_->initialize (cut_manager_config);
          }
      }
 
		return EXIT_SUCCESS;
  }

  int cut_service::reset ()
  {
       if (! is_initialized ())
          {
            ostringstream message;
            message << "cuts::cut_service::reset: "
                    << "Service '" << get_name () << "' is not initialized ! ";
            throw logic_error (message.str ());
          }
    if (_owns_manager_ && _cut_manager_ != 0)
      {
        _cut_manager_->reset ();
        delete _cut_manager_;
        _cut_manager_ = 0;
      }
    _owns_manager_ = false; 
    return EXIT_SUCCESS;
  }

  // ctor:
  cut_service::cut_service ()
    : base_service ("cuts::cut_service",
                    "A cut service")
  {
		_debug_ = false;
    _owns_manager_ = false;
    _cut_manager_ = 0; 
    return;
  }

  // dtor:
  cut_service::~cut_service ()
  {
    if (cut_service::is_initialized ())
      {
        this->cut_service::reset ();
      }
    return;
  }

  void cut_service::tree_dump (ostream & a_out ,
                               const string & a_title,
                               const string & a_indent,
                               bool a_inherit) const
  {
    namespace du = datatools::utils;
    this->base_service::tree_dump (a_out, a_title, a_indent, true);
    a_out << a_indent << du::i_tree_dumpable::tag
          << "Owns manager : '" << _owns_manager_ << "'" << endl;
    a_out << a_indent << du::i_tree_dumpable::inherit_tag (a_inherit)
          << "Cut manager  :  " << _cut_manager_ << endl;

    return;
  }

}  // end of namespace cuts

// end of cut_service.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
