//! \file    api.h
//! \brief   Internal definitions for symbol visibility and deprecation
//! \details Different compilers handle compile time features such as
//!          symbol visibility and attributes in a plethora of ways.
//!          Provide portable definitions for use by developers of
//!          datatools
//!
//!          At present Clang, Intel, Microsoft and GNU compilers are
//!          supported. Other compilers are configured to emit a
//!          warning about lack of support.
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

// - Deprecation attribute -
// NB check non gnu first because __GNUC__ can be defined by compilers
// suporting the GNU extensions!
#if defined(__clang__)
#if __has_extension(attribute_deprecated_with_message)
#define DATATOOLS_DEPRECATED(Message) __attribute__((deprecated(Message)))
#else
#define DATATOOLS_DEPRECATED(Message) __attribute__((deprecated))
#endif

#elif defined(__INTEL_COMPILER)
#define DATATOOLS_DEPRECATED(Message) __attribute__((deprecated))

#elif defined(_MSC_VER)
#define DATATOOLS_DEPRECATED(Message) __declspec(deprecated(Message))

#elif defined(__GNUC__)
// Version macro as recommended by the Pre-defined Compiler Macros Project
// http://sourceforge.net/p/predef/wiki/Home/
#if defined(__GNUC_PATCHLEVEL__)
#define __GNUC_VERSION__ (__GNUC__ * 10000 \
                          + __GNUC_MINOR__ * 100 \
                          + __GNUC_PATCHLEVEL__)
#else
#define __GNUC_VERSION__ (__GNUC__ * 10000 \
                          + __GNUC_MINOR__ * 100)
#endif
#if __GNUC_VERSION__ < 40500
#define DATATOOLS_DEPRECATED(Message) __attribute__((deprecated))
#else
#define DATATOOLS_DEPRECATED(Message) __attribute__((deprecated(Message)))
#endif

#else
#warning "No DATATOOLS_DEPRECATION available - check your use of the datatools API"
#endif

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
