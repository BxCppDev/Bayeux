/// \file genvtx/vg_macros.h
/// \brief Macros to automatically register vertex generator class.
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2012-04-24
 * Last modified : 2014-04-09
 *
 * Copyright (C) 2012-2014 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 *   Macros to register vertex generator classes.
 *
 * History:
 *
 */

#ifndef GENVTX_VG_MACROS_H
#define GENVTX_VG_MACROS_H 1

// This project:
#include <genvtx/i_vertex_generator.h>

/*** Macros for registration of static creator methods for vertex generator classes ***/

/** Registration */

#define GENVTX_VG_REGISTRATION_INTERFACE(GENVTX_CLASS_NAME)             \
  private:                                                              \
  DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_INTERFACE (::genvtx::i_vertex_generator,GENVTX_CLASS_NAME); \
  /**/

#define GENVTX_VG_REGISTRATION_IMPLEMENT(GENVTX_CLASS_NAME,GENVTX_ID)   \
  DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_IMPLEMENTATION (::genvtx::i_vertex_generator,GENVTX_CLASS_NAME,GENVTX_ID); \
  /**/

#endif // GENVTX_VG_MACROS_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
