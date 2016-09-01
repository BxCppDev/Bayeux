//! \file  datatools/detail/logger_macros.h
//! \brief Macros associated to logging utilities
//
// Copyright (c) 2013-2016 by Ben Morgan <bmorgan.warwick@gmail.com>
// Copyright (c) 2013-2016 by The University of Warwick
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

#ifndef DATATOOLS_DETAIL_LOGGER_MACROS_H
#define DATATOOLS_DETAIL_LOGGER_MACROS_H

// Standard Library:
#include <iostream>

// Third Party:
// - Boost:
#include <boost/current_function.hpp>

// This project:
#include <datatools/logger.h>

//! Log Message if Priority is greater or equal to PRIO_FATAL
#define DT_LOG_FATAL(Priority, Message)                                 \
  {                                                                     \
    ::datatools::logger::priority _dt_xxx_p = Priority;                 \
    if (_dt_xxx_p == ::datatools::logger::PRIO_ALWAYS || _dt_xxx_p >= ::datatools::logger::PRIO_FATAL) { \
      std::cerr << "[fatal:" << BOOST_CURRENT_FUNCTION << ":" << __LINE__ << "] " << Message << std::endl; \
    }                                                                   \
  }

//! Log Message if Priority is greater or equal to PRIO_CRITICAL
#define DT_LOG_CRITICAL(Priority, Message)                              \
  {                                                                     \
    ::datatools::logger::priority _dt_xxx_p = Priority;                 \
    if (_dt_xxx_p == ::datatools::logger::PRIO_ALWAYS || _dt_xxx_p >= ::datatools::logger::PRIO_CRITICAL) { \
      std::cerr << "[critical:" << BOOST_CURRENT_FUNCTION << ":" << __LINE__ << "] " << Message << std::endl; \
    }                                                                   \
  }

//! Log Message if Priority is greater or equal to PRIO_ERROR
#define DT_LOG_ERROR(Priority, Message)                                 \
  {                                                                     \
    ::datatools::logger::priority _dt_xxx_p = Priority;                 \
    if (_dt_xxx_p == ::datatools::logger::PRIO_ALWAYS || _dt_xxx_p  >= ::datatools::logger::PRIO_ERROR) { \
      std::cerr << "[error:" << BOOST_CURRENT_FUNCTION << ":" << __LINE__ << "] " << Message << std::endl; \
    }                                                                   \
  }

//! Log Message if Priority is greater or equal to PRIO_WARNING
#define DT_LOG_WARNING(Priority, Message)                               \
  {                                                                     \
    ::datatools::logger::priority _dt_xxx_p = Priority;                 \
    if (_dt_xxx_p == ::datatools::logger::PRIO_ALWAYS || _dt_xxx_p >= ::datatools::logger::PRIO_WARNING) { \
      std::cerr << "[warning:" << BOOST_CURRENT_FUNCTION << ":" << __LINE__ << "] " << Message << std::endl; \
    }                                                                   \
  }

//! Log Message if Priority is greater or equal to PRIO_NOTICE
#define DT_LOG_NOTICE(Priority, Message)                                \
  {                                                                     \
    ::datatools::logger::priority _dt_xxx_p = Priority;                 \
    if (_dt_xxx_p == ::datatools::logger::PRIO_ALWAYS || _dt_xxx_p >= ::datatools::logger::PRIO_NOTICE) { \
      std::clog << "[notice:" << BOOST_CURRENT_FUNCTION << ":" << __LINE__ << "] " << Message << std::endl; \
    }                                                                   \
  }

//! Log Message if Priority is greater or equal to PRIO_INFORMATION
#define DT_LOG_INFORMATION(Priority, Message)                           \
  {                                                                     \
    ::datatools::logger::priority _dt_xxx_p = Priority;                 \
    if (_dt_xxx_p == ::datatools::logger::PRIO_ALWAYS || _dt_xxx_p >= ::datatools::logger::PRIO_INFORMATION) { \
      std::clog << "[information:" << BOOST_CURRENT_FUNCTION << ":" << __LINE__ << "] " << Message << std::endl; \
    }                                                                   \
  }

//! Log Message if Priority is greater or equal to PRIO_DEBUG
#define DT_LOG_DEBUG(Priority, Message)                                 \
  {                                                                     \
    ::datatools::logger::priority _dt_xxx_p = Priority;                 \
    if (_dt_xxx_p == ::datatools::logger::PRIO_ALWAYS || _dt_xxx_p >= ::datatools::logger::PRIO_DEBUG) { \
      std::clog << "[debug:" << BOOST_CURRENT_FUNCTION << ":" << __LINE__ << "] " << Message << std::endl; \
    }                                                                   \
  }

//! Log Message if Priority is greater or equal to PRIO_TRACE
#define DT_LOG_TRACE(Priority, Message)                                 \
  {                                                                     \
    ::datatools::logger::priority _dt_xxx_p = Priority;                 \
    if (_dt_xxx_p == ::datatools::logger::PRIO_ALWAYS || _dt_xxx_p >= ::datatools::logger::PRIO_TRACE) { \
      std::cerr << "[trace:" << BOOST_CURRENT_FUNCTION << ":" << __LINE__ << "] " << Message << std::endl; \
    }                                                                   \
  }

//! Log an entering message if Priority is greater or equal to PRIO_TRACE
#define DT_LOG_TRACE_ENTERING(Priority)         \
  {                                             \
    DT_LOG_TRACE(Priority, "Entering...");      \
  }

//! Log an exiting message if Priority is greater or equal to PRIO_TRACE
#define DT_LOG_TRACE_EXITING(Priority)          \
  {                                             \
    DT_LOG_TRACE(Priority, "Exiting.");         \
  }

#endif // DATATOOLS_DETAIL_LOGGER_MACROS_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
