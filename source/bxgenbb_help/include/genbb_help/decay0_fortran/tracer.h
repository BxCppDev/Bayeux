// -*- mode: c++; -*-
/* tracer.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2013-05-25
 * Last modified: 2013-05-25
 *
 * License:
 * Copyright 2013 F. Mauger
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
 *   Manager for GENBB input data
 *
 * History:
 *
 */

#ifndef GENBB_HELP_DECAY0_FORTRAN_TRACER_H_
#define GENBB_HELP_DECAY0_FORTRAN_TRACER_H_

#include <string>
#include <boost/cstdint.hpp>

#include <genbb_help/decay0_fortran/__genbb_help_FC.h>

extern "C"
{
  void tracer_trace (int32_t * id_, float * value_);
  void tracer_trace_msg (int32_t * id_, char * word_);
  void tracer_trace_tag (int32_t * id_, char * word_, float * value_);
}

#endif // GENBB_HELP_DECAY0_FORTRAN_TRACER_H_

// end of genbb_mgr.h
