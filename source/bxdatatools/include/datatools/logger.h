//! \file    datatools/logger.h
//! \brief   Utilities for logging information
//! \details Logging information from the progress of an application
//!          often requires finer granularity than offered by the
//!          standard library streams. Log messages should also
//!          record where they came from.
//!
//!          To help developers log messages, datatools supplies a
//!          very basic priority based logging system. This uses the
//!          standard iostreams under the hood, but messages are logged
//!          at one of 8 priority levels from most to least critical.
//!          A minimum priority may be set, with only messages at that
//!          priority or higher logged to the relevant iostream.
//!
//!          This minimum priority can be stored in a variable, allowing
//!          logging priority to be adjusted on an instance-by-instance
//!          basis as well as at global or class scope.
//!
//!          Helper macros are provided to provide logging at each
//!          level if a provided priority meets the minimum level.
//!          For example, to log an informational message, you could use:
//!
//!          @code
//!          datatools::logger::priority prio_;
//!          ...
//!          DT_LOG_INFORMATION(prio_, "Information message");
//!          @endcode
//!
//!          Provided the variable prio_ meets the minimum priority for
//!          information messages to be logged, the message will be
//!          streamed to the relevant iostream. As can be seen, the
//!          prio_ variable supplied to the logging macro could come
//!          from any relevant source (global, function argument, class
//!          variable, instance variable and so on).
//
// Copyright (c) 2013 by Ben Morgan <bmorgan.warwick@gmail.com>
// Copyright (c) 2013 by The University of Warwick
// Copyright (c) 2016-2018 by François Mauger <mauger@lpccaen.inp3.fr>
// Copyright (c) 2016-2018 by Université de Caen Normandie
//
// This file is part of datatools.
//
// datatools is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// datatools is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with datatools.  If not, see <http://www.gnu.org/licenses/>.

#ifndef DATATOOLS_LOGGER_H
#define DATATOOLS_LOGGER_H

// Standard Library:
#include <iostream>

// Third Party:
// - Boost:
#include "boost/current_function.hpp"

// This project:
#ifndef Q_MOC_RUN
#include <datatools/reflection_interface.h>
#endif // Q_MOC_RUN

/// \brief Top-level namespace of the Bayeux/datatools module library
namespace datatools {

  // Forward declarations:
  class properties;
  class object_configuration_description;

  //! Organise logging functionality under one roof
  struct logger {

    //! Priority levels for logging from most to least critical
    enum priority {
      PRIO_ALWAYS   = -1, //!< Always print the message
      PRIO_UNDEFINED = 0, //!< Undefined/invalid priority
      PRIO_FATAL,         //!< A fatal error. The application will most likely terminate. This is the highest priority.
      PRIO_CRITICAL,      //!< A critical error. The application might not be able to continue running successfully.
      PRIO_ERROR,         //!< An error. An operation did not complete successfully, but the application as a whole is not affected.
      PRIO_WARNING,       //!< A warning. An operation completed with an unexpected result.
      PRIO_NOTICE,        //!< A notice, which is an information with just a higher priority.
      PRIO_INFORMATION,   //!< An informational message, usually denoting the successful completion of an operation.
      PRIO_DEBUG,         //!< A debugging message.
      PRIO_TRACE          //!< A tracing message. This is the lowest priority.
    };

    //! return priority enumeration value from string
    //! The string can take three forms for matching, e.g. the PRIO_FATAL
    //! priority would be returned from input strings "PRIO_FATAL", "FATAL"
    //! or "fatal".
    //! @param name priority name
    //! @returns the matching priority enumeration or the PRIO_UNDEFINED
    //!          if the name is not valid
    static priority get_priority(const std::string& name);

    //! return priority label from the priority enumeration value
    //! @param p_ priority enumeration
    //! @returns the matching priority label or the empty string if the priority
    //!          is not valid
    static std::string get_priority_label(priority p_);

    //! check if priority is defined
    //! @param p_ priority enumeration
    //! @returns true if the priority is undefined
    static bool is_undefined(priority p_);

    //! check if priority satisfy the fatal level
    //! @param p_ priority enumeration
    //! @returns true if the priority is at least at the fatal level
    static bool is_fatal(priority p_);

    //! check if priority satisfy the critical level
    //! @param p_ priority enumeration
    //! @returns true if the priority is at least at the critical level
    static bool is_critical(priority p_);

    //! check if priority satisfy the error level
    //! @param p_ priority enumeration
    //! @returns true if the priority is at least at the error level
    static bool is_error(priority p_);

    //! check if priority satisfy the warning level
    //! @param p_ priority enumeration
    //! @returns true if the priority is at least at the warning level
    static bool is_warning(priority p_);

    //! check if priority satisfy the notice level
    //! @param p_ priority enumeration
    //! @returns true if the priority is at least at the notice level
    static bool is_notice(priority p_);

    //! check if priority satisfy the information level
    //! @param p_ priority enumeration
    //! @returns true if the priority is at least at the information level
    static bool is_information(priority p_);

    //! check if priority satisfy the debug level
    //! @param p_ priority enumeration
    //! @returns true if the priority is at least at the debug level
    static bool is_debug(priority p_);

    //! check if priority satisfy the trace level
    //! @param p_ priority enumeration
    //! @returns true if the priority is at least at the trace level
    static bool is_trace(priority p_);

    //! return the priority level from a collection of properties
    //! The following properties are recognized:
    //!   logging.priority : string = "fatal" # or "trace", "debug",
    //!                                       # "information", "notice",
    //!                                       # "warning", "error", "critical"
    //!
    //! Shortcuts to activate some specific priorities:
    //! a)   logging.debug   : boolean = 1
    //!      debug           : boolean = 1
    //!   is equivalent to :
    //!      logging.priority : string = "debug"
    //!
    //! b)   logging.verbose : boolean = 1
    //!      verbose         : boolean = 1
    //!   is equivalent to :
    //!      logging.priority : string = "notice"
    //!
    //! Note that the "logging.priority" property has precedence on
    //! "logging.debug" (or "debug") and "logging.verbose" or ("verbose").
    //! @param config_         a collection of properties to be searched
    //! @param default_prio_   the default logging priority threshold
    //! @param throw_on_error_ is true, an invalid logging priority label
    //!        triggers an exception
    //! @returns the priority as configured from the properties
    static priority extract_logging_configuration(const datatools::properties & config_,
                                                  priority default_prio_ = datatools::logger::PRIO_FATAL,
                                                  bool throw_on_error_ = true);

    static void declare_ocd_logging_configuration(datatools::object_configuration_description &,
                                                  const std::string & default_value_ = "fatal",
                                                  const std::string & prefix_ = "",
                                                  const std::string & from_ = "");

  };

} // namespace datatools

#ifndef Q_MOC_RUN
// Activate reflection layer for the 'datatools::logger::priority' enum:
DR_TYPE_INIT(::datatools::logger) // mandatory to access the embedded enum
DR_TYPE_INIT(::datatools::logger::priority)
#endif // Q_MOC_RUN

#include <datatools/detail/logger_macros.h>

#endif // DATATOOLS_LOGGER_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
