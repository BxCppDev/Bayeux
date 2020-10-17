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

#ifndef DATATOOLS_COMMAND_UTILS_H
#define DATATOOLS_COMMAND_UTILS_H

// Standard library:
#include <iostream>

// This project:
#ifndef Q_MOC_RUN
#include <datatools/reflection_interface.h>
#endif // Q_MOC_RUN

namespace datatools {

  /// \brief Generic command utilities
  class command
  {
  public:

    /// \brief Command error codes
    enum error_code_type {
      // Special non-error values:
      CEC_DEPRECATED                   = -3, //!< Deprecated context without error
      CEC_STOP                         = -2, //!< Stop without error (can be used to stop a loop or a pipeline)
      CEC_CONTINUE                     = -1, //!< Continue without error (can be used to continue a loop or a pipeline)
      // Sucess:
      CEC_SUCCESS                      =  0, //!< Generic success
      // Failures:
      CEC_FAILURE                      =  1, //!< Generic failure
      CEC_PARSING_FAILURE              =  2, //!< Generic parsing error
      CEC_CONTEXT_INVALID              =  3, //!< Invalid context
      CEC_NOT_IMPLEMENTED              =  8, //!< Not implemented feature
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
      CEC_PARAMETER_INVALID_UNIT       = 37  //!< Invalid parameter unit (for real numbers)
    };

    // CAMP classes with RTTI imply inheritance
    virtual ~command() = default;

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

      /// Destructor (must be virtual for use with CAMP RTTI)
      virtual ~returned_info();

      /// Set continue code
      void set_continue();

      /// Set stop code
      void set_stop();

      /// Set success code and output
      void set_success(const std::string & output_ = "");

      /// Set error code and message
      void set_failure(error_code_type code_, const std::string & message_ = "");

      /// Check stop
      bool is_stop() const;

      /// Check continue
      bool is_continue() const;

      /// Check deprecation
      bool is_deprecated() const;

      /// Check special case
      bool is_special() const;

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

      DR_CLASS_RTTI()

    };

    DR_CLASS_RTTI()

  };

} // end of namespace datatools

#ifndef Q_MOC_RUN
// Activate reflection layer:
DR_TYPE_INIT(::datatools::command) // mandatory to access the embedded enum
DR_TYPE_INIT(::datatools::command::error_code_type)
DR_TYPE_INIT(::datatools::command::returned_info)
#endif // Q_MOC_RUN

#endif // DATATOOLS_COMMAND_UTILS_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
