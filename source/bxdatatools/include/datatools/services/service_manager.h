/* service_manager.h
 * Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-06-07
 * Last modified : 2011-06-17
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
 * 
 * Description: 
 *
 *   A service manager.
 * 
 * History: 
 * 
 */

#ifndef __datatools__service__service_manager_h
#define __datatools__service__service_manager_h 1

#include <string>
#include <map>

#include <datatools/services/service_tools.h>
#include <datatools/utils/i_tree_dump.h>

namespace datatools {

    namespace utils {
			class properties;
			class multi_properties;
		}

    namespace service {

      using namespace std;

      class service_manager : public datatools::utils::i_tree_dumpable
      {
      public:

				// ctor :
				service_manager (const string & a_name = "", 
												 const string & a_description = "", 
												 bool a_preload = true,
												 bool a_abort_on_error = false,
												 bool a_debug = false);
				
				// dtor :
				~service_manager ();

				void set_name (const string & a_name);

				void set_description (const string & a_description);

				const string & get_name () const;

				const string & get_description () const;

				bool is_debug () const;

				void set_debug (bool = true);

				bool is_abort_on_error () const;

				void set_abort_on_error (bool = true);

			private:

				void set_preload_ (bool);

			public:

				bool is_initialized () const;

				void initialize ();

				void initialize (const datatools::utils::properties & a_config);

				void reset ();
	
				virtual void tree_dump (ostream & a_out         = clog, 
																const string & a_title  = "",
																const string & a_indent = "",
																bool a_inherit          = false) const;
				  
			protected:

				void at_initialize_ (const datatools::utils::properties & a_config);

				void at_reset_ ();

			public:

				bool has_service (const string & a_service_name) const;

				const service_dict_type & get_services () const;

				service_dict_type & get_services ();

				bool can_drop_service (const string & a_service_name);

				int drop_service (const string & a_service_name);

				int load_service (const string & a_service_name,
													const string & a_service_id,
													const datatools::utils::properties & a_service_config,
													bool a_replace  = false);

				void load (const datatools::utils::multi_properties & a_config);
				
				// virtual base_service * create_service (const string & a_service_id,
				// 																			 const datatools::utils::properties & a_service_configuration, 
				// 																			 service_dict_t & a_service_dict,
				// 																			 void * a_user_resource = 0);
				
				void dump_services (ostream & a_out = clog,
														const string & a_title = "",
														const string & a_indent = "") const;

				// factory dictionnary :
				
			protected:

				void preload_global_dict_ ();

			public:

				bool has_creator (const string & a_service_id) const;
 
				int register_creator (const service_creator_type & a_service_creator, 
															const string & a_service_id);

				int unregister_creator (const string & a_service_id);
				
				const service_creator_dict_type & get_creators () const;   
				
				service_creator_dict_type & get_creators ();   

      private:

				bool debug_;
				string name_;
				string description_;
				bool initialized_;
				bool abort_on_error_;
				bool preload_;
				service_creator_dict_type creators_;
				service_dict_type         services_;
				void *                    user_resources_;
      };

    }  // end of namespace service

}  // end of namespace datatools

#endif // __datatools__service__service_manager_h

// end of service_manager.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
