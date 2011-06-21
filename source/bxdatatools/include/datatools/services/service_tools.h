/* service_tools.h
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
 * 
 * Description: 
 *
 *   Service tools, typedefs.
 * 
 * History: 
 * 
 */

#ifndef __datatools__service__service_tools_h
#define __datatools__service__service_tools_h 1

#include <string>
#include <map>

#include <boost/cstdint.hpp>

#include <datatools/utils/handle.h>
#include <datatools/utils/i_tree_dump.h>
#include <datatools/utils/properties.h>

namespace datatools {

	namespace service {
		
		using namespace std;

		class base_service;
		
		typedef datatools::utils::handle<base_service> service_handle_type;
		
		/** Constants to measure the level of dependance between services :
		 */
		enum dependency_level_type
      {
				NO_DEPENDENCY       =  0, //!< The service does not depend on the external service
				OPTIONAL_DEPENDENCY =  1, //!< The service can work without the external service
				WEAK_DEPENDENCY     =  2, //!< Not so strong dependency on the external service (however part of the service may be invalidated)
				STRICT_DEPENDENCY   =  3, //!< Strictly depends on the external service
				UNKNWON_DEPENDENCY  = -1
      };
		
		/** Record that store informations about the dependency  between services :
		 */
		struct dependency_info_type
		{
			string id;      //!< ID of the external service
			string version; //!< Version of the external service
			string meta;    //!< Auxiliary information 
			int    level;   //!< Level of the dependency (see dependency_level_type enum)
			dependency_info_type ();
		};

		typedef map<string, int> dependency_level_dict_type;
		typedef map<string, dependency_info_type> service_dependency_dict_type;

		/** Record that handle an dynamically allocated service and additional informations :
		 */
		class service_entry : public datatools::utils::i_tree_dumpable 
		{
		public:

			enum status_type
				{
					STATUS_BLANK             = 0x0,
					STATUS_CREATED           = 0x1,
					STATUS_INITIALIZED       = 0x2,
					STATUS_BROKEN_DEPENDENCY = 0x4
				};
			
			string                       service_name;    //!< The name of the service
			string                       service_id;      //!< The ID (type) of the service
			datatools::utils::properties service_config;  //!< The configuration of the service 
			uint32_t                     service_status;  //!< The status of the service
			service_handle_type          service_handle;  //!< The handle for the allocated service
			service_dependency_dict_type service_masters; //!< The list of services the service depends on (by names)
			dependency_level_dict_type   service_slaves;  //!< The list of depending services (by names)
			
			service_entry ();

			bool can_be_dropped () const;
			bool has_slave (const string & a_service_name) const;
			void remove_slave (const string & a_service_name);
			virtual void tree_dump (ostream & a_out         = clog, 
															const string & a_title  = "",
															const string & a_indent = "",
															bool a_inherit          = false) const;

		};

		typedef map<string, service_entry> service_dict_type;

		/*
		struct i_service_creator
		{
			virtual datatools::service::base_service * allocate () = 0;
			virtual void initialize (base_service &, 
															 const datatools::utils::properties & a_configuration, 
															 datatools::service::service_dict_type & a_service_dict) = 0;
		};
		*/

		typedef datatools::service::base_service * 
		(*service_creator_type) (const datatools::utils::properties & a_configuration, 
														 datatools::service::service_dict_type & a_service_dict,
														 base_service * a_ptr);
		
		typedef map<string, service_creator_type> service_creator_dict_type;




	}  // end of namespace service

}  // end of namespace datatools

#endif // __datatools__service__service_tools_h

// end of service_tools.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
