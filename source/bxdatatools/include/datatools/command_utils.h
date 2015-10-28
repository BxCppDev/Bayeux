//! \file datatools/command_utils.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2014-09-25
 * Last modified : 2014-09-25
 *
 * Copyright (C) 2014 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
#ifndef DATATOOLS_COMMAND_UTILS_H
#define DATATOOLS_COMMAND_UTILS_H

// Standard library:
#include <string>
#include <iostream>

namespace datatools {

  /// \brief Generic command utilities
  class command
  {
  public:

    /// \brief Command error codes
    enum error_code_type {
      CEC_STOP                         = -2, //!< Stop without error (can be used to stop a loop or a pipeline)
      CEC_CONTINUE                     = -1, //!< Continue without error (can be used to continue a loop or a pipeline)
      CEC_SUCCESS                      =  0, //!< Generic success
      CEC_FAILURE                      =  1, //!< Generic failure
      CEC_PARSING_FAILURE              =  2, //!< Generic parsing error
      CEC_CONTEXT_INVALID              =  3, //!< Invalid context
      CEC_ABORT                        =  9, //!< Abort (can be used to abort a loop or a pipeline)
      CEC_SCOPE_INVALID                = 10, //!< Invalid scope (wrong namespace or general context)
      CEC_COMMAND_INVALID              = 20, //!< Invalid command (unrecognized command name)
      CEC_COMMAND_NO_PRIVILEDGE        = 21, //!< Invalid command priviledge
      CEC_COMMAND_INVALID_CONTEXT      = 22, //!< Invalid command context
      CEC_COMMAND_INVALID_SYNTAX       = 23, //!< Invalid command syntax
      CEC_COMMAND_INVALID_NUMBER_OF_ARGUMENTS = 24, //!< Invalid command number of arguments
      CEC_PARAMETER_INVALID            = 30, //!< Invalid parameter error
      CEC_PARAMETER_INVALID_KEY        = 31, //!< Invalid parameter key
      CEC_PARAMETER_NO_PRIVILEDGE      = 32, //!< Invalid parameter access mode
      CEC_PARAMETER_INVALID_CONTEXT    = 33, //!< Invalid parameter context
      CEC_PARAMETER_INVALID_TYPE       = 34, //!< Invalid parameter type
      CEC_PARAMETER_UNSET_VALUE        = 35, //!< Unset value
      CEC_PARAMETER_INVALID_VALUE      = 36, //!< Invalid parameter value
      CEC_PARAMETER_INVALID_UNIT       = 37, //!< Invalid parameter unit (for real numbers)
    };

    /// \brief Command returned information
    class returned_info
    {
    public:

      /// Default constructor
      returned_info();

      /// Constructor of a successful command returned info
      returned_info(const std::string & output_);

      /// Constructor of a failed command returned info
      returned_info(error_code_type code_, const std::string & message_ = "");

      /// Set continue code
      void set_continue();

      /// Set stop code
      void set_stop();

      /// Set success code and output
      void set_success(const std::string & output_ = "");

      /// Set error code and message
      void set_failure(error_code_type code_, const std::string & message_ = "");

      /// Check success
      bool is_stop() const;

      /// Check success
      bool is_success() const;

      /// Check failure
      bool is_failure() const;

      /// Check if error code matches a given value
      bool error_is(error_code_type code_) const;

      /// Set the error code
      void set_error_code(error_code_type code_);

      /// Return the error code
      error_code_type get_error_code() const;

      /// Check if an error message is set
      bool has_error_message() const;

      /// Set the error message
      void set_error_message(const std::string & message_);

      /// Get the error message
      const std::string& get_error_message() const;

      /// Check if an output is set
      bool has_output() const;

      /// Set the output
      void set_output(const std::string & output_);

      /// Get the output
      const std::string& get_output() const;

      /// Reset
      void reset();

      /// Print operator
      friend std::ostream & operator<<(std::ostream & out_, const returned_info & ri_);

    private:

      error_code_type _error_code_;    //!< Error code
      std::string     _error_message_; //!< Error message
      std::string     _output_;        //!< Output

    };

  };

} // end of namespace datatools

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

#endif // DATATOOLS_COMMAND_UTILS_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
