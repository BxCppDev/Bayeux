/// \file datatools/handle_macros.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2012-03-20
 * Last modified : 2013-04-23
 */

#ifndef DATATOOLS_HANDLE_MACROS_H
#define DATATOOLS_HANDLE_MACROS_H

// Standard Library:
#include <stdexcept>
#include <sstream>

// Third Party:
// - Boost:
#include <boost/preprocessor/stringize.hpp>

// This Project:
#include <datatools/handle.h>

// Macros:

#define DATATOOLS_HANDLE_DECLARE(HandleVar,Type)	\
  ::datatools::handle<Type> HandleVar;			\
							\
  /**/

#define DATATOOLS_HANDLE_DECLARE_NEW(HandleVar,Type)	\
  ::datatools::handle<Type> HandleVar(new Type);	\
							\
  /**/

#define DATATOOLS_HANDLE_GET_REF(RefVar,HandleVar,Type)			\
  DT_THROW_IF (! HandleVar,						\
	       std::logic_error,					\
	       "Handle '" << BOOST_PP_STRINGIZE(HandleVar) << "' has no data !"); \
  const Type& RefVar = HandleVar.get();
/**/

#define DATATOOLS_HANDLE_GRAB_REF(RefVar,HandleVar,Type)		\
  DT_THROW_IF (! HandleVar,						\
	       std::logic_error,					\
	       "Handle '" << BOOST_PP_STRINGIZE(HandleVar) << "' has no data !"); \
  Type& RefVar = HandleVar.grab();
/**/

#endif // DATATOOLS_HANDLE_MACROS_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
