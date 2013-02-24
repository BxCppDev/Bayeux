// -*- mode: c++ ; -*-
/* manager_macros.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2013-02-21
 * Last modified: 2013-02-21
 */

#ifndef GEOMTOOLS_MANAGER_MACROS_H_
#define GEOMTOOLS_MANAGER_MACROS_H_ 1

#define GEOMTOOLS_PLUGIN_CLASS_DECLARE(T)		\
  class T : public ::geomtools::manager::base_plugin	\
  /**/


#define GEOMTOOLS_PLUGIN_CONSTRUCTOR_DECLARE(T)	\
  public:					\
  T();						\
  /**/


#define GEOMTOOLS_PLUGIN_CONSTRUCTOR_IMPLEMENT_HEAD(T)	\
  T::T() : ::geomtools::manager::base_plugin()		\
  /**/


#define GEOMTOOLS_PLUGIN_DESTRUCTOR_DECLARE(T)	\
  virtual ~T();					\
  /**/


#define GEOMTOOLS_PLUGIN_DESTRUCTOR_IMPLEMENT(T)	\
  T::~T() {						\
    if (this->is_initialized()) this->reset();		\
  }							\
  /**/


#define GEOMTOOLS_PLUGIN_IS_INITIALIZED_DECLARE()	\
  public:						\
  bool is_initialized() const;				\
  /**/


#define GEOMTOOLS_PLUGIN_IS_INITIALIZED_IMPLEMENT_HEAD(T)	\
  bool T::is_initialized() const				\
  /**/


#define GEOMTOOLS_PLUGIN_INITIALIZE_DECLARE()			\
  public:							\
  int initialize(const ::datatools::properties&,		\
		 ::geomtools::manager::plugins_dict_type&,		\
		 const ::datatools::service_dict_type & services_);	\
  /**/


#define GEOMTOOLS_PLUGIN_INITIALIZE_IMPLEMENT_HEAD(T,CONF,PLUGINS,SERVICES) \
  int T::initialize(const ::datatools::properties & CONF,		\
		    ::geomtools::manager::plugins_dict_type & PLUGINS,	\
		    const ::datatools::service_dict_type & SERVICES)	\


#define GEOMTOOLS_PLUGIN_RESET_DECLARE()	\
  public:					\
  int reset();					\
  /**/


#define GEOMTOOLS_PLUGIN_RESET_IMPLEMENT_HEAD(T)	\
  int T::reset()					\
	/**/


#define GEOMTOOLS_PLUGIN_INTERFACE(PLUGIN_CLASS_NAME)		\
								\
  public:							\
								\
  GEOMTOOLS_PLUGIN_CONSTRUCTOR_DECLARE(PLUGIN_CLASS_NAME);	\
								\
  GEOMTOOLS_PLUGIN_DESTRUCTOR_DECLARE(PLUGIN_CLASS_NAME);	\
								\
  GEOMTOOLS_PLUGIN_IS_INITIALIZED_DECLARE();			\
								\
  GEOMTOOLS_PLUGIN_INITIALIZE_DECLARE();			\
								\
  GEOMTOOLS_PLUGIN_RESET_DECLARE();				\
								\
  /**/

/** Registration */

#define GEOMTOOLS_PLUGIN_REGISTRATION_INTERFACE(PLUGIN_CLASS_NAME)	\
  private:								\
  DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_INTERFACE(::geomtools::manager::base_plugin,PLUGIN_CLASS_NAME); \
  /**/

#define GEOMTOOLS_PLUGIN_REGISTRATION_IMPLEMENT(PLUGIN_CLASS_NAME,PLUGIN_ID) \
  DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_IMPLEMENTATION (::geomtools::manager::base_plugin,PLUGIN_CLASS_NAME,PLUGIN_ID); \
  /**/


#endif // GEOMTOOLS_MANAGER_MACROS_H_

// end of manager_macros.h
