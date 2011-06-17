/* service_manager.cc
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

#include <datatools/services/service_manager.h>
#include <datatools/services/base_service.h>
#include <datatools/utils/utils.h>
#include <datatools/utils/properties.h>
#include <datatools/utils/multi_properties.h>

namespace datatools {


	namespace service {

		void service_manager::set_name (const string & a_name)
		{
			name_ = a_name;
			return;
		}

		void service_manager::set_description (const string & a_description)
		{
			description_ = a_description;
			return;
		}

		const string & service_manager::get_name () const
		{
			return name_;
		}
		
		const string & service_manager::get_description () const
		{
			return description_;
		}

		bool service_manager::is_debug () const
		{
			return debug_;
		}
		
		void service_manager::set_debug (bool a_debug)
		{
			debug_ = a_debug;
			return;
		}

		bool service_manager::is_abort_on_error () const
		{
			return abort_on_error_;
		}

		void service_manager::set_abort_on_error (bool a_abort_on_error)
		{
			abort_on_error_ = a_abort_on_error;
			return;
		}
		
		void service_manager::set_preload_ (bool a_preload)
		{
			preload_ = a_preload;
			if (preload_)
				{
					preload_global_dict_ ();
				}
			return;
		}

		bool service_manager::is_initialized () const
		{
			return initialized_;
		}

		void service_manager::load (const datatools::utils::multi_properties & a_config)
		{
			bool debug = false;
			if (debug)
				{
					cerr << "DEVEL: " 
							 << "datatools::service::service_manager::load: "
							 << "Entering..."
							 << endl;	
				}
			if (is_initialized ())
				{
					ostringstream message;
					message << "manager::load: "
									<< "Service manager is already initialized !";
					throw logic_error (message.str ());					
				}

			using namespace datatools::utils;
			for (multi_properties::entries_ordered_col_t::const_iterator i 
						 = a_config.ordered_entries ().begin ();
					 i != a_config.ordered_entries ().end ();
					 i++)
				{
					multi_properties::entry * e = *i;
					const string & service_name = e->get_key ();
					const string & service_id = e->get_meta ();
					if (debug)
						{
							cerr << "DEVEL: " 
									 << "datatools::service::service_manager::load: "
									 << "Configuration for service named '" << service_name << "' "
									 << " with ID '" << service_id << "'..."
									 << endl;	
						}
					if (services_.find (service_name) != services_.end ())
						{
							ostringstream message;
							message << "manager::load: "
											<< "Service with name '" << service_name << "' already exists !";
							throw logic_error (message.str ());
						}
					load_service (service_name, service_id, e->get_properties ()); 
				}
			return;
		}

		void service_manager::at_initialize_ (const datatools::utils::properties & a_config)
		{
			if (! is_debug ())
				{
					if (a_config.has_flag ("debug"))
						{
							set_debug (true);
						}
				}

			if (name_.empty ())
				{
					if (a_config.has_key ("name"))
						{
							name_ = a_config.fetch_string ("name");
						}
				}

			if (description_.empty ())
				{
					if (a_config.has_key ("description"))
						{
							description_ = a_config.fetch_string ("description");
						}
				}

			// // creators :
			// {
			// 	vector<string> list_of_creators;
			// 	if (a_config.has_key ("creators"))
			// 		{
			// 			a_config.fetch ("creators", list_of_creators);
			// 		}
			// }

			// services :
			{
				vector<string> services_configuration_files;
				if (a_config.has_key ("services.configuration_files"))
					{
						a_config.fetch ("services.configuration_files", services_configuration_files);
					}
				for (vector<string>::iterator i = services_configuration_files.begin (); 
						 i < services_configuration_files.end (); 
						 ++i)
					{
						datatools::utils::fetch_path_with_env (*i);
						datatools::utils::multi_properties mconfig;
						mconfig.read (*i);
						this->load (mconfig);
					}
			}

			return;
		}


		void service_manager::initialize ()
		{
			datatools::utils::properties dummy_config;
			initialize (dummy_config);
			return;
		}
		

		void service_manager::initialize (const datatools::utils::properties & a_config)
		{
			if (initialized_)
				{
					ostringstream message;
					message << "datatools::service::service_manager::initialize: "
									<< "Manager is already initialized !";
					throw logic_error (message.str());	      
				}
			at_initialize_ (a_config);
			initialized_ = true;
			return;
		}


		void service_manager::at_reset_ ()
		{
			// if (services_.size () > 0)
			// 	{
			// 		for (service_dict_type::iterator i = services_.begin ();
			// 				 i != services_.end ();
			// 				 i++)
			// 			{
			// 				if (i->second != 0)
			// 					{
			// 						delete i->second;
			// 					}
			// 			}
			// 		services_.clear ();
			// 	}
			services_.clear ();

			creators_.clear ();

			abort_on_error_ = true;
			return;
		}


		void service_manager::reset ()
		{
			if (! initialized_)
				{
					ostringstream message;
					message << "datatools::service::service_manager::reset: "
									<< "Manager is not initialized !";
					throw logic_error (message.str());	      
				}

			at_reset_ ();

			initialized_ = false;
			return;
		}


		// ctor :
		service_manager::service_manager (const string & a_name, 
																			const string & a_description, 
																			bool a_preload,
																			bool a_abort_on_error,
																			bool a_debug)
		{
			initialized_ = false;			
			set_debug (a_debug);
			set_abort_on_error (a_abort_on_error);
			set_name (a_name);
			set_description (a_description);
			set_preload_ (a_preload);
			user_resources_ = 0;
			return;
		}


		service_manager::~service_manager ()
		{
			if (initialized_)
				{
					reset ();
				}
			return;
		}


		/****************
		 *   SERVICES   *
		 ****************/

		bool service_manager::has_service (const string & a_service_name) const
		{
			return services_.find (a_service_name) != services_.end ();
		}
	

		const service_dict_type & service_manager::get_services () const
		{
			return services_;
		}
	

		service_dict_type & service_manager::get_services ()
		{
			return services_;
		}


		bool service_manager::can_drop_service (const string & a_service_name)
		{
			service_dict_type::const_iterator found = services_.find (a_service_name);
			if (found == services_.end ())
				{
					ostringstream message;
					message << "datatools::service::service_manager::can_drop_service: "
									<< "Service '" << a_service_name << "' does not exist !";
					throw logic_error (message.str());	      
				}
			return found->second.can_be_dropped ();
		}
     

		int service_manager::drop_service (const string & a_service_name)
		{
			service_dict_type::const_iterator found = services_.find (a_service_name);
			if (found == services_.end ())
				{
					ostringstream message;
					message << "datatools::service::service_manager::drop_service: "
									<< "Service '" << a_service_name << "' does not exist !";
					throw logic_error (message.str());	      
				}

			const service_entry & the_service_entry = found->second;
			if (! found->second.can_be_dropped ())
				{
					ostringstream message;
					message << "datatools::service::service_manager::drop_service: "
									<< "Service '" << a_service_name 
									<< "' cannot be dropped because of existing dependent services !";
					if (is_abort_on_error ())
						{
							throw logic_error (message.str());
						}
					clog << "ERROR: " << message.str () << endl;					
					return datatools::utils::FAILURE;
				}
			return datatools::utils::SUCCESS;
		}


		int service_manager::load_service (const string & a_service_name,
																			 const string & a_service_id,
																			 const datatools::utils::properties & a_service_config,
																			 bool a_replace)
		{
			bool debug = is_debug ();
			if (debug)
				{
					cerr << "DEVEL: " 
							 << "datatools::service::service_manager::load_service: "
							 << "Entering..."
							 << endl;	
				}
			if (has_service (a_service_name))
				{
					if (! a_replace)
						{
							ostringstream message;
							message << "datatools::service::service_manager::load_service: "
											<< "Service '" << a_service_name << "' already exists !";
							if (is_abort_on_error ())
								{
									throw logic_error (message.str());	      
								}
							clog << "ERROR: " << message.str () << endl;
							return datatools::utils::FAILURE;
						}
					else
						{
							if (can_drop_service (a_service_name))
								{
									drop_service (a_service_name);
								}
							else
								{
									ostringstream message;
									message << "datatools::service::service_manager::load_service: "
													<< "Cannot drop service '" << a_service_name << "' !";
									if (is_abort_on_error ())
										{
											throw logic_error (message.str());	      
										}
									clog << "ERROR: " << message.str () << endl;
									return datatools::utils::FAILURE;
								}
						}
				}
			{
				// Add a new entry :
				service_entry dummy_service_entry;
	 			dummy_service_entry.service_name = a_service_name;
				if (debug)
					{
						cerr << "DEVEL: " 
								 << "datatools::service::service_manager::load_service: "
								 << "Add an entry for service '" << a_service_name << "'..."
								 << endl;	
					}
				services_[a_service_name] = dummy_service_entry;
				if (debug)
					{
						cerr << "DEVEL: " 
								 << "datatools::service::service_manager::load_service: "
								 << "Done."
								 << endl;	
					}
			}
			// fetch a reference on it and update :
			service_entry & the_service_entry = services_.find (a_service_name)->second;
			if (debug)
				{
					cerr << "DEVEL: " 
							 << "datatools::service::service_manager::load_service: "
							 << "Fetch..."
							 << endl;	
				}
			the_service_entry.service_id     = a_service_id;
			the_service_entry.service_status = service_entry::STATUS_OK; // default status is OK.
			// search for the service creator for this ID : 
			service_creator_dict_type::iterator found = creators_.find (a_service_id);
			if (found == creators_.end ())
				{
					ostringstream message;
					message << "datatools::service::service_manager::load_service: "
									<< "Could not find a service creator for ID '" << a_service_id 
									<< "' for service '" << a_service_name << "' !";
					if (is_abort_on_error ())
						{
							throw logic_error (message.str());	      
						}
					clog << "ERROR: " << message.str () << endl;
					services_.erase (a_service_name);
					return datatools::utils::FAILURE;								
				}

			if (debug)
				{
					cerr << "DEVEL: " 
							 << "datatools::service::service_manager::load_service: "
							 << "TEST 1"
							 << endl;	
				}

			// here we have a creator :
			service_creator_type a_creator = found->second;
			// invoke the creator and allocate a new service object using configuration parameters from
			// the 'a_service_config' properties container and the current dictionnary of services 'services_';
			// 'user_resources_' is a anonymous (void) pointer to some arbitrary external data structure (optional).
			base_service * a_new_service = a_creator (a_service_config, 
																								&services_, 
																								user_resources_);
			if (a_new_service == 0)
				{
					ostringstream message;
					message << "datatools::service::service_manager::load_service: "
									<< "Could not load a new service '" << a_service_name << "' with type '" << a_service_id << "' !";
					if (is_abort_on_error ())
						{
							throw logic_error (message.str());	      
						}
					clog << "ERROR: " 
							 << message.str () << endl;
					services_.erase (a_service_name);
					return datatools::utils::FAILURE;								
				}

			if (debug)
				{
					cerr << "DEVEL: " 
							 << "datatools::service::service_manager::load_service: "
							 << "TEST 2"
							 << endl;	
				}
			// attach the service to this manager :
			a_new_service->set_service_manager (*this);
			// let the embedded handle manage the pointer of the new service :
			the_service_entry.service_handle.reset (a_new_service);
			if (debug)
				{
					cerr << "DEVEL: " 
							 << "datatools::service::service_manager::load_service: "
							 << "TEST 3"
							 << endl;	
				}
			// check if the new service has dependencies on other services :
			a_new_service->fetch_dependencies(the_service_entry.service_masters);
			for (service_dependency_dict_type::const_iterator j 
						 = the_service_entry.service_masters.begin ();
					 j != the_service_entry.service_masters.end ();
					 ++j)
				{ 
					const string & master_service_name = j->first;
					cerr << "DEVEL: " 
							 << "datatools::service::service_manager::load_service: "
							 << "Master '" << master_service_name << "' "
							 << endl;	
					continue;
					/*
					const dependency_info_type & master_depinfo = j->second;
					// search for missing strict dependencies from the current dictionnary of services:
					service_dict_type::iterator found = services_.find (master_service_name);
					if (master_depinfo.level == STRICT_DEPENDENCY)
						{
							cerr << "DEVEL: " 
									 << "datatools::service::service_manager::load_service: "
									 << "Master '" << master_service_name << "' is a strict dependency !"
									 << endl;	
							if (found == services_.end ())
								{
									ostringstream message;
									message << "datatools::service::service_manager::load_service: "
													<< "Could not find service named '" << master_service_name 
													<< "' on which the new service '" << a_service_name << "' depends !";
									if (is_abort_on_error ())
										{
											throw logic_error (message.str());	      
										}
									clog << "ERROR: " << message.str () << endl;
									the_service_entry.service_status = service_entry::STATUS_BROKEN_DEPENDENCY;
									return datatools::utils::FAILURE;								
								}
							else
								{
									dependency_info_type di;
									di.name = a_service_name;
									di.level = STRICT_DEPENDENCY;
									found->second.service_slaves[a_service_name] = di;
								}
						}
					else if (master_depinfo.level > NO_DEPENDENCY)
						{
							dependency_info_type di;
							di.name = a_service_name;
							di.level = master_depinfo.level;
							found->second.service_slaves[a_service_name] = di;									
						}
					*/
				}
			if (debug)
				{
					cerr << "DEVEL: " 
							 << "datatools::service::service_manager::load_service: "
							 << "Exiting."
							 << endl;	
				}
			return datatools::utils::SUCCESS;
		}


		void service_manager::dump_services (ostream & a_out,
																				 const string & a_title,
																				 const string & a_indent) const
		{
			string indent;
			if (! a_indent.empty ())
				{
					indent = a_indent;
				}
			if (! a_title.empty ())
				{
					a_out << a_title << ":" << endl;
				}

			// Creators:
			{
				size_t sz = creators_.size ();
				a_out << indent << "Creators : " << endl;
				size_t count = 0; 
				for (service_creator_dict_type::const_iterator it = creators_.begin ();
						 it != creators_.end ();
						 it++) 
					{
						count++;
						a_out << indent;
						if (count == sz) a_out << "`-- "; 
						else a_out << "|-- ";
						a_out << it->first << ": " << hex << (void *) it->second 
									 << dec << endl;
					}
			}

			// Services:
			{
				size_t sz = services_.size ();
				size_t count = 0; 
				for (service_dict_type::const_iterator it = services_.begin ();
						 it != services_.end ();
						 it++) 
					{
						count++;
						a_out << indent;
						if (count == sz) a_out << "`-- "; 
						else a_out << "|-- ";
						const string & service_name = it->first;
						const base_service & a_service = it->second.service_handle.get ();
						a_out << "Name : '" << service_name << "'  Type : '" << a_service.service_id () << "'"
									 << endl;
					}
			}
 
			return;
		}
		
		/****************
		 *   CREATORS   *
		 ****************/

		void service_manager::preload_global_dict_ ()
		{
			bool devel = false;
			if (devel)
				{
					cerr << "DEVEL: " 
							 << "datatools::service::service_manager::preload_global_dict_: "
							 << "Entering..."
							 << endl;
				}
			if (creators_.size () > 0)
				{
					ostringstream message;
					message << "datatools::service::service_manager::preload_global_dict_: "
									<< "Ignore preload request !";
					clog << "WARNING: " << message.str () << endl;
				}
			const service_creator_dict_type & global_dict = base_service::get_service_creator_db ().get_dict ();
			if (devel)
				{
					cerr << "DEVEL: " 
							 << "datatools::service::service_manager::preload_global_dict_: "
							 << "Global dictionary size = " << global_dict.size ()
							 << endl;
				}

			for (service_creator_dict_type::const_iterator i = global_dict.begin ();
					 i != global_dict.end ();
					 ++i)
				{
					const string & a_service_id = i->first;
					service_creator_type a_service_creator = i->second;
					if (devel)
						{
							cerr << "DEVEL: " 
									 << "datatools::service::service_manager::preload_global_dict_: "
									 << "Preloading '" << a_service_id << "' @ " << a_service_creator
									 << endl;
						}
					int status = register_creator (a_service_creator, a_service_id);
					if (status != datatools::utils::SUCCESS)
						{
							ostringstream message;
							message << "datatools::service::service_manager::preload_global_dict_: "
											<< "Cannot register service creator with ID '" << a_service_id << "' !";
							clog << "WARNING: " 
									 << message.str () << endl;
						}
				}
			return;
		}

		const service_creator_dict_type & service_manager::get_creators () const
		{
			return creators_;
		}   
		
		service_creator_dict_type & service_manager::get_creators () 
		{
			return creators_;
		}   

		bool service_manager::has_creator (const string & a_service_id) const
		{
			return creators_.find (a_service_id) != creators_.end ();
		}

		int service_manager::unregister_creator (const string & a_service_id)
		{
			service_creator_dict_type::iterator found = creators_.find (a_service_id);
			if (found != creators_.end ())
				{
					creators_.erase (found);
				}
			return datatools::utils::SUCCESS;
		}

		int service_manager::register_creator (const service_creator_type & a_service_creator,
																					 const string & a_service_id)
		{
			cerr << "DEVEL: " 
					 << "datatools::service::service_manager::register_creator: "
					 << "Entering..."
					 << endl;
			cerr << "DEVEL: " 
					 << "datatools::service::service_manager::register_creator: "
					 << "Service ID is '" << a_service_id << "'..."
					 << endl;
			if (a_service_id.empty ())
				{
					ostringstream message;
					message << "service_manager::register_creator: " 
									<< "Missing service ID !";
					if (is_abort_on_error ())
						{
							throw logic_error (message.str ());
						}
					clog << "ERROR: " 
							 << message.str () << endl;
					return datatools::utils::ERROR;
				}
			service_creator_dict_type::const_iterator found = creators_.find (a_service_id);
			if (found != creators_.end ())
				{
					ostringstream message;
					message << "service_manager::register_creator: " 
									<< "Service ID '" << a_service_id << "' is already used "
									<< "within the factory dictionary !";
					if (is_abort_on_error ())
						{
							throw logic_error (message.str ());
						}
					else 
						{
							message << ' ' << "Overwriting current value...";
							clog << "ERROR: " 
									 << message.str () << endl;
							return datatools::utils::ERROR;
						}
				}
			cerr << "DEVEL: " 
					 << "datatools::service::service_manager::register_creator: "
					 << "Store it with ID '" << a_service_id << "'..."
					 << endl;
			creators_ [a_service_id] = a_service_creator;
			return datatools::utils::SUCCESS;
		}

		void service_manager::tree_dump (ostream & a_out , 
																		 const string & a_title,
																		 const string & a_indent,
																		 bool a_inherit) const
		{
			namespace du = datatools::utils;
			string indent;
			if (! a_indent.empty ())
				{
					indent = a_indent;
				}
			if ( ! a_title.empty () ) 
				{
					a_out << indent << a_title << endl;
				}  
			a_out << indent << du::i_tree_dumpable::tag 
						<< "Name           : '" << name_ << "'" << endl;
			a_out << indent << du::i_tree_dumpable::tag 
						<< "Description    : '" << description_ << "'" << endl;
			a_out << indent << du::i_tree_dumpable::tag 
						<< "Debug          : " << debug_ << endl;
			a_out << indent << du::i_tree_dumpable::tag 
						<< "Abort on error : " << abort_on_error_ << endl;
			a_out << indent << du::i_tree_dumpable::tag 
						<< "Preload        : " << preload_ << endl;
			a_out << indent << du::i_tree_dumpable::tag 
						<< "Initialized    : " << is_initialized () << endl;

			{
				a_out << indent << du::i_tree_dumpable::tag 
							<< "Creators       : ";
				size_t count = 0; 
				size_t sz = creators_.size ();
				if (sz == 0)
					{
						a_out << "<none>";
					}
				a_out << endl;
				for (service_creator_dict_type::const_iterator it = creators_.begin ();
						 it != creators_.end ();
						 it++) 
					{
						count++;
						a_out << indent << du::i_tree_dumpable::skip_tag; 
						if (count == sz) a_out <<  du::i_tree_dumpable::last_tag; 
						else a_out << du::i_tree_dumpable::tag ;
						a_out << "Service ID : '" << it->first << "' @ " << hex << (void *) it->second 
									<< dec << endl;
					}
			}

			{
				size_t count = 0; 
				a_out << indent << du::i_tree_dumpable::tag 
							<< "Services       : ";
				size_t sz = services_.size ();
				if (sz == 0)
					{
						a_out << "<none>";
					}
				a_out << endl;
				for (service_dict_type::const_iterator i = services_.begin ();
						 i != services_.end ();
						 i++)
					{
						const string & service_name = i->first;
						const service_entry & service_entry = i->second;
						a_out << indent << du::i_tree_dumpable::skip_tag;

						std::ostringstream indent_oss;
						indent_oss << indent << du::i_tree_dumpable::skip_tag;
						service_dict_type::const_iterator j = i; 
						j++;
						if (j == services_.end ()) 
							{
								a_out << du::i_tree_dumpable::last_tag;
								indent_oss << du::i_tree_dumpable::last_skip_tag;
							}
						else 
							{
								a_out << du::i_tree_dumpable::tag;
								indent_oss << du::i_tree_dumpable::skip_tag;
							}
						a_out << "Service : '" << service_name << "'" << std::endl;
						service_entry.tree_dump (a_out, "", indent_oss.str ());
					}				
			}

			a_out << indent << du::i_tree_dumpable::inherit_tag (a_inherit)
						<< "User resources : " << user_resources_ << endl;
				
			return;
		}
      
	}  // end of namespace service

}  // end of namespace datatools

// end of service_manager.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
