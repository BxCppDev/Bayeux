/// \file mctools/detail/step_hit_processor_macros.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2012-05-03
 * Last modified : 2014-04-23
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
 *  Factory registration macros for step hit processor classes.
 *
 * History:
 *
 */

#ifndef MCTOOLS_DETAIL_STEP_HIT_PROCESSOR_H
#define MCTOOLS_DETAIL_STEP_HIT_PROCESSOR_H 1

// This project:
#include <mctools/base_step_hit_processor.h>

#define MCTOOLS_STEP_HIT_PROCESSOR_REGISTRATION_INTERFACE(SHP_CLASS_NAME) \
  private:                                                              \
  DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_INTERFACE (::mctools::base_step_hit_processor,SHP_CLASS_NAME) \
  /**/

#define MCTOOLS_STEP_HIT_PROCESSOR_REGISTRATION_IMPLEMENT(SHP_CLASS_NAME,SHP_ID) \
  DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_IMPLEMENTATION (::mctools::base_step_hit_processor,SHP_CLASS_NAME,SHP_ID) \
  /**/

#endif // MCTOOLS_DETAIL_STEP_HIT_PROCESSOR_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
