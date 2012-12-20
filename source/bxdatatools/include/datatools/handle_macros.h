/* datatools/handle_macros.h */
/* Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2012-03-20
 * Last modified : 2012-03-20
 */
#ifndef DATATOOLS_HANDLE_MACROS_H_
#define DATATOOLS_HANDLE_MACROS_H_
// Standard Library
#include <stdexcept>
#include <sstream>

// Third Party
// - Boost
#include <boost/preprocessor/stringize.hpp>

// This Project
#include <datatools/handle.h>

/************
 *  MACROS  *
 ************/

#define DATATOOLS_HANDLE_DECLARE(HandleVar,Type)	\
  ::datatools::handle<Type> HandleVar;			\
							\
  /**/

#define DATATOOLS_HANDLE_DECLARE_NEW(HandleVar,Type)	\
  ::datatools::handle<Type> HandleVar(new Type);	\
							\
  /**/

#define DATATOOLS_HANDLE_GET_REF(RefVar,HandleVar,Type)			\
  if (! HandleVar.has_data()) {						\
    std::ostringstream _datatools_handle_get_ref_message;		\
    _datatools_handle_get_ref_message << "macro DATATOOLS_HANDLE" << "_GET_CONST_REF: " \
				      << "Handle '" << BOOST_PP_STRINGIZE(HandleVar) \
				      << "' has no data !";		\
    throw std::logic_error(_datatools_handle_get_ref_message.str());	\
  }									\
  const Type& RefVar = HandleVar.get();					\
									\
  /**/

#define DATATOOLS_HANDLE_GRAB_REF(RefVar,HandleVar,Type)		\
  if (! HandleVar.has_data()) {						\
    std::ostringstream _datatools_handle_grab_ref_message;		\
    _datatools_handle_grab_ref_message << "macro DATATOOLS_HANDLE" << "_GRAB_REF: " \
				       << "Handle '" << BOOST_PP_STRINGIZE(HandleVar) \
				       << "' has no data !";		\
    throw std::logic_error (_datatools_handle_grab_ref_message.str());	\
  }									\
  Type& RefVar = HandleVar.get();					\
									\
  /**/

#endif // DATATOOLS_HANDLE_MACROS_H_

