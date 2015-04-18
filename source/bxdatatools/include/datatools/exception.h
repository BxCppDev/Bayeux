//! \file    datatools/exception.h
//! \brief   Utility macros for exception handling
//! \details Exceptions are often thrown based on the value of a boolean
//!          expression. The exception, if it holds a string based
//!          message about what went wrong, should report where it was
//!          thrown from.
//!
//!          To help developers throwing exceptions in this fashion,
//!          datatools supplies some macros to assist in simplifying this
//!          work, and making the intent clearer in code.
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

#ifndef DATATOOLS_EXCEPTION_H
#define DATATOOLS_EXCEPTION_H
// Standard Library:
#include <stdexcept>
#include <sstream>

// Third Party:
// - Boost:
#include <boost/current_function.hpp>

/*! Throw ExceptionType with Message if the expression Condition is true
  This macro is intended to simplify the common use case of throwing
  exceptions when a boolean condition is true, with the exception
  holding a string message indicating where and why the exception was
  thrown. This macro takes three arguments as follows

  @param Condition     Boolean expression
  @param ExceptionType Typename of object to throw, the type must take
  a string as its constructor argument
  @param Message       Message to supply to ExceptionType, this must
  take the form of a string or output stream
  sequence

  The Message is formatted as

  @code
  [SIGNATURE:LINENUMBER: Message]
  @endcode

  where SIGNATURE is the signature of the function from which the
  exception is thrown, and LINENUMBER is the line number where the
  throw occured.

  In the simplest case it may be used as

  @code
  DT_THROW_IF(i < 0, std::logic_error, "parameter i is negative");
  @endcode

  If Message is composed from several streamable objects, it can
  be composed using the streaming operator:

  @code
  DT_THROW_IF(i < 0, std::logic_error, "parameter i(" << i << ") is negative");
  @endcode
*/
#define DT_THROW_IF(Condition, ExceptionType, Message)			\
  {									\
    if (Condition) {							\
      std::stringstream s;						\
      s << "[" << BOOST_CURRENT_FUNCTION << ":" << __LINE__ << ": " << Message << "]"; \
      throw ExceptionType(s.str());					\
    }									\
  }


/*! Throw ExceptionType with Message
  This macro is intended to simplify the common use case of throwing
  exceptions, with the exception
  holding a string message indicating where and why the exception was
  thrown. This macro takes two arguments as follows

  @param ExceptionType Typename of object to throw, the type must take
  a string as its constructor argument
  @param Message       Message to supply to ExceptionType, this must
  take the form of a string or output stream
  sequence

  The Message is formatted as

  @code
  [SIGNATURE:LINENUMBER: Message]
  @endcode

  where SIGNATURE is the signature of the function from which the
  exception is thrown, and LINENUMBER is the line number where the
  throw occured.

  In the simplest case it may be used as

  @code
  DT_THROW(std::domain_error, "parameter i is negative");
  @endcode

  If Message is composed from several streamable objects, it can
  be composed using the streaming operator:

  @code
  DT_THROW(std::domain_error, "parameter i(" << i << ") is negative");
  @endcode
*/
#define DT_THROW(ExceptionType, Message)				\
  {									\
    std::stringstream s;						\
    s << "[" << BOOST_CURRENT_FUNCTION << ":" << __LINE__ << ": " << Message << "]"; \
    throw ExceptionType(s.str());					\
  }

#endif // DATATOOLS_EXCEPTION_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
