//! \file datatools/command_utils.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2014-09-25
 * Last modified : 2016-06-23
 *
 * Copyright (C) 2014-2016 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 *  Command utilities.
 *
 */
#ifndef DATATOOLS_DETAILS_COMMAND_MACROS_H
#define DATATOOLS_DETAILS_COMMAND_MACROS_H

// Standard library:
#include <sstream>

// This project:
#include <datatools/command_utils.h>

/*! Build a command::returned_info object with proper error code and
  comprehensive message.

  This macro is intended to simplify the common use case of initializing
  command::returned_info objects when some error is detected.
  This macro takes three arguments as follows

  @param ReturnedInfo  The command::returned_info target object
  @param ErrorCode     The error code (as a command::error_code_type value)
  @param ErrorMessage  Message to supply to the  command::returned_info object,
  this must take the form of a string or output stream
  sequence

  In the simplest case it may be used as

  @code
  command::returned_error cri;
  DT_COMMAND_RETURNED_ERROR(cri, command::CEC_FAILURE, "parameter i is negative!");
  @endcode

  If Message is composed from several streamable objects, it can
  be composed using the streaming operator:

  @code
  command::returned_error cri;
  DT_COMMAND_RETURNED_ERROR(cri, command::CEC_FAILURE, "Operation '" << "compute"  << "' failed!");
  @endcode
*/
#define DT_COMMAND_RETURNED_ERROR(ReturnedInfo, ErrorCode, ErrorMessage) \
  {                                                                     \
    std::ostringstream s;                                               \
    s << "[" << BOOST_CURRENT_FUNCTION << ":" << __LINE__ << ": " << ErrorMessage << "]"; \
    ReturnedInfo.set_failure(ErrorCode, s.str());                       \
  }                                                                     \
  /**/

/*! Build a command::returned_info object with proper output message.

  This macro is intended to simplify the common use case of initializing
  command::returned_info objects when no error is detected.
  This macro takes two arguments as follows

  @param ReturnedInfo  The command::returned_info target object
  @param OutputMessage  Ouptut to supply to the command::returned_info object,
  this must take the form of a string or output stream
  sequence

  In the simplest case it may be used as

  @code
  command::returned_error cri;
  DT_COMMAND_RETURNED_SUCCESS(cri, "this command was successful!");
  @endcode

  If Message is composed from several streamable objects, it can
  be composed using the streaming operator:

  @code
  command::returned_error cri;
  DT_COMMAND_RETURNED_SUCCESS(cri, "Operation '" << "compute"  << "' was successful!");
  @endcode
*/
#define DT_COMMAND_RETURNED_SUCCESS(ReturnedInfo, OutputMessage)        \
  {                                                                     \
    std::ostringstream s;                                               \
    s << OutputMessage;                                                 \
    ReturnedInfo.set_success(s.str());                                  \
  }                                                                     \
  /**/

#endif // DATATOOLS_DETAILS_COMMAND_MACROS_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
