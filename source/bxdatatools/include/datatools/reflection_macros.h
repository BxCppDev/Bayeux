/* -*- mode: c++; -*- */
/* reflection_macros.h */
/*
 * Description :
 *
 *  Some useful classes and macros related to datatools/Camp reflection.
 *
 * Copyright (C) 2012 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
#ifndef DATATOOLS_REFLECTION_MACROS_H_
#define DATATOOLS_REFLECTION_MACROS_H_

// Standard Library
#include <string>

// Third Party
// - Camp
#include <camp/camptype.hpp>
#include <camp/class.hpp>
#include <camp/userobject.hpp>
#include <camp/value.hpp>
#include <camp/args.hpp>
#include <camp/function.hpp>

// Ourself :
#include <datatools/detail/reflection_utils.h>
#include <datatools/detail/reflection_export.h>
#include <datatools/detail/reflection_version.h>

// Inform Camp that class Type exists :
#define DR_CLASS_REGISTER(Type)                   \
  CAMP_TYPE( Type )                               \
  /**/

// Inform Camp that non copyable class Type exists :
#define DR_CLASS_NONCOPYABLE_REGISTER(Type)       \
  CAMP_TYPE_NONCOPYABLE( Type )                   \
  /**/

// Declare Camp RTTI within class declaration :
#define DR_CLASS_RTTI()                         \
  CAMP_RTTI()                                   \
  /**/

#define DR_CLASS_IMPLEMENT_REFLECTION_DECLARATION(Introspectable)       \
  namespace datatools{                                                  \
    namespace detail {                                                  \
      namespace reflection {                                            \
        template<>                                                      \
        void implement_reflection< Introspectable > (unsigned int);     \
      }}}                                                               \
  /**/

#define DR_CLASS_INIT(Introspectable)                           \
  DR_CLASS_REGISTER(Introspectable);                            \
  DR_CLASS_IMPLEMENT_REFLECTION_DECLARATION(Introspectable);    \
  /**/

#define DR_CLASS_NONCOPYABLE_INIT(Introspectable)               \
  DR_CLASS_NONCOPYABLE_REGISTER(Introspectable);                \
  DR_CLASS_IMPLEMENT_REFLECTION_DECLARATION(Introspectable);    \
  /**/


/****************************************
 *    Introspection metaclass macros    *
 ****************************************/

#define DR_CLASS_IMPLEMENT_REFLECTION_HEAD(TypeId,TypeTag)      \
  template<>                                                    \
  void implement_reflection< TypeId > (unsigned int TypeTag)    \
  /**/


// Declare introspection metadata for a class :
#define DR_CLASS_DECLARE2(TypeId,TypeName)      \
  ::camp::Class::declare< TypeId >(TypeName)    \
  /**/

// Declare introspection metadata for a class :
#define DR_CLASS_DECLARE(TypeId)                                        \
  DR_CLASS_DECLARE2( TypeId , ::datatools::detail::reflection::guid< TypeId >() ) \
  /**/

// Declare metadata base class for a class :
#define DR_CLASS_BASE(ParentTypeId)             \
  base< ParentTypeId >()                        \
  /**/

// Declare metadata default constructor for a class :
#define DR_CLASS_CONSTRUCTOR()                  \
  constructor0()                                \
  /**/

// Declare metadata constructor 1 for a class :
#define DR_CLASS_CONSTRUCTOR1(Arg0Type)         \
  constructor1< Arg0Type >()                    \
  /**/

// Declare metadata constructor 2 for a class :
#define DR_CLASS_CONSTRUCTOR2(Arg0Type,Arg1Type)        \
  constructor2< Arg0Type , Arg1Type >()                 \
  /**/

// Declare metadata constructor 3 for a class :
#define DR_CLASS_CONSTRUCTOR3(Arg0Type,Arg1Type,Arg2Type)       \
  constructor3< Arg0Type , Arg1Type , Arg2Type >()              \
  /**/

// Declare metadata constructor 4 for a class :
#define DR_CLASS_CONSTRUCTOR4(Arg0Type,Arg1Type,Arg2Type,Arg3Type)      \
  constructor4< Arg0Type , Arg1Type , Arg2Type , Arg3Type >()           \
  /**/

// Declare metadata constructor 5 for a class :
#define DR_CLASS_CONSTRUCTOR5(Arg0Type,Arg1Type,Arg2Type,Arg3Type,Arg4Type) \
  constructor5< Arg0Type , Arg1Type , Arg2Type , Arg3Type , Arg4Type >() \
  /**/

// Declare metadata static tag for a class :
#define DR_CLASS_TAG(TagName,TagValue)          \
  property(TagName, &TagValue)                  \
  /**/

// Declare metadata read-only property for a class :
#define DR_CLASS_PROPERTY_GETTER(PropertyName,PropertyGetterId) \
  property(PropertyName, &PropertyGetterId)                     \
  /**/

// Declare metadata read-write property for a class :
#define DR_CLASS_PROPERTY_GETTER_SETTER(PropertyName,PropertyGetterId,PropertySetterId) \
  property(PropertyName, &PropertyGetterId, &PropertySetterId)          \
  /**/

// Declare metadata read-write property for a class :
#define DR_CLASS_PROPERTY_GETTER_SETTER_GETTER(PropertyName,PropertyGetterId,PropertySetterId,PropertyGetterId2) \
  property(PropertyName, &PropertyGetterId, &PropertySetterId, &PropertyGetterId2) \
  /**/

/***************************************
 * Macros for methods with no argument *
 ***************************************/

// Declare metadata property for a class method :
#define DR_CLASS_METHOD(MethodName,TypeId,MethodId,ReturnType)  \
  function(MethodName,                                          \
           boost::function< ReturnType ( TypeId & )>            \
           (boost::bind(& MethodId , _1)))                      \
  /**/

// Declare metadata property for a class method :
#define DR_CLASS_VOID_METHOD(MethodName,TypeId,MethodId)        \
  DR_CLASS_METHOD(MethodName,TypeId,MethodId,void)              \
  /**/

// Declare metadata property for a class method (const) :
#define DR_CLASS_METHOD_CONST(MethodName,TypeId,MethodId,ReturnType)    \
  function(MethodName,                                                  \
           boost::function< ReturnType ( const TypeId & )>              \
           (boost::bind(& MethodId , _1 )))                             \
  /**/

// Declare metadata property for a class method (const) :
#define DR_CLASS_VOID_METHOD_CONST(MethodName,TypeId,MethodId)  \
  DR_CLASS_METHOD_CONST(MethodName,TypeId,MethodId,void)        \
  /**/


/**************************************
 * Macros for methods with 1 argument *
 **************************************/

// Declare metadata property for a class method :
#define DR_CLASS_METHOD1(MethodName,TypeId,MethodId,ReturnType,Arg0Type) \
  function(MethodName,                                                  \
           boost::function< ReturnType ( TypeId & , Arg0Type )>         \
           (boost::bind(& MethodId , _1, _2)))                          \
  /**/

// Declare metadata property for a class method :
#define DR_CLASS_VOID_METHOD1(MethodName,TypeId,MethodId,Arg0Type)      \
  DR_CLASS_METHOD1(MethodName,TypeId,MethodId,void,Arg0Type)            \
  /**/

// Declare metadata property for a class method (const) :
#define DR_CLASS_METHOD1_CONST(MethodName,TypeId,MethodId,ReturnType,Arg0Type) \
  function(MethodName,                                                  \
           boost::function< ReturnType ( const TypeId & , Arg0Type )>   \
           (boost::bind(& MethodId , _1, _2 )))                         \
  /**/

// Declare metadata property for a class method (const) :
#define DR_CLASS_VOID_METHOD1_CONST(MethodName,TypeId,MethodId,Arg0Type) \
  DR_CLASS_METHOD1_CONST(MethodName,TypeId,MethodId,void,Arg0Type)      \
  /**/


/***************************************
 * Macros for methods with 2 arguments *
 ***************************************/

// Declare metadata property for a class method :
#define DR_CLASS_METHOD2(MethodName,TypeId,MethodId,ReturnType,Arg0Type,Arg1Type) \
  function(MethodName,                                                  \
           boost::function< ReturnType ( TypeId & , Arg0Type , Arg1Type )> \
           (boost::bind(& MethodId , _1, _2, _3)))                      \
  /**/

// Declare metadata property for a class method :
#define DR_CLASS_VOID_METHOD2(MethodName,TypeId,MethodId,Arg0Type,Arg1Type) \
  DR_CLASS_METHOD2(MethodName,TypeId,MethodId,void,Arg0Type,Arg1Type)   \
  /**/

// Declare metadata property for a class method (const) :
#define DR_CLASS_METHOD2_CONST(MethodName,TypeId,MethodId,ReturnType,Arg0Type,Arg1Type) \
  function(MethodName,                                                  \
           boost::function< ReturnType ( const TypeId & , Arg0Type , Arg1Type )> \
           (boost::bind(& MethodId , _1, _2, _3 )))                     \
  /**/

// Declare metadata property for a class method (const) :
#define DR_CLASS_VOID_METHOD2_CONST(MethodName,TypeId,MethodId,Arg0Type,Arg1Type) \
  DR_CLASS_METHOD2_CONST(MethodName,TypeId,MethodId,void,Arg0Type,Arg1Type) \
  /**/


/***************************************
 * Macros for methods with 3 arguments *
 ***************************************/

// Declare metadata property for a class method :
#define DR_CLASS_METHOD3(MethodName,TypeId,MethodId,ReturnType,Arg0Type,Arg1Type,Arg2Type) \
  function(MethodName,                                                  \
           boost::function< ReturnType ( TypeId & , Arg0Type , Arg1Type , Arg2Type )> \
           (boost::bind(& MethodId , _1, _2, _3, _4)))                  \
  /**/

// Declare metadata property for a class method :
#define DR_CLASS_VOID_METHOD3(MethodName,TypeId,MethodId,Arg0Type,Arg1Type,Arg2Type) \
  DR_CLASS_METHOD3(MethodName,TypeId,MethodId,void,Arg0Type,Arg1Type,Arg2Type) \
  /**/

// Declare metadata property for a class method (const) :
#define DR_CLASS_METHOD3_CONST(MethodName,TypeId,MethodId,ReturnType,Arg0Type,Arg1Type,Arg2Type) \
  function(MethodName,                                                  \
           boost::function< ReturnType ( const TypeId & , Arg0Type , Arg1Type , Arg2Type )> \
           (boost::bind(& MethodId , _1, _2, _3, _4 )))                 \
  /**/

// Declare metadata property for a class method (const) :
#define DR_CLASS_VOID_METHOD3_CONST(MethodName,TypeId,MethodId,Arg0Type,Arg1Type,Arg2Type) \
  DR_CLASS_METHOD3_CONST(MethodName,TypeId,MethodId,void,Arg0Type,Arg1Type,Arg2Type) \
  /**/


/***************************************
 * Macros for methods with 4 arguments *
 ***************************************/

// Declare metadata property for a class method :
#define DR_CLASS_METHOD4(MethodName,TypeId,MethodId,ReturnType,Arg0Type,Arg1Type,Arg2Type,Arg3Type) \
  function(MethodName,                                                  \
           boost::function< ReturnType ( TypeId & , Arg0Type , Arg1Type , Arg2Type , Arg3Type )> \
           (boost::bind(& MethodId , _1, _2, _3, _4, _5)))              \
  /**/

// Declare metadata property for a class method :
#define DR_CLASS_VOID_METHOD4(MethodName,TypeId,MethodId,Arg0Type,Arg1Type,Arg2Type,Arg3Type) \
  DR_CLASS_METHOD4(MethodName,TypeId,MethodId,void,Arg0Type,Arg1Type,Arg2Type,Arg3Type) \
  /**/

// Declare metadata property for a class method (const) :
#define DR_CLASS_METHOD4_CONST(MethodName,TypeId,MethodId,ReturnType,Arg0Type,Arg1Type,Arg2Type,Arg3Type) \
  function(MethodName,                                                  \
           boost::function< ReturnType ( const TypeId & , Arg0Type , Arg1Type , Arg2Type , Arg3Type )> \
           (boost::bind(& MethodId , _1, _2, _3, _4, _5 )))             \
  /**/

// Declare metadata property for a class method (const) :
#define DR_CLASS_VOID_METHOD4_CONST(MethodName,TypeId,MethodId,Arg0Type,Arg1Type,Arg2Type,Arg3Type) \
  DR_CLASS_METHOD4_CONST(MethodName,TypeId,MethodId,void,Arg0Type,Arg1Type,Arg2Type,Arg3Type) \
  /**/

#define DR_CLASS         camp::Class
#define DR_CLASS_BY_NAME camp::classByName
#define DR_OBJECT        camp::UserObject
#define DR_VALUE         camp::Value
#define DR_ARGS          camp::Args
#define DR_FUNCTION      camp::Function
#define DR_SET           set
#define DR_CALL          call
#define DR_GET           get
#define DR_NOTYPE        camp::noType
#define DR_BOOLEAN       camp::boolType
#define DR_INTEGER       camp::intType
#define DR_REAL          camp::realType
#define DR_STRING        camp::stringType
#define DR_ENUM          camp::enumType
#define DR_ARRAY         camp::arrayType
#define DR_USERTYPE      camp::userType

#define DR_REF(TypeId)                          \
  boost::ref( TypeId )                          \
  /**/

#define DR_CONST_REF(TypeId)                    \
  const boost::cref( TypeId )                   \
  /**/

#define DR_REF_WRAPPER(TypeId)                  \
  boost::reference_wrapper< TypeId >            \
  /**/

#define DR_CONST_REF_WRAPPER(TypeId)            \
  boost::reference_wrapper< TypeId const>       \
  /**/

#define DR_CLASS_EXPORT_IMPLEMENT(T)                                    \
  namespace datatools {                                                 \
    namespace detail {                                                  \
      namespace reflection {                                            \
          template<>                                                    \
          struct init_guid< T > {                                       \
            static guid_initializer< T > const & g;                     \
          };                                                            \
          guid_initializer< T > const & init_guid< T >::g =             \
            ::boost::serialization::singleton< guid_initializer< T > >  \
            ::get_mutable_instance().export_guid();                     \
      }}}                                                               \
  /**/


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

#define DR_CLASS_EXPORT(T, K)                   \
  DR_CLASS_EXPORT_KEY (T, K)                    \
  DR_CLASS_EXPORT_IMPLEMENT (T)                 \
  /**/

// specify the current version number for the class
// version numbers limited to 8 bits !!!
#define DR_CLASS_VERSION(T, N)                                          \
  namespace datatools {                                                 \
    namespace detail {                                                  \
      namespace reflection {                                            \
        template<>                                                      \
        struct version<T >                                              \
        {                                                               \
          typedef boost::mpl::int_<N> type;                             \
          typedef boost::mpl::integral_c_tag tag;                       \
          BOOST_STATIC_CONSTANT(int, value = version::type::value);     \
          BOOST_MPL_ASSERT((                                            \
                            boost::mpl::less<boost::mpl::int_<N>,       \
                                             boost::mpl::int_<256> >)); \
      };                                                                \
      }}}                                                               \
  /**/

#endif // DATATOOLS_REFLECTION_MACROS_H_

