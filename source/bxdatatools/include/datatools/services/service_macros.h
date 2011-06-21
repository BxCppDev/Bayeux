/* service_macros.h
 * Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-06-07
 * Last modified : 2011-06-20
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
 *   Macros.
 * 
 * History: 
 * 
 */

#define DATATOOLS_SERVICE_CLASS_DECLARE(T)		\
   class T : public datatools::service::base_service	\
     /**/
  
#define DATATOOLS_SERVICE_CONSTRUCTOR_DECLARE(T)			\
   public:								\
     T ();								\
   /**/
  
#define DATATOOLS_SERVICE_CONSTRUCTOR_IMPLEMENT_HEAD(T,Name,Desc,Ver)	\
  T::T ()					\
    : datatools::service::base_service (Name,				\
					Desc,				\
					Ver)				\
    /**/
  
#define DATATOOLS_SERVICE_DESTRUCTOR_DECLARE(T)	\
  virtual ~T ();				\
  /**/

#define DATATOOLS_SERVICE_DESTRUCTOR_IMPLEMENT(T)			\
  T::~T ()								\
  {									\
    if (is_initialized ()) reset ();					\
    return;								\
  }									\
  /**/

#define DATATOOLS_SERVICE_IS_INITIALIZED_DECLARE()			\
  public:								\
  bool is_initialized () const;						\
  /**/


#define DATATOOLS_SERVICE_IS_INITIALIZED_IMPLEMENT_HEAD(T)		\
  bool T::is_initialized () const					\
  /**/

#define DATATOOLS_SERVICE_INITIALIZE_DECLARE()				\
  public:								\
  int initialize (const datatools::utils::properties &,			\
									service_dict_type &);											\
  /**/

#define DATATOOLS_SERVICE_INITIALIZE_IMPLEMENT_HEAD(T,CONF,DICT)	\
  int T::initialize (const datatools::utils::properties & CONF,		\
		     service_dict_type & DICT)				\
    
#define DATATOOLS_SERVICE_RESET_DECLARE()				\
  public:								\
  int reset ();								\
  /**/

#define DATATOOLS_SERVICE_RESET_IMPLEMENT_HEAD(T)			\
  int T::reset ()							\
  /**/

#define DATATOOLS_SERVICE_INTERFACE(SERVICE_CLASS_NAME)			\
  									\
  public:								\
  									\
  DATATOOLS_SERVICE_CONSTRUCTOR_DECLARE(SERVICE_CLASS_NAME);		\
  									\
  DATATOOLS_SERVICE_DESTRUCTOR_DECLARE(SERVICE_CLASS_NAME);		\
  									\
  DATATOOLS_SERVICE_IS_INITIALIZED_DECLARE();				\
  									\
  DATATOOLS_SERVICE_INITIALIZE_DECLARE();				\
  									\
  DATATOOLS_SERVICE_RESET_DECLARE();					\
  									\
  /**/

/** Registration */

#define DATATOOLS_SERVICE_REGISTRATION_INTERFACE(SERVICE_CLASS_NAME)	\
	private:																															\
	static datatools::service::default_creator_registration<SERVICE_CLASS_NAME> \
	g_default_registration_;																							\
	/**/

#define DATATOOLS_SERVICE_REGISTRATION_IMPLEMENT(SERVICE_CLASS_NAME,SERVICE_ID) \
	datatools::service::default_creator_registration<SERVICE_CLASS_NAME>  \
	SERVICE_CLASS_NAME::g_default_registration_ (SERVICE_ID);							\
	/**/


// end of service_macros.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
