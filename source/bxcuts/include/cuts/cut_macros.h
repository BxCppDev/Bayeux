/* cut_macros.h
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
 * Description: 
 *
 *   Macros to help build cut classes.
 * 
 * History: 
 * 
 */

#ifndef __cuts__cut_macros_h
#define __cuts__cut_macros_h 1

#include <cuts/i_cut.h>

#define CUT_CLASS_DECLARE(T)                    \
  class T : public cuts::i_cut                  \
  /**/

#define CUT_INITIALIZE_DECLARE()                                  \
  public:                                                         \
  virtual void initialize (const datatools::utils::properties &,  \
                           datatools::service::service_manager &, \
                           cuts::cut_handle_dict_type &);         \
  /**/

#define CUT_INITIALIZE_IMPLEMENT_HEAD(T,CONF,SERVICE_MANAGER,DICT)      \
  void T::initialize (const datatools::utils::properties & CONF,        \
                      datatools::service::service_manager & SERVICE_MANAGER, \
                      cuts::cut_handle_dict_type & DICT)                \
  /**/

#define CUT_RESET_DECLARE()                     \
  public:                                       \
  virtual void reset ();                        \
  /**/

#define CUT_RESET_IMPLEMENT_HEAD(T)             \
  void T::reset ()                              \
  /**/

#define CUT_ACCEPT_DECLARE()                                   \
  protected :                                                  \
  virtual int _accept ();                                      \
  /**/

#define CUT_ACCEPT_IMPLEMENT_HEAD(T)            \
  int T::_accept ()                             \
  /**/

#define CUT_CONSTRUCTOR_DECLARE(T)                      \
  public:                                               \
  T (int a_debug_level = 0);                            \
  /**/

#define CUT_CONSTRUCTOR_IMPLEMENT_HEAD(T,DebugLevel,Name,Description,Version) \
  T::T (int DebugLevel)                                                 \
    : cuts::i_cut (Name,                                                \
                   Description,                                         \
                   Version,                                             \
                   DebugLevel)                                          \
  /**/

#define CUT_DESTRUCTOR_DECLARE(T)               \
  public:                                       \
  virtual ~T ();                                \
  /**/

#define CUT_DESTRUCTOR_IMPLEMENT_HEAD(T)        \
  T::~T ()                                      \
  /**/

#define CUT_RESET_IMPLEMENT_HEAD(T)             \
  void T::reset ()                              \
  /**/

#define CUT_DEFAULT_DESTRUCTOR_IMPLEMENT(T)     \
  CUT_DESTRUCTOR_IMPLEMENT_HEAD (T)             \
  {                                             \
    if (is_initialized ()) this->T::reset ();   \
    return;                                     \
  }                                             \
  /**/

/*** Macro for the declaration of the full cut interface ***/
#define CUT_INTERFACE()                         \
                                                \
  CUT_INITIALIZE_DECLARE();                     \
                                                \
  CUT_RESET_DECLARE();                          \
                                                \
  CUT_ACCEPT_DECLARE();                         \
                                                \
  /**/  

/*** Macro for the declaration of the full cut interface, including ctor/dtor ***/
#define CUT_INTERFACE_CTOR_DTOR(CUT_CLASS_NAME) \
                                                \
  CUT_CONSTRUCTOR_DECLARE(CUT_CLASS_NAME);      \
                                                \
  CUT_DESTRUCTOR_DECLARE(CUT_CLASS_NAME);       \
                                                \
  CUT_INTERFACE ();                             \
                                                \
  /**/  

/*** Macro for the declaration of the full cut interface, including ctor/dtor, excluding reset ***/
#define CUT_INTERFACE_NORESET_CTOR_DTOR(CUT_CLASS_NAME) \
                                                \
  CUT_CONSTRUCTOR_DECLARE(CUT_CLASS_NAME);      \
                                                \
  CUT_DESTRUCTOR_DECLARE(CUT_CLASS_NAME);       \
                                                \
  CUT_INITIALIZE_DECLARE();                     \
                                                \
  CUT_ACCEPT_DECLARE();                         \
                                                \
  /**/  

/*** Macros for interface/implementation of static creator methods in cut classes ***/

/** Registration */

#define CUT_REGISTRATION_INTERFACE(CUT_CLASS_NAME)                              \
  private:                                                                      \
  DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_INTERFACE (::cuts::i_cut,CUT_CLASS_NAME); \
  /**/

#define CUT_REGISTRATION_IMPLEMENT(CUT_CLASS_NAME,CUT_ID)                                   \
  DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_IMPLEMENTATION (::cuts::i_cut,CUT_CLASS_NAME,CUT_ID); \
  /**/

#endif // __cuts__cut_macros_h

// end of cut_macros.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
