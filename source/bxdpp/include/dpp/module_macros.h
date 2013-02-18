/* module_macros.h
 * Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-06-07
 * Last modified : 2013-02-15
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
  virtual void initialize (const ::DPP_DU::properties &,      \
                           ::DPP_DS::service_manager &,       \
                           ::dpp::module_handle_dict_type &); \
  /**/

#define DPP_MODULE_INITIALIZE_IMPLEMENT_HEAD(T,CONF,SERVICE_MANAGER,DICT) \
  void T::initialize (const ::DPP_DU::properties & CONF,                \
                      ::DPP_DS::service_manager & SERVICE_MANAGER,      \
                      ::dpp::module_handle_dict_type & DICT)            \
  /**/

#define DPP_MODULE_RESET_DECLARE()              \
  public:                                       \
  virtual void reset ();                        \
  /**/

#define DPP_MODULE_RESET_IMPLEMENT_HEAD(T)      \
  void T::reset ()                              \
  /**/

#define DPP_MODULE_PROCESS_DECLARE()            \
  public :                                      \
  virtual int process (DPP_DU::things &);       \
  /**/

#define DPP_MODULE_PROCESS_IMPLEMENT_HEAD(T,EventRecord)  \
  int T::process (DPP_DU::things & EventRecord)           \
  /**/

#define DPP_MODULE_CONSTRUCTOR_DECLARE(T)       \
  public:                                       \
  T (int a_debug_level = ::dpp::NO_DEBUG);      \
  /**/

#define DPP_MODULE_CONSTRUCTOR_IMPLEMENT_HEAD(T,DebugLevel,Name,Description,Version) \
  T::T (int DebugLevel)                                                 \
    : base_module (Name,                                                \
                   Description,                                         \
                   Version,                                             \
                   DebugLevel)                                          \
      /**/

#define DPP_MODULE_DESTRUCTOR_DECLARE(T)        \
  public:                                       \
  virtual ~T ();                                \
  /**/

#define DPP_MODULE_DESTRUCTOR_IMPLEMENT_HEAD(T) \
  T::~T ()                                      \
  /**/

#define DPP_MODULE_DEFAULT_DESTRUCTOR_IMPLEMENT(T)  \
  DPP_MODULE_DESTRUCTOR_IMPLEMENT_HEAD (T)          \
  {                                                 \
    if (is_initialized ()) T::reset ();             \
    return;                                         \
  }                                                 \
  /**/

/*** Macro for the declaration of the full module interface ***/
#define DPP_MODULE_INTERFACE()                  \
                                                \
  DPP_MODULE_INITIALIZE_DECLARE();              \
                                                \
  DPP_MODULE_RESET_DECLARE();                   \
                                                \
  DPP_MODULE_PROCESS_DECLARE();                 \
                                                \
  /**/

/*** Macro for the declaration of the full module interface ***/
#define DPP_MODULE_INTERFACE_CTOR_DTOR(MODULE_CLASS_NAME) \
                                                          \
  DPP_MODULE_CONSTRUCTOR_DECLARE(MODULE_CLASS_NAME);      \
                                                          \
  DPP_MODULE_DESTRUCTOR_DECLARE(MODULE_CLASS_NAME);       \
                                                          \
  DPP_MODULE_INTERFACE ();                                \
                                                          \
  /**/

/*** Macros for interface/implementation of static creator methods in module classes ***/

/** Registration */

#define DPP_MODULE_REGISTRATION_INTERFACE(MODULE_CLASS_NAME)            \
  private:                                                              \
  DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_INTERFACE (::dpp::base_module,MODULE_CLASS_NAME); \
  /**/

#define DPP_MODULE_REGISTRATION_IMPLEMENT(MODULE_CLASS_NAME,MODULE_ID)  \
  DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_IMPLEMENTATION (::dpp::base_module,MODULE_CLASS_NAME,MODULE_ID); \
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
