/* cut_macros.h
 * Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-06-07
 * Last modified : 2013-12-14
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
 *   Macros to help build cut classes.
 *
 * History:
 *
 */

#ifndef CUTS_CUT_MACROS_H_
#define CUTS_CUT_MACROS_H_ 1

#include <cuts/i_cut.h>

#define CUT_CLASS_DECLARE(T)                    \
  class T : public ::cuts::i_cut                \
  /**/

#define CUT_INITIALIZE_DECLARE()                          \
  public:                                                 \
  virtual void initialize(const datatools::properties &,  \
                          datatools::service_manager &,   \
                          cuts::cut_handle_dict_type &);  \
  /**/

#define CUT_INITIALIZE_IMPLEMENT_HEAD(T,Config,ServiceManager,CutDict)  \
  void T::initialize(const datatools::properties & Config,              \
                     datatools::service_manager & ServiceManager,       \
                     cuts::cut_handle_dict_type & CutDict)              \
  /**/

#define CUT_RESET_DECLARE()                     \
  public:                                       \
  virtual void reset();                         \
  /**/

#define CUT_RESET_IMPLEMENT_HEAD(T)             \
  void T::reset()                               \
  /**/

#define CUT_ACCEPT_DECLARE()                    \
  protected :                                   \
  virtual int _accept();                        \
  /**/

#define CUT_ACCEPT_IMPLEMENT_HEAD(T)            \
  int T::_accept()                              \
  /**/

#define CUT_CONSTRUCTOR_DECLARE(T)                                    \
  public:                                                             \
  T(::datatools::logger::priority = ::datatools::logger::PRIO_FATAL); \
  /**/

#define CUT_CONSTRUCTOR_IMPLEMENT_HEAD(T,LoggingPriority) \
  T::T(::datatools::logger::priority LoggingPriority)     \
    : ::cuts::i_cut(LoggingPriority)                      \
      /**/

#define CUT_DESTRUCTOR_DECLARE(T)               \
  public:                                       \
  virtual ~T();                                 \
  /**/

#define CUT_DESTRUCTOR_IMPLEMENT_HEAD(T)        \
  T::~T()                                       \
  /**/

#define CUT_RESET_IMPLEMENT_HEAD(T)             \
  void T::reset()                               \
  /**/

#define CUT_DEFAULT_DESTRUCTOR_IMPLEMENT(T)     \
  CUT_DESTRUCTOR_IMPLEMENT_HEAD(T)              \
  {                                             \
    if (is_initialized()) this->T::reset();     \
    return;                                     \
  }                                             \
  /**/

/*** Macro for the declaration of the full cut interface ***/
#define CUT_INTERFACE()                         \
  CUT_INITIALIZE_DECLARE();                     \
  CUT_RESET_DECLARE();                          \
  CUT_ACCEPT_DECLARE();                         \
  /**/

/*** Macro for the declaration of the full cut interface, including ctor/dtor ***/
#define CUT_INTERFACE_CTOR_DTOR(T)              \
  CUT_CONSTRUCTOR_DECLARE(T);                   \
  CUT_DESTRUCTOR_DECLARE(T);                    \
  CUT_INTERFACE();                              \
  /**/

/*** Macro for the declaration of the full cut interface, including ctor/dtor, excluding reset ***/
#define CUT_INTERFACE_NORESET_CTOR_DTOR(T)      \
  CUT_CONSTRUCTOR_DECLARE(T);                   \
  CUT_DESTRUCTOR_DECLARE(T);                    \
  CUT_INITIALIZE_DECLARE();                     \
  CUT_ACCEPT_DECLARE();                         \
  /**/

/*** Macro for the declaration of the full cut interface, including ctor/dtor, excluding reset ***/
#define CUT_INTERFACE_NOACCEPT_CTOR_DTOR(T)     \
  CUT_CONSTRUCTOR_DECLARE(T);                   \
  CUT_DESTRUCTOR_DECLARE(T);                    \
  CUT_INITIALIZE_DECLARE();                     \
  CUT_RESET_DECLARE();                          \
  /**/

/*** Macro for the declaration of the full cut interface, including ctor/dtor, excluding init and reset ***/
#define CUT_INTERFACE_NOINIT_NORESET_CTOR_DTOR(T) \
  CUT_CONSTRUCTOR_DECLARE(T);                     \
  CUT_DESTRUCTOR_DECLARE(T);                      \
  CUT_ACCEPT_DECLARE();                           \
  /**/

#endif // CUTS_CUT_MACROS_H_

// end of cut_macros.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
