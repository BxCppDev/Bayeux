/* module_macros.h
 * Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-06-07
 * Last modified : 2013-05-15
 *
 * Copyright (C) 2011-2013 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 * Description:
 *
 *   Macros to help build module classes.
 *
 * History:
 *
 */

/*! \file module_macros.h
 * \brief Macros to automatically generate some parts of the data processing module interface.
 *
 */

#ifndef DPP_MODULE_MACROS_H_
#define DPP_MODULE_MACROS_H_ 1

#include <dpp/base_module.h>

#define DPP_MODULE_CLASS_DECLARE(T)             \
  class T : public ::dpp::base_module           \
  /**/

#define DPP_MODULE_INITIALIZE_DECLARE()                       \
  public:                                                     \
  virtual void initialize(const ::datatools::properties &,    \
                          ::datatools::service_manager &,     \
                          ::dpp::module_handle_dict_type &);  \
  /**/

#define DPP_MODULE_INITIALIZE_IMPLEMENT_HEAD(T,Setup,ServiceManager,ModuleDict) \
  void T::initialize(const ::datatools::properties & Setup,             \
                     ::datatools::service_manager & ServiceManager,     \
                     ::dpp::module_handle_dict_type & ModuleDict)       \
  /**/

#define DPP_MODULE_RESET_DECLARE()              \
  public:                                       \
  virtual void reset();                         \
  /**/

#define DPP_MODULE_RESET_IMPLEMENT_HEAD(T)      \
  void T::reset()                               \
  /**/

#define DPP_MODULE_PROCESS_DECLARE()                                    \
  public :                                                              \
  virtual dpp::base_module::process_status process(::datatools::things &); \
  /**/

#define DPP_MODULE_PROCESS_IMPLEMENT_HEAD(T,DataRecord)     \
  dpp::base_module::process_status                          \
  T::process(::datatools::things & DataRecord)              \
  /**/

#define DPP_MODULE_CONSTRUCTOR_DECLARE(T)                             \
  public:                                                             \
  T(::datatools::logger::priority = ::datatools::logger::PRIO_FATAL); \
  /**/

#define DPP_MODULE_CONSTRUCTOR_IMPLEMENT_HEAD(T,LoggingPriority)  \
  T::T(::datatools::logger::priority LoggingPriority)             \
    : ::dpp::base_module::base_module(LoggingPriority)            \
      /**/

#define DPP_MODULE_DESTRUCTOR_DECLARE(T)        \
  public:                                       \
  virtual ~T();                                 \
  /**/

#define DPP_MODULE_DESTRUCTOR_IMPLEMENT_HEAD(T) \
  T::~T()                                       \
  /**/

#define DPP_MODULE_DEFAULT_DESTRUCTOR_IMPLEMENT(T)  \
  DPP_MODULE_DESTRUCTOR_IMPLEMENT_HEAD(T)           \
  {                                                 \
    if (is_initialized()) T::reset();               \
    return;                                         \
  }                                                 \
  /**/

/*** Macro for the declaration of the full module interface ***/
#define DPP_MODULE_INTERFACE()                  \
  DPP_MODULE_INITIALIZE_DECLARE();              \
  DPP_MODULE_RESET_DECLARE();                   \
  DPP_MODULE_PROCESS_DECLARE();                 \
  /**/

/*** Macro for the declaration of the full module interface ***/
#define DPP_MODULE_INTERFACE_CTOR_DTOR(T)       \
  DPP_MODULE_CONSTRUCTOR_DECLARE(T);            \
  DPP_MODULE_DESTRUCTOR_DECLARE(T);             \
  DPP_MODULE_INTERFACE();                       \
  /**/

/*** Macros for interface/implementation of static creator methods in module classes ***/

/** Registration */

#define DPP_MODULE_REGISTRATION_INTERFACE(T)                            \
  private:                                                              \
  DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_INTERFACE(::dpp::base_module,T); \
  /**/

#define DPP_MODULE_REGISTRATION_IMPLEMENT(T,ModuleID)                   \
  DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_IMPLEMENTATION(::dpp::base_module,T,ModuleID); \
  /**/

#endif // DPP_MODULE_MACROS_H_

// end of module_macros.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
