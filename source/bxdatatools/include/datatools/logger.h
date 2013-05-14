//! \file    logger.h
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
//!          datatools_information(prio_, "Information message");
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
// Standard Library
#include <iostream>

// Third Party
// - A
#include "boost/current_function.hpp"

// This Project

namespace datatools {
//! Organise logging functionality under one roof
struct logger {
  //! Priority levels for logging from most to least critical
  enum priority {
    PRIO_FATAL = 1,   //!< A fatal error. The application will most likely terminate. This is the highest priority.
    PRIO_CRITICAL,    //!< A critical error. The application might not be able to continue running successfully.
    PRIO_ERROR,       //!< An error. An operation did not complete successfully, but the application as a whole is not affected.
    PRIO_WARNING,     //!< A warning. An operation completed with an unexpected result.
    PRIO_NOTICE,      //!< A notice, which is an information with just a higher priority.
    PRIO_INFORMATION, //!< An informational message, usually denoting the successful completion of an operation.
    PRIO_DEBUG,       //!< A debugging message.
    PRIO_TRACE        //!< A tracing message. This is the lowest priority.
  };

  //! return priority enumeration value from string
  //! The string can take three forms for matching, e.g. the PRIO_FATAL
  //! priority would be returned from input strings "PRIO_FATAL", "FATAL"
  //! or "fatal".
  //! @param name priority name
  //! @returns the matching priority enumeration
  static priority get_priority(const std::string& name);
};
} // namespace datatools

//! Log Message if Priority is greater or equal to PRIO_FATAL
#define datatools_fatal(Priority, Message) \
{ \
  if (Priority >= datatools::logger::PRIO_FATAL) { \
    std::cerr << "[fatal:" << BOOST_CURRENT_FUNCTION << ":" << __LINE__ << "] " << Message << std::endl; \
  }\
}  

//! Log Message if Priority is greater or equal to PRIO_CRITICAL
#define datatools_critical(Priority, Message) \
{ \
  if (Priority >= datatools::logger::PRIO_CRITICAL) { \
    std::cerr << "[critical:" << BOOST_CURRENT_FUNCTION << ":" << __LINE__ << "] " << Message << std::endl; \
  }\
}  

//! Log Message if Priority is greater or equal to PRIO_ERROR
#define datatools_error(Priority, Message) \
{ \
  if (Priority >= datatools::logger::PRIO_ERROR) { \
    std::cerr << "[error:" << BOOST_CURRENT_FUNCTION << ":" << __LINE__ << "] " << Message << std::endl; \
  }\
}  

//! Log Message if Priority is greater or equal to PRIO_WARNING
#define datatools_warning(Priority, Message) \
{ \
  if (Priority >= datatools::logger::PRIO_WARNING) { \
    std::cerr << "[warning:" << BOOST_CURRENT_FUNCTION << ":" << __LINE__ << "] " << Message << std::endl; \
  }\
}  

//! Log Message if Priority is greater or equal to PRIO_NOTICE
#define datatools_notice(Priority, Message) \
{ \
  if (Priority >= datatools::logger::PRIO_NOTICE) { \
    std::clog << "[notice:" << BOOST_CURRENT_FUNCTION << ":" << __LINE__ << "] " << Message << std::endl; \
  }\
}  

//! Log Message if Priority is greater or equal to PRIO_INFORMATION
#define datatools_information(Priority, Message) \
{ \
  if (Priority >= datatools::logger::PRIO_INFORMATION) { \
    std::clog << "[infomation:" << BOOST_CURRENT_FUNCTION << ":" << __LINE__ << "] " << Message << std::endl; \
  }\
}  

//! Log Message if Priority is greater or equal to PRIO_DEBUG
#define datatools_debug(Priority, Message) \
{ \
  if (Priority >= datatools::logger::PRIO_DEBUG) { \
    std::clog << "[debug:" << BOOST_CURRENT_FUNCTION << ":" << __LINE__ << "] " << Message << std::endl; \
  }\
}  

//! Log Message if Priority is greater or equal to PRIO_TRACE
#define datatools_trace(Priority, Message) \
{ \
  if (Priority >= datatools::logger::PRIO_TRACE) { \
    std::clog << "[trace:" << BOOST_CURRENT_FUNCTION << ":" << __LINE__ << "] " << Message << std::endl; \
  }\
}  

#endif // DATATOOLS_LOGGER_H

