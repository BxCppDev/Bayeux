// -*- mode: c++; -*-
/* component_macros.h
 * Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2013-11-11
 * Last modified : 2013-11-13
 *
 * Copyright (C) 2013 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
#ifndef ELECTRONICS_COMPONENT_MACROS_H_
#define ELECTRONICS_COMPONENT_MACROS_H_

// Standard Library

// Third Party
// - Bayeux/datatools
#include <electronics/component_base.h>

#define ELECTRONICS_COMPONENT_CLASS_DECLARE(T)        \
  class T : public ::electronics::component_base \
  /**/


#define ELECTRONICS_COMPONENT_CONSTRUCTOR_DECLARE(T)  \
  public:                                               \
  T();                                                  \
  /**/


#define ELECTRONICS_COMPONENT_CONSTRUCTOR_IMPLEMENT_HEAD(T)   \
  T::T() : ::electronics::component_base)                \
  /**/


#define ELECTRONICS_COMPONENT_DESTRUCTOR_DECLARE(T)   \
  virtual ~T();                                         \
  /**/


#define ELECTRONICS_COMPONENT_DESTRUCTOR_IMPLEMENT(T) \
  T::~T() {                                             \
    if (this->is_initialized()) this->reset();          \
  }                                                     \
  /**/


#define ELECTRONICS_COMPONENT_INITIALIZE_DECLARE()            \
  public:                                                       \
  void initialize(const ::datatools::properties&,               \
                  ::electronics::component_pool_type&);  \
  /**/


#define ELECTRONICS_COMPONENT_INITIALIZE_IMPLEMENT_HEAD(T,CONF,POOL)  \
  void T::initialize(const ::datatools::properties& CONF,               \
                     ::electronics::component_pool_type& POOL)   \


#define ELECTRONICS_COMPONENT_RESET_DECLARE() \
  public:                                       \
  void reset();                                 \
  /**/


#define ELECTRONICS_COMPONENT_RESET_IMPLEMENT_HEAD(T) \
  void T::reset()                                       \
         /**/


#define ELECTRONICS_COMPONENT_INTERFACE(COMPONENT_CLASS_NAME)         \
                                                                        \
  public:                                                               \
                                                                        \
  ELECTRONICS_COMPONENT_CONSTRUCTOR_DECLARE(COMPONENT_CLASS_NAME);    \
                                                                        \
  ELECTRONICS_COMPONENT_DESTRUCTOR_DECLARE(COMPONENT_CLASS_NAME);     \
                                                                        \
  ELECTRONICS_COMPONENT_INITIALIZE_DECLARE();                         \
                                                                        \
  ELECTRONICS_COMPONENT_RESET_DECLARE();                              \
                                                                        \
  /**/

/** Registration */

#define ELECTRONICS_COMPONENT_REGISTRATION_INTERFACE(COMPONENT_CLASS_NAME) \
  private:                                                              \
  DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_INTERFACE(::electronics::component_base, COMPONENT_CLASS_NAME); \
  /**/

#define ELECTRONICS_COMPONENT_REGISTRATION_IMPLEMENT(COMPONENT_CLASS_NAME,COMPONENT_ID) \
  DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_IMPLEMENTATION (::electronics::component_base,COMPONENT_CLASS_NAME,COMPONENT_ID); \
  /**/


#endif // ELECTRONICS_COMPONENT_MACROS_H_
