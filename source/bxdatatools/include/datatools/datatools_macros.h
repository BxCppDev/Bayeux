/// \file datatools/datatools_macros.h

#ifndef DATATOOLS_DATATOOLS_MACROS_H
#define DATATOOLS_DATATOOLS_MACROS_H

// Standard library:
#include <string>

// Third party:
// - Boost:
#include <boost/scoped_ptr.hpp>

#define DT_STATIC_CONST_STRING_REF_DECLARATION(Name) \
  static const std::string & Name();		     \
  /**/

// Construct On First Use Idiom
#define DT_STATIC_CONST_STRING_REF_DEFINITION(Class,Name,Value)		\
  const std::string & Class::Name()                                     \
  {                                                                     \
    DT_STATIC_CONST_STRING_IMPL(Value);					\
  }									\
/**/

// Construct On First Use Idiom
#define DT_STATIC_CONST_STRING_IMPL(Value)				\
  static std::string _token = Value;					\
  return _token;							\
  /**/

#endif // DATATOOLS_DATATOOLS_MACROS_H
