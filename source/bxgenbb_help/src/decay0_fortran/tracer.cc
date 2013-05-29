// -*- mode: c++; -*-
// tracker.cc
/*
 * Copyright 2007-2013 F. Mauger
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
 */

#include <genbb_help/decay0_fortran/tracer.h>
#include <datatools/tracer.h>

/* The following function is invoked from Fortran
 * using special mangling name.
 */

extern "C"
{
  void tracer_trace (int32_t * id_, float * value_)
  {
    DT_TRACER_TRACE(*id_, *value_);
  }

  void tracer_trace_msg (int32_t * id_, char * word_)
  {
    DT_TRACER_MESSAGE(*id_, word_);
  }

  void tracer_trace_tag (int32_t * id_, char * word_, float * value_)
  {
    DT_TRACER_MESSAGE(*id_, word_ << " = " << *value_);
  }

}

// end of tracker.cc
