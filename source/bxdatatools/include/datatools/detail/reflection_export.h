/* -*- mode: c++; -*- */
/* reflection_export.h */ 
// This code is derived by from Boost/Serialization export code.

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)


#ifndef DATATOOLS_DETAIL_REFLECTION_EXPORT_H_
#define DATATOOLS_DETAIL_REFLECTION_EXPORT_H_

#include <boost/mpl/bool.hpp>
#include <boost/serialization/singleton.hpp>

namespace datatools {                                              
  namespace detail {                                     
    namespace reflection {                                      
        //! \brief Struct used internally by the reflection mechanism
        template<class T>
        struct guid_initializer
        {  
          void export_guid(int tag) const {
            ::datatools::detail::reflection::implement_reflection<T> (tag);
            return;
          }
          guid_initializer const & export_guid() const {
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
      inline const char * guid(){
        return NULL;
      }
    } /* reflection */
  } /* detail */
} /* datatools */

#endif // DATATOOLS_DETAIL_REFLECTION_EXPORT_H_
