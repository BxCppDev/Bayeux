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

      class service_manager;

      class base_service : public datatools::utils::i_tree_dumpable
      {
      public:
				
				const string & get_name () const;
				
				const string & get_description () const;
				
				void set_description (const string & a_description);
	
				bool has_version () const;

				const string & get_version () const;
	
				void set_version (const string & a_version);

				bool has_meta () const;

				virtual string get_meta () const;

				virtual void fetch_dependencies (service_dependency_dict_type &) const;

      public:

				bool has_service_manager () const;

				const service_manager & get_service_manager () const;

				void set_service_manager (service_manager & a_service_manager);
	
				virtual bool is_initialized () const = 0;

				virtual int initialize (const datatools::utils::properties & a_config,
																service_dict_type * a_service_dict = 0,
																void * a_user_resource = 0) = 0;

				virtual int reset () = 0;
    
      protected:

				void set_name_ (const string & a_name);
		  
      public: 

				// ctor:
				base_service (const string & a_process_name, 
											const string & a_process_description, 
											const string & a_process_version);
	
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

				service_manager * service_manager_; //!< Local reference to a service manager.
				
				/**************************************************/
			public:

				virtual string service_id () const;
				
				virtual service_creator_type service_creator () const;

				class service_creator_db
				{
					service_creator_dict_type  dict_;
					
				public:
					
					service_creator_db (bool = false);
					
					virtual ~service_creator_db ();
					
					const service_creator_dict_type & get_dict () const;
					
					service_creator_dict_type & get_dict ();
					
					bool has_service_creator (const string & service_id_) const;
					
					service_creator_type & get_service_creator (const string & service_id_);
					
					void register_service_creator (service_creator_type, const string & service_id_);
					
					void dump_service_creators (ostream & out_ = clog);
					
				}; // service_creator_db
				
				/**************************************************/
				
				typedef boost::scoped_ptr<service_creator_db> scoped_service_creator_db_t;
				
			private:

				static scoped_service_creator_db_t g_service_creator_db_;

			public:
				
				static service_creator_db & get_service_creator_db ();

				/**************************************************/

			protected:
				
				/* utility to enable auto-registering of some cut 
				 * in the global dictionary:
				 *
				 * The templatized class 'service_t' must inherits from 'base_service'
				 * and implements the following methods:
				 *  - string        service_id () const
				 *  - service_creator_type service_creator () const
				 *
				 */
				template <class service_t>
				class creator_registration
				{
					service_t service_;
					
				public:
					
					creator_registration ()
					{
						bool devel = g_debug;
						//devel = true;
						using namespace std;
						if (devel) clog << "DEVEL: base_service::creator_registration::ctor: "
														<< "Entering..."
														<< endl;
						string service_id = service_.service_id ();
						if (devel) clog << "DEVEL: base_service::creator_registration::ctor: "
														<< "service_id='" << service_id << "'"
														<< endl;
						
						service_creator_type service_creator = service_.service_creator ();
						if (devel) clog << "DEVEL: base_service::creator_registration::ctor: "
														<< "service_creator='" << hex << (void *) service_creator << dec << "'"
														<< endl;
						
						try
							{ 
								bool test = false;
								//test = true;
								if (! test)
									{
										if (devel) 
											{
												clog << "DEVEL: base_service::creator_registration::ctor: "
														 << "register_service_creator='" << service_id << " @ " 
														 << hex << (void *) service_creator << dec << "'"
														 << endl;
											}
										base_service::get_service_creator_db ().register_service_creator (service_creator,
																																											service_id);
									}
							}
						catch (exception & x)
							{
								cerr << "ERROR: base_service::creator_registration::ctor: " 
										 << x.what () << endl;
							}
						catch (...)
							{
								cerr << "ERROR: base_service::creator_registration::ctor: " 
										 << "unexpected!" << endl;
							}
						if (devel) clog << "DEVEL: base_service::creator_registration::ctor: "
														<< "Exiting."
														<< endl;
					}
					
				};

      };

    }  // end of namespace service

}  // end of namespace datatools

#define DATATOOLS_SERVICE_CLASS_DECLARE(T)					\
  class T : public datatools::service::base_service	\
  /**/

#define DATATOOLS_SERVICE_CONSTRUCTOR_DECLARE(T)	\
  public:																					\
  T ();																						\
  /**/

#define DATATOOLS_SERVICE_CONSTRUCTOR_IMPLEMENT_HEAD(T,Name,Desc,Ver)	\
  T::T ()																															\
    : datatools::service::base_service (Name,													\
																				Desc,													\
																				Ver)													\
      /**/

#define DATATOOLS_SERVICE_DESTRUCTOR_DECLARE(T)	\
  virtual ~T ();																\
  /**/

#define DATATOOLS_SERVICE_DESTRUCTOR_IMPLEMENT(T)	\
  T::~T ()																				\
  {																								\
    if (is_initialized ()) reset ();							\
    return;																				\
  }																								\
  /**/

#define DATATOOLS_SERVICE_IS_INITIALIZED_DECLARE()	\
  public:																						\
  bool is_initialized () const;											\
  /**/


#define DATATOOLS_SERVICE_IS_INITIALIZED_IMPLEMENT_HEAD(T)	\
  bool T::is_initialized () const														\
  /**/

#define DATATOOLS_SERVICE_INITIALIZE_DECLARE()					\
  public:																								\
  int initialize (const datatools::utils::properties &,	\
									service_dict_type * = 0,  						\
									void * = 0);													\
  /**/

#define DATATOOLS_SERVICE_INITIALIZE_IMPLEMENT_HEAD(T,CONF,DICT,USER)	\
  int T::initialize (const datatools::utils::properties & CONF,		\
										 service_dict_type * DICT,										\
										 void * USER)																	\

#define DATATOOLS_SERVICE_RESET_DECLARE()				\
  public:																				\
  int reset ();																	\
  /**/

#define DATATOOLS_SERVICE_RESET_IMPLEMENT_HEAD(T)	\
  int T::reset ()																	\
  /**/

#define DATATOOLS_SERVICE_GET_META_DECLARE()		\
  public:																				\
  std::string get_meta () const;								\
  /**/

#define DATATOOLS_SERVICE_GET_META_IMPLEMENT_HEAD(T)	\
  std::string T::get_meta () const										\
  /**/

#define DATATOOLS_SERVICE_FETCH_DEPENDENCIES_DECLARE()										\
  public:																																\
  void fetch_dependencies (datatools::service::service_dependency_dict_type &) const;	\
  /**/

#define DATATOOLS_SERVICE_FETCH_DEPENDENCIES_IMPLEMENT_HEAD(T,DEPENDENCIES)	\
  void T::fetch_dependencies (datatools::service::service_dependency_dict_type &DEPENDENCIES) const \
  /**/

#define DATATOOLS_SERVICE_INTERFACE(SERVICE_CLASS_NAME)	      \
																															\
  public:																											\
																															\
  DATATOOLS_SERVICE_CONSTRUCTOR_DECLARE(SERVICE_CLASS_NAME);	\
																															\
  DATATOOLS_SERVICE_DESTRUCTOR_DECLARE(SERVICE_CLASS_NAME);		\
																															\
  DATATOOLS_SERVICE_IS_INITIALIZED_DECLARE();									\
																															\
  DATATOOLS_SERVICE_INITIALIZE_DECLARE();											\
																															\
  DATATOOLS_SERVICE_RESET_DECLARE();													\
																															\
  /**/

#define DATATOOLS_SERVICE_CREATOR_INTERFACE()														\
public:																																\
 virtual std::string service_id () const;																\
 virtual datatools::service::service_creator_type service_creator () const; \
protected:																															\
 static datatools::service::base_service * g_service_creator_ (const datatools::utils::properties &, \
																															 service_dict_type * = 0, \
																															 void * = 0); \
 /**/

#define DATATOOLS_SERVICE_DEFAULT_CREATOR_INTERFACE()										\
public:																																\
 virtual std::string service_id () const;																\
 virtual datatools::service::service_creator_type service_creator () const; \
protected:																															\
 static datatools::service::base_service * g_default_service_creator_ (const datatools::utils::properties &, \
																																			 service_dict_type * = 0, \
																																			 void * = 0); \
 /**/

#define DATATOOLS_SERVICE_CREATOR_IMPLEMENT_HEAD(SERVICE_CLASS_NAME,SERVICE_ID,CONF,DICT,USER) \
	std::string SERVICE_CLASS_NAME::service_id () const										\
	{																																			\
		return SERVICE_ID;																									\
  }																																			\
  datatools::service::service_creator_type SERVICE_CLASS_NAME::service_creator () const \
  {																																			\
	  return SERVICE_CLASS_NAME::g_service_creator_;											\
  }																																			\
  datatools::service::base_service * SERVICE_CLASS_NAME::g_service_creator_ (const datatools::utils::properties & CONF, \
																																								service_dict_type * DICT, \
																																								void * USER) \
	/**/

#define DATATOOLS_SERVICE_DEFAULT_CREATOR_IMPLEMENT(SERVICE_CLASS_NAME,SERVICE_ID,CONF,DICT,USER) \
	std::string SERVICE_CLASS_NAME::service_id () const										\
	{																																			\
		return SERVICE_ID;																									\
  }																																			\
  datatools::service::service_creator_type SERVICE_CLASS_NAME::service_creator () const \
  {																																			\
	  return SERVICE_CLASS_NAME::g_default_service_creator_;							\
  }																																			\
	datatools::service::base_service * SERVICE_CLASS_NAME::g_default_service_creator_ (const datatools::utils::properties & CONF, \
																																												service_dict_type * DICT, \
																																												void * USER) \
	{																																			\
		SERVICE_CLASS_NAME * ptr = new SERVICE_CLASS_NAME;									\
		ptr->initialize (a_configuration, a_dictionnary, a_user);						\
		return ptr;																													\
	}																																			\
	/**/

#define DATATOOLS_SERVICE_CREATOR_REGISTRATION_INTERFACE(SERVICE_CLASS_NAME)		\
  private:																															\
	static creator_registration<SERVICE_CLASS_NAME> this_service_registration_; \
	/**/

#define DATATOOLS_SERVICE_CREATOR_REGISTRATION_IMPLEMENT(SERVICE_CLASS_NAME)		\
  base_service::creator_registration<SERVICE_CLASS_NAME> SERVICE_CLASS_NAME::this_service_registration_; \
	/**/

#endif // __datatools__service__base_service_h

// end of base_service.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
