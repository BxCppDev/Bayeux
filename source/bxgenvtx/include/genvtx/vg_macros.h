/* vg_macros.h
 * Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2012-04-24
 * Last modified : 2012-04-24
 *
 * Copyright (C) 2012 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 *   Macros to help build electromagnetic field classes.
 *
 * History:
 *
 */

/*! \file vg_macros.h
 * \brief Macros to automatically generate some parts of the electromagnetic 
 *        fields interface.
 *
 */

#ifndef __genvtx__vg_macros_h
#define __genvtx__vg_macros_h 1

#include <genvtx/i_vertex_generator.h>

#define GENVTX_VG_CLASS_DECLARE(T)            \
  class T : public genvtx::i_vertex_generator \
  /**/

#define GENVTX_VG_IS_INITIALIZED_DECLARE() \
  public:                                  \
  virtual bool is_initialized () const;    \
  /**/

#define GENVTX_VG_IS_INITIALIZED_IMPLEMENT_HEAD(T) \
  bool T::is_initialized () const                  \
  /**/

#define GENVTX_VG_HAS_NEXT_VERTEX_DECLARE() \
  public:                                   \
  virtual bool has_next_vertex () const;    \
  /**/

#define GENVTX_VG_HAS_NEXT_VERTEX_IMPLEMENT_HEAD(T) \
  bool T::has_next_vertex () const                  \
  /**/

#define GENVTX_VG_INITIALIZE_DECLARE()                                  \
  public:                                                               \
  virtual void initialize (const datatools::utils::properties &,        \
                           datatools::service::service_manager &,       \
                           genvtx::vg_dict_type &);                     \
  /**/

#define GENVTX_VG_INITIALIZE_IMPLEMENT_HEAD(T,CONF,SERVICE_MANAGER,DICT)     \
  void T::initialize (const datatools::utils::properties & CONF,             \
                      datatools::service::service_manager & SERVICE_MANAGER, \
                      genvtx::vg_dict_type & DICT)                           \
  /**/

#define GENVTX_VG_RESET_DECLARE() \
  public:                         \
  virtual void reset ();          \
  /**/

#define GENVTX_VG_RESET_IMPLEMENT_HEAD(T) \
  void T::reset ()                        \
  /**/
 
#define GENVTX_VG_SHOOT_VERTEX_DECLARE()                                             \
  protected :                                                                        \
  virtual void _shoot_vertex (mygsl::rng & random_, geomtools::vector_3d & vertex_); \
   /**/

#define GENVTX_VG_SHOOT_VERTEX_IMPLEMENT_HEAD(T,Random,Vertex)               \
  void T::_shoot_vertex (mygsl::rng & Random, geomtools::vector_3d & Vertex) \
  /**/

#define GENVTX_VG_CONSTRUCTOR_DECLARE(T) \
  public:                                \
  T ();                                  \
  /**/

#define GENVTX_VG_CONSTRUCTOR_IMPLEMENT_HEAD(T) \
  T::T ()                                       \
    : genvtx::i_vertex_generator ()             \
  /**/

#define GENVTX_VG_DESTRUCTOR_DECLARE(T) \
  public:                               \
  virtual ~T ();                        \
  /**/

#define GENVTX_VG_DESTRUCTOR_IMPLEMENT_HEAD(T) \
  T::~T ()                                     \
  /**/

#define GENVTX_VG_DEFAULT_DESTRUCTOR_IMPLEMENT(T) \
  GENVTX_VG_DESTRUCTOR_IMPLEMENT_HEAD (T)         \
  {                                               \
    if (is_initialized ()) reset ();              \
    return;                                       \
  }                                               \
  /**/

/*** Macro for the declaration of the minimal interface ***/
#define GENVTX_VG_INTERFACE()         \
  GENVTX_VG_IS_INITIALIZED_DECLARE(); \
  GENVTX_VG_INITIALIZE_DECLARE();     \
  GENVTX_VG_RESET_DECLARE();          \
  GENVTX_VG_SHOOT_VERTEX_DECLARE();   \
  public:                             \
  /**/

/*** Macro for the declaration of the minimal interface ***/
#define GENVTX_VG_INTERFACE_CTOR_DTOR(GENVTX_CLASS_NAME) \
  GENVTX_VG_CONSTRUCTOR_DECLARE(GENVTX_CLASS_NAME);      \
  GENVTX_VG_DESTRUCTOR_DECLARE(GENVTX_CLASS_NAME);       \
  GENVTX_VG_INTERFACE ();                                \
  /**/

/*** Macros for interface/implementation of static creator methods in EM field classes ***/

/** Registration */

#define GENVTX_VG_REGISTRATION_INTERFACE(GENVTX_CLASS_NAME)                                                       \
  private:                                                                                                       \
  DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_INTERFACE (genvtx::i_vertex_generator,GENVTX_CLASS_NAME); \
  /**/

#define GENVTX_VG_REGISTRATION_IMPLEMENT(GENVTX_CLASS_NAME,GENVTX_ID)                                                            \
  DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_IMPLEMENTATION (genvtx::i_vertex_generator,GENVTX_CLASS_NAME,GENVTX_ID); \
  /**/

#endif // __genvtx__vg_macros_h

// end of vg_macros.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
