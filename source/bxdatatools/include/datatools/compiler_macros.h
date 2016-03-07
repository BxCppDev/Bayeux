//! \file    datatools/compiler_macros.h
//! \brief   Portable macros for compiler attributes
//! \details Working with many different compilers often requires using
//!          different statements for this like attributes (deprecation,
//!          visibility) and pragmas (suppressing external warnings)
//!
//!          To help developers use these features without needing to know
//!          the exact compiler constructs, datatools supplies preprocessor
//!          macros that expand to the required construct for the compiler
//!          in use.
//!
// Copyright (c) 2016 by Ben Morgan <Ben.Morgan@warwick.ac.uk>
// Copyright (c) 2016 by The University of Warwick
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


#ifndef DATATOOLS_COMPILER_MACROS_H
#define DATATOOLS_COMPILER_MACROS_H

//! Mark variable X as possibly unused
//! Use this to wrap a variable requires naming but that may be unused in the
//! current scope. Primarily for use in declarations/definitions of concrete
//! functions where some of the arguments from the abstract method are not used
//! @param X variable name to mark
//!
//! @code
//! void foo(int bar, int DT_UNUSED(baz));
//! @endcode
//!
//! \todo See http://julipedia.meroh.net/2015/02/unused-parameters-in-c-and-c.html for an additional marking convention to handle case when variable is needed!
#if defined(__clang__) || defined(__GNUC__) || defined(__INTEL_COMPILER)
#define DT_UNUSED(X) \
    X __attribute__((unused))
#else
#define DT_UNUSED(X) \
    X
#endif

#endif // DATATOOLS_COMPILER_MACROS_H

