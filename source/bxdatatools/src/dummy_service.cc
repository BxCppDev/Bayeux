/* dummy_service.cc
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

#include <datatools/services/dummy_service.h>

namespace datatools {
  
  namespace service {

		/** Auto-registration of this service class in a central service Db */
		DATATOOLS_SERVICE_REGISTRATION_IMPLEMENT(dummy_service, "datatools::service::dummy_service")
  		
    const string & dummy_service::get_label () const
    {
      return label_;
    }
    
    void dummy_service::set_label (const string & a_label)
    {
      label_ = a_label;
      return;
    }
 
    bool dummy_service::is_initialized () const
    {
      return ! label_.empty (); 
    }
    
    int dummy_service::initialize (const datatools::utils::properties & a_config,
				   service_dict_type & a_service_dict)
    {
      return EXIT_SUCCESS;
    }
    
    int dummy_service::reset ()
    {
      label_ = "";
      return EXIT_SUCCESS;
    }
    
    // ctor:
    dummy_service::dummy_service () 
      : base_service ("datatools::service::dummy",
		      "A dummy service")
    {
      label_ = "";
      return;
    }
    
    // dtor:
    dummy_service::~dummy_service ()
    {
      if (! dummy_service::is_initialized ())
				{
					dummy_service::reset ();
				}
      return;
    }

    void dummy_service::tree_dump (ostream & a_out , 
																	 const string & a_title,
																	 const string & a_indent,
																	 bool a_inherit) const
    {
      namespace du = datatools::utils;
      this->base_service::tree_dump (a_out, a_title, a_indent, true);
      a_out << a_indent << du::i_tree_dumpable::inherit_tag (a_inherit)
						<< "Label : '" << label_ << "'" << endl;
     
      return;
    }

  }  // end of namespace service
  
}  // end of namespace datatools

// end of dummy_service.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
