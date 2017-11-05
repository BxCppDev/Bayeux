/// \file datatools/detail/reflection_export.h
// This code is derived by from Boost/Serialization export code.

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com .
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)


#ifndef DATATOOLS_DETAIL_REFLECTION_EXPORT_H
#define DATATOOLS_DETAIL_REFLECTION_EXPORT_H

// Third party:
// - Boost:
#include <boost/mpl/bool.hpp>
#include <boost/serialization/singleton.hpp>

namespace datatools {
  namespace detail {
    namespace reflection {

      //! \brief Struct used internally by the reflection mechanism
      template<class T>
      struct guid_initializer
      {
        void export_guid(int tag) const
        {
          ::datatools::detail::reflection::implement_reflection<T> (tag);
          return;
        }
        guid_initializer const & export_guid() const
        {
          //BOOST_STATIC_WARNING(boost::is_polymorphic< T >::value);
          // note: exporting an abstract base class will have no effect
          // and cannot be used to instantitiate serialization code
          // (one might be using this in a DLL to instantiate code)
          //BOOST_STATIC_WARNING(! boost::serialization::is_abstract< T >::value);
          //export_guid(boost::serialization::is_abstract< T >());
          export_guid (0);
          return *this;
        }
      };

      template<typename T>
      struct init_guid;

    } // namespace reflection
  } // namespace detail
} // namespace datatools


namespace datatools {
  namespace detail {
    namespace reflection {
      //! \brief Struct used internally by the reflection mechanism
      template<class T>
      struct guid_defined : boost::mpl::false_ {};
      template<class T>
      inline const char * guid()
      {
        return NULL;
      }

    } // namespace reflection
  } // namespace detail
} // namespace datatools

/// \def DR_CLASS_EXPORT_IMPLEMENT(T)
/// \brief Define registration of a class \a T with reflection support
///
/// This macro must be invoked outside of any namespace scope.
/** Example:
    @code{.cpp}
namespace foo {
  class bar {
    ...
  };
}
DR_CLASS_REGISTER(foo::bar);
DR_CLASS_EXPORT_IMPLEMENT(foo::bar);
    @endcode
*/
#define DR_CLASS_EXPORT_IMPLEMENT(T)                                    \
  namespace datatools {                                                 \
    namespace detail {                                                  \
      namespace reflection {                                            \
        template<>                                                      \
        struct init_guid< T > {                                         \
          static guid_initializer< T > const & g;                       \
        };                                                              \
        guid_initializer< T > const & init_guid< T >::g =               \
          ::boost::serialization::singleton< guid_initializer< T > >    \
          ::get_mutable_instance().export_guid();                       \
      }}}                                                               \
  /**/


/// \def DR_CLASS_EXPORT_KEY(T, K)
/// \brief Define class identifier \a K for the registration of a class \a T with reflection support
///
/// This macro must be invoked outside of any namespace scope.
/** Example:
    @code{.cpp}
DR_CLASS_EXPORT_KEY(foo::bar, "foo::bar"");
    @endcode
*/
#define DR_CLASS_EXPORT_KEY(T, K)                               \
  namespace datatools {                                         \
    namespace detail {                                          \
      namespace reflection {                                    \
        template<>                                              \
        struct guid_defined< T > : boost::mpl::true_ {};        \
        template<>                                              \
        inline const char * guid< T >(){                        \
          return K;                                             \
        }                                                       \
      }}}                                                       \
  /**/

/// \def DR_CLASS_EXPORT(T, K)
/// \brief Implement the code for reflection support of class  \a T with identifier \a K
///
/// This macro must be invoked outside of any namespace scope.
/** Example:
    @code{.cpp}
DR_CLASS_EXPORT(foo::bar, "foo::bar"");
    @endcode
*/
#define DR_CLASS_EXPORT(T, K)                   \
  DR_CLASS_EXPORT_KEY (T, K)                    \
  DR_CLASS_EXPORT_IMPLEMENT (T)                 \
  /**/

/// \def DR_TYPE_EXPORT(T, K)
/// \see DR_CLASS_EXPORT
#define DR_TYPE_EXPORT(T, K)                    \
  DR_CLASS_EXPORT (T, K)                        \
  /**/

#endif // DATATOOLS_DETAIL_REFLECTION_EXPORT_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
