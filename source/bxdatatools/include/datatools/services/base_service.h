/* base_service.h
 * Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-06-07
 * Last modified : 2011-06-07
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
 *   Base service.
 * 
 * History: 
 * 
 */

#ifndef __datatools__service__base_service_h
#define __datatools__service__base_service_h 1

#include <string>

#include <boost/scoped_ptr.hpp>

#include <datatools/utils/i_tree_dump.h>
#include <datatools/services/service_tools.h>

namespace datatools {

    namespace utils {
			class properties;
		}

    namespace service {

      using namespace std;

      class base_service : public datatools::utils::i_tree_dumpable
      {
      public:
				
				const string & get_name () const;
				
				const string & get_description () const;
				
				void set_description (const string & a_description);
	
				bool has_version () const;

				const string & get_version () const;
	
				void set_version (const string & a_version);

				virtual void fetch_dependencies (service_dependency_dict_type &) const;

      public:

				virtual bool is_initialized () const = 0;

				virtual int initialize_standalone (const datatools::utils::properties & a_config);

				virtual int initialize (const datatools::utils::properties & a_config,
																service_dict_type & a_service_dict) = 0;

				virtual int reset () = 0;
    
      protected:

				void set_name_ (const string & a_name);
		  
      public: 

				// ctor:
				base_service (const string & a_process_name, 
											const string & a_process_description = "", 
											const string & a_process_version = "");
	
				// dtor:
				virtual ~base_service ();
	
				virtual void tree_dump (ostream & a_out         = clog, 
																const string & a_title  = "",
																const string & a_indent = "",
																bool a_inherit          = false) const;
	
			public: 
				
				static bool g_debug;

			protected: 
				
				string name_;           //!< The name of the service
	
				string description_;    //!< The description of the service
	
				string version_;        //!< The version of the service
				
				/**************************************************/

			public:
			
				class service_creator_db
				{
					service_creator_dict_type  dict_;
					
				public:
					
					service_creator_db (bool = false);
					
					virtual ~service_creator_db ();
					
					const service_creator_dict_type & get_dict () const;
					
					service_creator_dict_type & get_dict ();
					
					bool has_service_creator (const string & a_service_id) const;
					
					service_creator_type & get_service_creator (const string & a_service_id);
					
					void register_service_creator (const service_creator_type & a_service_creator, 
																				 const string & a_service_id);

					void register_service_creator2 ();
					

					void dump_service_creators (ostream & a_out = clog);
					
				}; // service_creator_db
				
				typedef boost::scoped_ptr<service_creator_db> scoped_service_creator_db_t;
				
			private:

				static scoped_service_creator_db_t g_service_creator_db_;

			public:
				
				static service_creator_db & get_service_creator_db ();

      };
			
			/* utility to enable auto-registering of a service class
			 * in the global dictionary:
			 *
			 * The templatized class 'Service' must inherits from 'base_service':
			 */
			template <class Service>
			struct default_creator_registration
			{
				string service_id;
			public:
				static base_service * 
				g_default_service_creator (const datatools::utils::properties & a_service_config,
																	 service_dict_type & a_service_dictionary, 
																	 base_service * a_ptr = 0)
				{
					if (a_ptr == 0)
						{
							Service * new_service = new Service;
							return new_service;
						}
					else
						{
							int status = a_ptr->initialize (a_service_config, 
																							a_service_dictionary);
							if (status != 0)
								{
									return 0;
								}
							return a_ptr;
						}
				}

				// ctor :
				default_creator_registration (const string & a_service_id) 
					: service_id (a_service_id)
				{
					base_service::get_service_creator_db ().register_service_creator (default_creator_registration<Service>::g_default_service_creator,
																																						service_id);
					return;
				}

			};
	
    }  // end of namespace service

}  // end of namespace datatools

#endif // __datatools__service__base_service_h

// end of base_service.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
