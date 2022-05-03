/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing#kwsys for details.  */
#ifndef datatools_Configure_hxx
#define datatools_Configure_hxx

/* Include C configuration.  */
#include <datatools/detail/Configure.h>

/* Whether wstring is available.  */
#define datatools_STL_HAS_WSTRING 1
/* Whether <ext/stdio_filebuf.h> is available. */
#define datatools_CXX_HAS_EXT_STDIO_FILEBUF_H                         \
  
/* Whether the translation map is available or not. */
#define datatools_SYSTEMTOOLS_USE_TRANSLATION_MAP                     \
  

#if defined(__SUNPRO_CC) && __SUNPRO_CC > 0x5130 && defined(__has_attribute)
#  define datatools_has_cpp_attribute(x) __has_attribute(x)
#elif defined(__has_cpp_attribute)
#  define datatools_has_cpp_attribute(x) __has_cpp_attribute(x)
#else
#  define datatools_has_cpp_attribute(x) 0
#endif

#if __cplusplus >= 201103L
#  define datatools_NULLPTR nullptr
#else
#  define datatools_NULLPTR 0
#endif

#ifndef datatools_FALLTHROUGH
#  if __cplusplus >= 201703L &&                                               \
    datatools_has_cpp_attribute(fallthrough)
#    define datatools_FALLTHROUGH [[fallthrough]]
#  elif __cplusplus >= 201103L &&                                             \
    datatools_has_cpp_attribute(gnu::fallthrough)
#    define datatools_FALLTHROUGH [[gnu::fallthrough]]
#  elif __cplusplus >= 201103L &&                                             \
    datatools_has_cpp_attribute(clang::fallthrough)
#    define datatools_FALLTHROUGH [[clang::fallthrough]]
#  endif
#endif
#ifndef datatools_FALLTHROUGH
#  define datatools_FALLTHROUGH static_cast<void>(0)
#endif

#undef datatools_has_cpp_attribute

/* If building a C++ file in kwsys itself, give the source file
   access to the macros without a configured namespace.  */
#if defined(KWSYS_NAMESPACE)
#  if !datatools_NAME_IS_KWSYS
#    define kwsys datatools
#  endif
#  define KWSYS_NAME_IS_KWSYS datatools_NAME_IS_KWSYS
#  define KWSYS_STL_HAS_WSTRING datatools_STL_HAS_WSTRING
#  define KWSYS_CXX_HAS_EXT_STDIO_FILEBUF_H                                   \
    datatools_CXX_HAS_EXT_STDIO_FILEBUF_H
#  define KWSYS_FALLTHROUGH datatools_FALLTHROUGH
#  define KWSYS_SYSTEMTOOLS_USE_TRANSLATION_MAP                               \
    datatools_SYSTEMTOOLS_USE_TRANSLATION_MAP
#endif

#endif
