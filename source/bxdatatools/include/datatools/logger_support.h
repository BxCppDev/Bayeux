//! \file    logger_support.h
//! \brief   Utilities for class basic logging support
//! \details Many classes may benefit of some logging features.
//!          datatools supplies some macros to automate the
//!          declaration and definition of basic logging resources
//!          within a class (i.e. a \c datatools::logging::priority enum).
//!          It relies on the datatools::logger concept.
//!          @see logger.h
//
// Copyright (c) 2013 by Francois Mauger <mauger@lpccaen.in2p3.fr>
// Copyright (c) 2013 by The University of Caen Basse-Normandie
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

#ifndef DATATOOLS_LOGGER_SUPPORT_H
#define DATATOOLS_LOGGER_SUPPORT_H

#include <datatools/logger.h>

//! \def DT_LOGGER_OBJECT_INTERFACE(LoggingPriorityAttribute)
//! \brief install some basic logging features in a class (declaration)
//! \details Install some basic logging features in class \c T using
//!          the special attribute \c LoggingPriorityAttribute to store
//!          the logging priority.
//!          This is the interface macro that must be used from within
//!          the class declaration body and associated to the
//!          DT_LOGGER_OBJECT_IMPLEMENTATION implementation macro.
//!
//!          @see DT_LOGGER_OBJECT_IMPLEMENTATION
//!
//! Example in the header file \c foo.h:
//! @code
//! #include <datatools/logger_support.h>
//! class foo {
//!   DT_LOGGER_OBJECT_INTERFACE(logging_priority_);
//! };
//! @endcode
//!
#define DT_LOGGER_OBJECT_INTERFACE(LoggingPriorityAttribute)    \
private:                                                        \
  ::datatools::logger::priority LoggingPriorityAttribute;       \
public:                                                         \
 void set_logging_priority(::datatools::logger::priority);      \
 ::datatools::logger::priority get_logging_priority() const;
 /**/


//! \def DT_LOGGER_OBJECT_IMPLEMENTATION(T,LoggingPriorityAttribute)
//! \brief install some basic logging features in a class (definition)
//! \details Install some simple logging features in the class \c T using
//!          the special private attribute \c LoggingPriorityAttribute to
//!          store the logging priority.
//!          This is the implementation macro that must be used from within
//!          the class definition file and associated to the interface
//!          \c DT_LOGGER_OBJECT_INTERFACE macro.
//!
//!          @see DT_LOGGER_OBJECT_INTERFACE
//!
//! Example in source file \c foo.cc:
//! @code
//! #include <foo.h>
//! DT_LOGGER_OBJECT_IMPLEMENTATION(foo,logging_priority_)
//! @endcode
//!
#define DT_LOGGER_OBJECT_IMPLEMENTATION(T,LoggingPriorityAttribute)     \
  void T::set_logging_priority(::datatools::logger::priority p) {       \
    LoggingPriorityAttribute = p;                                       \
  }                                                                     \
  ::datatools::logger::priority T::get_logging_priority() const {       \
    return LoggingPriorityAttribute;                                    \
  }
  /**/

//! \def DT_LOGGER_OBJECT_DEFAULT_INTERFACE()
//! \brief default version of the \c DT_LOGGER_OBJECT_INTERFACE macro
//! \details @see DT_LOGGER_OBJECT_INTERFACE
//!
//! Example in the header file \c foo.h:
//! @code
//! #include <datatools/logger_support.h>
//! class foo {
//!   DT_LOGGER_OBJECT_DEFAULT_INTERFACE();
//! };
//! @endcode
#define DT_LOGGER_OBJECT_DEFAULT_INTERFACE()            \
  DT_LOGGER_OBJECT_INTERFACE(_dt_logging_priority_)
  /**/

//! \def DT_LOGGER_OBJECT_DEFAULT_IMPLEMENTATION(T)
//! \brief default version of the \c DT_LOGGER_OBJECT_IMPLEMENTATION macro
//! \details @see DT_LOGGER_OBJECT_IMPLEMENTATION
//!
//! Example in source file \c foo.cc:
//! @code
//! #include <foo.h>
//! DT_LOGGER_OBJECT_IMPLEMENTATION(foo)
//! @endcode
#define DT_LOGGER_OBJECT_DEFAULT_IMPLEMENTATION(T)              \
  DT_LOGGER_OBJECT_IMPLEMENTATION(T,_dt_logging_priority_)
  /**/

#endif // DATATOOLS_LOGGER_H_


