/* datatools/utils/handle_macros.h */
/* Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2012-03-20
 * Last modified : 2012-03-20
 */

#ifndef __datatools__utils__handle_macros_h
#define __datatools__utils__handle_macros_h 1

#include <datatools/utils/handle.h>
#include <stdexcept>
#include <sstream>
#include <boost/preprocessor/stringize.hpp>

/************
 *  MACROS  *
 ************/

#define DATATOOLS_UTILS_HANDLE_DECLARE(HandleVar,Type) \
  datatools::utils::handle<Type> HandleVar; \
\
  /**/

#define DATATOOLS_UTILS_HANDLE_DECLARE_NEW(HandleVar,Type) \
  datatools::utils::handle<Type> HandleVar (new Type); \
\
  /**/

#define DATATOOLS_UTILS_HANDLE_GET_REF(RefVar,HandleVar,Type) \
  if (! HandleVar.has_data ()) \
    {\
      std::ostringstream _datatools_handle_get_ref_message;\
      _datatools_handle_get_ref_message << "macro DATATOOLS_HANDLE" << "_GET_CONST_REF: " \
                                              << "Handle '" << BOOST_PP_STRINGIZE(HandleVar) \
                                              << "' has no data !";     \
      throw std::logic_error (_datatools_handle_get_ref_message.str ());\
    }\
  const Type & RefVar = HandleVar.get (); \
\
  /**/

#define DATATOOLS_UTILS_HANDLE_GRAB_REF(RefVar,HandleVar,Type) \
  if (! HandleVar.has_data ()) \
    {\
      std::ostringstream _datatools_handle_grab_ref_message;\
      _datatools_handle_grab_ref_message << "macro DATATOOLS_HANDLE" << "_GRAB_REF: " \
                                         << "Handle '" << BOOST_PP_STRINGIZE(HandleVar) \
                                         << "' has no data !";\
      throw std::logic_error (_datatools_handle_grab_ref_message.str ());\
    }\
  Type & RefVar = HandleVar.get (); \
\
  /**/

#endif // __datatools__utils__handle_macros_h 

/* end of datatools/utils/handle.h */
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
