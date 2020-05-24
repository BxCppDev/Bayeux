/// \file datatools/reflection_interface.h
/// \details Some utilities and macros related to Camp based reflection within datatools
/*
 * Copyright (C) 2012-2016 Francois Mauger <mauger@lpccaen.in2p3.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#ifndef DATATOOLS_REFLECTION_INTERFACE_H
#define DATATOOLS_REFLECTION_INTERFACE_H

// Third Party:
// - Boost:
#include <boost/utility.hpp> // 2020-05-20 : fix missing header in CAMP 0.8.4
// - Camp:
#include <camp/detail/typeid.hpp>
#include <camp/camptype.hpp>

/// \def DR_CLASS_RTTI()
/// \brief Declare Camp RTTI within class declaration
///
/** Example:
    @code{.cpp}
    namespace foo {
    class bar {
    ...
    #ifndef Q_MOC_RUN // Special guard to avoid clash with Qt MOC interpreter
    DR_CLASS_RTTI();
    #endif // Q_MOC_RUN
    };
    }
    @endcode
*/
#define DR_CLASS_RTTI()                         \
  CAMP_RTTI()                                   \
  /**/
/// \def DR_CLASS_REGISTER(Class)
/// \brief Inform Camp that class \a Class exists
///
/// @arg \a Class the full identifier of the class to be registered
///      in the Camp system (including namespaces)
///
/** Example:
    @code{.cpp}
    namespace foo {
    class bar {
    ...
    };
    }
    DR_CLASS_REGISTER(foo::bar);
    @endcode
*/
#define DR_CLASS_REGISTER(Class)                \
  CAMP_TYPE( Class )                            \
  /**/

/// \def DR_TYPE_REGISTER(Type)
/// \brief Inform Camp that type \a Type exists
///
/// @see DR_CLASS_REGISTER
#define DR_TYPE_REGISTER(Type)                  \
  CAMP_TYPE( Type )                             \
  /**/

/// \def DR_CLASS_NONCOPYABLE_REGISTER(Class)
/// \brief Inform Camp that non copyable class \a Class exists :
///
/// @arg Class the full identifier of the class to be registered
///      in the Camp system (including namespaces)
///
/** Example:
    @code{.cpp}
    namespace foo {
    class bar : private boost::noncopyable {
    ...
    };
    }
    DR_CLASS_NONCOPYABLE_REGISTER(foo::bar);
    @endcode
*/
#define DR_CLASS_NONCOPYABLE_REGISTER(Class)    \
  CAMP_TYPE_NONCOPYABLE( Class )                \
  /**/

/// \def DR_TYPE_NONCOPYABLE_REGISTER(Type)
/// \brief Inform Camp that non copyable type \a Type exists
///
/// @see DR_CLASS_NONCOPYABLE_REGISTER
#define DR_TYPE_NONCOPYABLE_REGISTER(Type)      \
  CAMP_TYPE_NONCOPYABLE( Type )                 \
  /**/

namespace datatools{
  namespace detail {
    /// Nested private namespace of the Bayeux/datatools module library (reflection)
    namespace reflection {
      template<typename Introspectable>
      void implement_reflection(unsigned int);
    }
  }
}

/// \def DR_CLASS_IMPLEMENT_REFLECTION_DECLARATION(Introspectable)
/// \brief Declare the reflection template function associated to class \a Introspectable
#define DR_CLASS_IMPLEMENT_REFLECTION_DECLARATION(Introspectable)   \
  namespace datatools{                                              \
    namespace detail {                                              \
      namespace reflection {                                        \
        template<>                                                  \
        void implement_reflection< Introspectable > (unsigned int); \
      }}}                                                           \
  /**/

/// \def DR_TYPE_IMPLEMENT_REFLECTION_DECLARATION(Introspectable)
/// \see DR_CLASS_IMPLEMENT_REFLECTION_DECLARATION
#define DR_TYPE_IMPLEMENT_REFLECTION_DECLARATION(Introspectable)  \
  DR_CLASS_IMPLEMENT_REFLECTION_DECLARATION(Introspectable)       \
  /**/

/// \def DR_CLASS_INIT(Introspectable)
/// \brief Inform Camp that class \a Introspectable exists and trigger the automatic registration
///        of dedicated reflection feature.
///
/// @arg \a Introspectable the full identifier (with namespaces) of the introspectable class to be registered
///      in the Camp system with introspection support in Bayeux/datatools.
///
/** Example:
    @code{.cpp}
    namespace foo {
    class bar {
    ...
    };
    }
    DR_CLASS_INIT(foo::bar);
    @endcode
*/
#define DR_CLASS_INIT(Introspectable)                       \
  DR_CLASS_REGISTER(Introspectable)                         \
  DR_CLASS_IMPLEMENT_REFLECTION_DECLARATION(Introspectable) \
  /**/

/// \def DR_CLASS_NONCOPYABLE_INIT(Introspectable)
/// \brief Inform Camp that non copyable class \a Introspectable exists and trigger the automatic registration
///        of dedicated introspection feature.
/// @see DR_CLASS_INIT
#define DR_CLASS_NONCOPYABLE_INIT(Introspectable)           \
  DR_CLASS_NONCOPYABLE_REGISTER(Introspectable)             \
  DR_CLASS_IMPLEMENT_REFLECTION_DECLARATION(Introspectable) \
  /**/

/// \def DR_TYPE_INIT(Introspectable)
/// @see DR_CLASS_INIT
#define DR_TYPE_INIT(Introspectable)            \
  DR_CLASS_INIT(Introspectable)                 \
  /**/

/// \def DR_TYPE_NONCOPYABLE_INIT(Introspectable)
/// @see DR_CLASS_NONCOPYABLE_INIT
#define DR_TYPE_NONCOPYABLE_INIT(Introspectable)  \
  DR_CLASS_NONCOPYABLE_INIT(Introspectable)       \
  /**/

#endif // DATATOOLS_REFLECTION_INTERFACE_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
