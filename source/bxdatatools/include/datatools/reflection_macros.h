/// \file datatools/reflection_macros.h
/// \details Some useful classes and macros related to Camp based reflection within datatools
/*
 * Copyright (C) 2012-2015 Francois Mauger <mauger@lpccaen.in2p3.fr>
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

#ifndef DATATOOLS_REFLECTION_MACROS_H
#define DATATOOLS_REFLECTION_MACROS_H

// Standard Library:
#include <string>

// Third Party:
// - Camp:
#include <camp/camptype.hpp>
#include <camp/class.hpp>
#include <camp/enum.hpp>
#include <camp/value.hpp>
#include <camp/function.hpp>
#include <camp/property.hpp>
#include <camp/arrayproperty.hpp>
#include <camp/enumobject.hpp>
#include <camp/userobject.hpp>
#include <camp/classvisitor.hpp>
#include <camp/simpleproperty.hpp>
#include <camp/arrayproperty.hpp>
#include <camp/enumproperty.hpp>
#include <camp/userproperty.hpp>
#include <camp/observer.hpp>
#include <camp/args.hpp>
#include <camp/function.hpp>

// This project :
#include <datatools/detail/reflection_utils.h>
#include <datatools/detail/reflection_export.h>
#include <datatools/detail/reflection_version.h>

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
#define DR_CLASS_REGISTER(Class)                 \
  CAMP_TYPE( Class )                             \
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
#define DR_CLASS_NONCOPYABLE_REGISTER(Class)     \
  CAMP_TYPE_NONCOPYABLE( Class )                 \
  /**/

/// \def DR_TYPE_NONCOPYABLE_REGISTER(Type)
/// \brief Inform Camp that non copyable type \a Type exists
///
/// @see DR_CLASS_NONCOPYABLE_REGISTER
#define DR_TYPE_NONCOPYABLE_REGISTER(Type)      \
  CAMP_TYPE_NONCOPYABLE( Type )                 \
  /**/

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

/// \def DR_CLASS_IMPLEMENT_REFLECTION_DECLARATION(Introspectable)
/// \brief Declare the reflection template function associated to class \a Introspectable
#define DR_CLASS_IMPLEMENT_REFLECTION_DECLARATION(Introspectable)       \
  namespace datatools{                                                  \
    namespace detail {                                                  \
      namespace reflection {                                            \
        template<>                                                      \
        void implement_reflection< Introspectable > (unsigned int);     \
      }}}                                                               \
  /**/

/// \def DR_TYPE_IMPLEMENT_REFLECTION_DECLARATION(Introspectable)
/// \see DR_CLASS_IMPLEMENT_REFLECTION_DECLARATION
#define DR_TYPE_IMPLEMENT_REFLECTION_DECLARATION(Introspectable)        \
  DR_CLASS_IMPLEMENT_REFLECTION_DECLARATION(Introspectable)             \
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
#define DR_CLASS_INIT(Introspectable)                           \
  DR_CLASS_REGISTER(Introspectable);                            \
  DR_CLASS_IMPLEMENT_REFLECTION_DECLARATION(Introspectable);    \
  /**/

/// \def DR_CLASS_NONCOPYABLE_INIT(Introspectable)
/// \brief Inform Camp that non copyable class \a Introspectable exists and trigger the automatic registration
///        of dedicated introspection feature.
/// @see DR_CLASS_INIT
#define DR_CLASS_NONCOPYABLE_INIT(Introspectable)               \
  DR_CLASS_NONCOPYABLE_REGISTER(Introspectable);                \
  DR_CLASS_IMPLEMENT_REFLECTION_DECLARATION(Introspectable);    \
  /**/

/// \def DR_TYPE_INIT(Introspectable)
/// @see DR_CLASS_INIT
#define DR_TYPE_INIT(Introspectable)            \
  DR_CLASS_INIT(Introspectable)                 \
  /**/

/// \def DR_TYPE_NONCOPYABLE_INIT(Introspectable)
/// @see DR_CLASS_NONCOPYABLE_INIT
#define DR_TYPE_NONCOPYABLE_INIT(Introspectable)        \
  DR_CLASS_NONCOPYABLE_INIT(Introspectable)             \
  /**/

/****************************************
 *    Introspection metaclass macros    *
 ****************************************/

/// \def DR_CLASS_IMPLEMENT_REFLECTION_HEAD(TypeId,TypeTag)
/// \brief Header of the implementation code block that defines
///        Camp based reflection functionalities for class \a TypeId.
#define DR_CLASS_IMPLEMENT_REFLECTION_HEAD(TypeId,TypeTag)      \
  template<>                                                    \
  void implement_reflection< TypeId > (unsigned int TypeTag)    \
  /**/

/// \def DR_TYPE_IMPLEMENT_REFLECTION_HEAD(TypeId,TypeTag)
/// @see DR_CLASS_IMPLEMENT_REFLECTION_HEAD
#define DR_TYPE_IMPLEMENT_REFLECTION_HEAD(TypeId,TypeTag)       \
  DR_CLASS_IMPLEMENT_REFLECTION_HEAD(TypeId,TypeTag)            \
  /**/

/// \def DR_CLASS_DECLARE2(ClassId,ClassName)
/// \brief Declare reflection metaclass for class \a ClassId with registration class name \a ClassName
#define DR_CLASS_DECLARE2(ClassId,ClassName)      \
  ::camp::Class::declare< ClassId >(ClassName)    \
  /**/

/// \def DR_CLASS_DECLARE(ClassId)
/// \brief Declare reflection metaclass for class \a ClassId
#define DR_CLASS_DECLARE(ClassId)                                        \
  DR_CLASS_DECLARE2( ClassId , ::datatools::detail::reflection::guid< ClassId >() ) \
  /**/

/// \def DR_CLASS_BASE(ParentTypeId)
/// \brief Directive to declare the  \a ParentTypeId class as a base class
#define DR_CLASS_BASE(ParentTypeId)             \
  base< ParentTypeId >()                        \
  /**/

/// \def DR_CLASS_CONSTRUCTOR()
/// \brief Declare default constructor for a class
#define DR_CLASS_CONSTRUCTOR()                  \
  constructor0()                                \
  /**/

/// \def DR_CLASS_CONSTRUCTOR1(Arg0Type)
/// \brief Declare a class constructor with one argument of type \a Arg0Type
#define DR_CLASS_CONSTRUCTOR1(Arg0Type)         \
  constructor1< Arg0Type >()                    \
  /**/

/// \def DR_CLASS_CONSTRUCTOR2(Arg0Type,Arg1Type)
/// \brief Declare a class constructor with two arguments of type \a Arg0Type and \a Arg1Type
#define DR_CLASS_CONSTRUCTOR2(Arg0Type,Arg1Type)        \
  constructor2< Arg0Type , Arg1Type >()                 \
  /**/

/// \def DR_CLASS_CONSTRUCTOR3(Arg0Type,Arg1Type,Arg2Type)
/// \brief Declare a class constructor with three arguments of type \a Arg0Type, \a Arg1Type and \a Arg2Type
#define DR_CLASS_CONSTRUCTOR3(Arg0Type,Arg1Type,Arg2Type)       \
  constructor3< Arg0Type , Arg1Type , Arg2Type >()              \
  /**/

/// \def DR_CLASS_CONSTRUCTOR4(Arg0Type,Arg1Type,Arg2Type,Arg3Type)
/// \brief Declare a class constructor with four arguments of type \a Arg0Type, \a Arg1Type, \a Arg2Type and \a Arg3Type
#define DR_CLASS_CONSTRUCTOR4(Arg0Type,Arg1Type,Arg2Type,Arg3Type)      \
  constructor4< Arg0Type , Arg1Type , Arg2Type , Arg3Type >()           \
  /**/

/// \def DR_CLASS_CONSTRUCTOR5(Arg0Type,Arg1Type,Arg2Type,Arg3Type,Arg4Type)
/// \brief Declare a class constructor with five arguments of type \a Arg0Type, \a Arg1Type,
///        \a Arg2Type, \a Arg3Type and \a Arg4Type
#define DR_CLASS_CONSTRUCTOR5(Arg0Type,Arg1Type,Arg2Type,Arg3Type,Arg4Type) \
  constructor5< Arg0Type , Arg1Type , Arg2Type , Arg3Type , Arg4Type >() \
  /**/

// Declare metadata static flag tag for a class :
#define DR_CLASS_TAG_FLAG(TagName)              \
  tag(TagName)                                  \
  /**/

// Declare metadata static tag with value for a class :
#define DR_CLASS_TAG(TagName,TagValue)          \
  tag(TagName, TagValue)                       \
  /**/

// Declare metadata read-only property for a class :
#define DR_CLASS_PROPERTY_GETTER(PropertyName,PropertyGetterId) \
  property(PropertyName, &PropertyGetterId)                     \
  /**/

// Declare metadata read-only property for a class :
#define DR_CLASS_PROPERTY_REFGETTER(PropertyName,RefPropertyGetterId)   \
  property(PropertyName, RefPropertyGetterId)                           \
  /**/

// Declare metadata read-write property for a class :
#define DR_CLASS_PROPERTY_GETTER_SETTER(PropertyName,PropertyGetterId,PropertySetterId) \
  property(PropertyName, &PropertyGetterId, &PropertySetterId)          \
  /**/

// Declare metadata read-write property for a class :
#define DR_CLASS_PROPERTY_REFGETTER_REFSETTER(PropertyName,RefPropertyGetterId,RefPropertySetterId) \
  property(PropertyName, RefPropertyGetterId, RefPropertySetterId)      \
  /**/

// Declare metadata read-write property for a class :
#define DR_CLASS_PROPERTY_GETTER_SETTER_GETTER(PropertyName,PropertyGetterId,PropertySetterId,PropertyGetterId2) \
  property(PropertyName, &PropertyGetterId, &PropertySetterId, &PropertyGetterId2) \
  /**/

// Declare metadata read-write property for a class :
#define DR_CLASS_PROPERTY_REFGETTER_REFSETTER_REFGETTER(PropertyName,RefPropertyGetterId,RefPropertySetterId,RefPropertyGetterId2) \
  property(PropertyName, RefPropertyGetterId, RefPropertySetterId, RefPropertyGetterId2) \
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
#define DR_CLASS_REFMETHOD(MethodName,TypeId,RefMethodId,ReturnType)    \
  function(MethodName,                                                  \
           boost::function< ReturnType ( TypeId & )>                    \
           (boost::bind(RefMethodId , _1)))                             \
  /**/

// Declare metadata property for a class method :
#define DR_CLASS_VOID_METHOD(MethodName,TypeId,MethodId)        \
  DR_CLASS_METHOD(MethodName,TypeId,MethodId,void)              \
  /**/

// Declare metadata property for a class method :
#define DR_CLASS_VOID_REFMETHOD(MethodName,TypeId,RefMethodId)  \
  DR_CLASS_REFMETHOD(MethodName,TypeId,RefMethodId,void)        \
  /**/

// Declare metadata property for a class method (const) :
#define DR_CLASS_METHOD_CONST(MethodName,TypeId,MethodId,ReturnType)    \
  function(MethodName,                                                  \
           boost::function< ReturnType ( const TypeId & )>              \
           (boost::bind(& MethodId , _1 )))                             \
  /**/

// Declare metadata property for a class method (const) :
#define DR_CLASS_REFMETHOD_CONST(MethodName,TypeId,RefMethodId,ReturnType) \
  function(MethodName,                                                  \
           boost::function< ReturnType ( const TypeId & )>              \
           (boost::bind(RefMethodId , _1 )))                            \
  /**/

// Declare metadata property for a class method (const) :
#define DR_CLASS_VOID_METHOD_CONST(MethodName,TypeId,MethodId)  \
  DR_CLASS_METHOD_CONST(MethodName,TypeId,MethodId,void)        \
  /**/

// Declare metadata property for a class method (const) :
#define DR_CLASS_VOID_REFMETHOD_CONST(MethodName,TypeId,RefMethodId)    \
  DR_CLASS_REFMETHOD_CONST(MethodName,TypeId,MethodId,void)             \
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

// Declare metadata property for a class method :
#define DR_CLASS_REFMETHOD1(MethodName,TypeId,RefMethodId,ReturnType,Arg0Type) \
  function(MethodName,                                                  \
           boost::function< ReturnType ( TypeId & , Arg0Type )>         \
           (boost::bind(RefMethodId , _1, _2)))                         \
    /**/

// Declare metadata property for a class method :
#define DR_CLASS_VOID_REFMETHOD1(MethodName,TypeId,RefMethodId,Arg0Type) \
  DR_CLASS_REFMETHOD1(MethodName,TypeId,RefMethodId,void,Arg0Type)      \
  /**/

// Declare metadata property for a class method (const) :
#define DR_CLASS_REFMETHOD1_CONST(MethodName,TypeId,RefMethodId,ReturnType,Arg0Type) \
  function(MethodName,                                                  \
           boost::function< ReturnType ( const TypeId & , Arg0Type )>   \
           (boost::bind(RefMethodId , _1, _2 )))                        \
  /**/

// Declare metadata property for a class method (const) :
#define DR_CLASS_VOID_REFMETHOD1_CONST(MethodName,TypeId,RefMethodId,Arg0Type) \
  DR_CLASS_REFMETHOD1_CONST(MethodName,TypeId,RefMethodId,void,Arg0Type) \
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

// Declare metadata property for a class method :
#define DR_CLASS_REFMETHOD2(MethodName,TypeId,RefMethodId,ReturnType,Arg0Type,Arg1Type) \
  function(MethodName,                                                  \
           boost::function< ReturnType ( TypeId & , Arg0Type , Arg1Type )> \
           (boost::bind(RefMethodId , _1, _2, _3)))                     \
  /**/

// Declare metadata property for a class method :
#define DR_CLASS_VOID_REFMETHOD2(MethodName,TypeId,RefMethodId,Arg0Type,Arg1Type) \
  DR_CLASS_REFMETHOD2(MethodName,TypeId,RefMethodId,void,Arg0Type,Arg1Type) \
  /**/

// Declare metadata property for a class method (const) :
#define DR_CLASS_REFMETHOD2_CONST(MethodName,TypeId,RefMethodId,ReturnType,Arg0Type,Arg1Type) \
  function(MethodName,                                                  \
           boost::function< ReturnType ( const TypeId & , Arg0Type , Arg1Type )> \
           (boost::bind(RefMethodId , _1, _2, _3 )))                    \
  /**/

// Declare metadata property for a class method (const) :
#define DR_CLASS_VOID_REFMETHOD2_CONST(MethodName,TypeId,RefMethodId,Arg0Type,Arg1Type) \
  DR_CLASS_REFMETHOD2_CONST(MethodName,TypeId,RefMethodId,void,Arg0Type,Arg1Type) \
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
#define DR_CLASS_VOID_METHOD3_CONST(MethodName,TypeId,RefMethodId,Arg0Type,Arg1Type,Arg2Type) \
  DR_CLASS_METHOD3_CONST(MethodName,TypeId,RefMethodId,void,Arg0Type,Arg1Type,Arg2Type) \
  /**/

// Declare metadata property for a class method :
#define DR_CLASS_REFMETHOD3(MethodName,TypeId,RefMethodId,ReturnType,Arg0Type,Arg1Type,Arg2Type) \
  function(MethodName,                                                  \
           boost::function< ReturnType ( TypeId & , Arg0Type , Arg1Type , Arg2Type )> \
           (boost::bind(RefMethodId , _1, _2, _3, _4)))                 \
  /**/

// Declare metadata property for a class method :
#define DR_CLASS_VOID_REFMETHOD3(MethodName,TypeId,RefMethodId,Arg0Type,Arg1Type,Arg2Type) \
  DR_CLASS_REFMETHOD3(MethodName,TypeId,RefMethodId,void,Arg0Type,Arg1Type,Arg2Type) \
  /**/

// Declare metadata property for a class method (const) :
#define DR_CLASS_REFMETHOD3_CONST(MethodName,TypeId,RefMethodId,ReturnType,Arg0Type,Arg1Type,Arg2Type) \
  function(MethodName,                                                  \
           boost::function< ReturnType ( const TypeId & , Arg0Type , Arg1Type , Arg2Type )> \
           (boost::bind(RefMethodId , _1, _2, _3, _4 )))                \
    /**/

// Declare metadata property for a class method (const) :
#define DR_CLASS_VOID_REFMETHOD3_CONST(MethodName,TypeId,RefMethodId,Arg0Type,Arg1Type,Arg2Type) \
  DR_CLASS_REFMETHOD3_CONST(MethodName,TypeId,RefMethodId,void,Arg0Type,Arg1Type,Arg2Type) \
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

// Declare metadata property for a class method :
#define DR_CLASS_REFMETHOD4(MethodName,TypeId,RefMethodId,ReturnType,Arg0Type,Arg1Type,Arg2Type,Arg3Type) \
  function(MethodName,                                                  \
           boost::function< ReturnType ( TypeId & , Arg0Type , Arg1Type , Arg2Type , Arg3Type )> \
           (boost::bind(RefMethodId , _1, _2, _3, _4, _5)))             \
  /**/

// Declare metadata property for a class method :
#define DR_CLASS_VOID_REFMETHOD4(MethodName,TypeId,RefMethodId,Arg0Type,Arg1Type,Arg2Type,Arg3Type) \
  DR_CLASS_REFMETHOD4(MethodName,TypeId,RefMethodId,void,Arg0Type,Arg1Type,Arg2Type,Arg3Type) \
  /**/

// Declare metadata property for a class method (const) :
#define DR_CLASS_REFMETHOD4_CONST(MethodName,TypeId,RefMethodId,ReturnType,Arg0Type,Arg1Type,Arg2Type,Arg3Type) \
  function(MethodName,                                                  \
           boost::function< ReturnType ( const TypeId & , Arg0Type , Arg1Type , Arg2Type , Arg3Type )> \
           (boost::bind(RefMethodId , _1, _2, _3, _4, _5 )))            \
  /**/

// Declare metadata property for a class method (const) :
#define DR_CLASS_VOID_REFMETHOD4_CONST(MethodName,TypeId,RefMethodId,Arg0Type,Arg1Type,Arg2Type,Arg3Type) \
  DR_CLASS_REFMETHOD4_CONST(MethodName,TypeId,RefMethodId,void,Arg0Type,Arg1Type,Arg2Type,Arg3Type) \
  /**/

// Declare introspection metadata for an enum :
#define DR_ENUM_DECLARE2(TypeId,TypeName)       \
  ::camp::Enum::declare< TypeId >(TypeName)     \
  /**/

// Declare introspection metadata for a enum :
#define DR_ENUM_DECLARE(TypeId)                                         \
  DR_ENUM_DECLARE2( TypeId , ::datatools::detail::reflection::guid< TypeId >() ) \
  /**/


/************************************************************************/

// Should we use this aliases/macros ?
// They probably obfuscate the code...

/// \def DR_CLASS
/// \deprecated
#define DR_CLASS           camp::Class

/// \def DR_ENUM
/// \deprecated
#define DR_ENUM            camp::Enum

/// \def DR_CLASS_BY_NAME
/// \deprecated
#define DR_CLASS_BY_NAME   camp::classByName

/// \def DR_CLASS_BY_TYPE
/// \deprecated
#define DR_CLASS_BY_TYPE   camp::classByType

/// \def DR_CLASS_BY_INDEX
/// \deprecated
#define DR_CLASS_BY_INDEX  camp::classByIndex

/// \def DR_CLASS_BY_OBJECT
/// \deprecated
#define DR_CLASS_BY_OBJECT camp::classByObject

/// \def DR_ENUM_BY_NAME
/// \deprecated
#define DR_ENUM_BY_NAME    camp::enumByName

/// \def DR_ENUM_BY_TYPE
/// \deprecated
#define DR_ENUM_BY_TYPE    camp::enumByType

/// \def DR_ENUM_BY_INDEX
/// \deprecated
#define DR_ENUM_BY_INDEX   camp::enumByIndex

/// \def DR_ENUM_BY_OBJECT
/// \deprecated
#define DR_ENUM_BY_OBJECT  camp::enumByObject

/// \def DR_OBJECT
/// \deprecated
#define DR_OBJECT        camp::UserObject

/// \def DR_VALUE
/// \deprecated
#define DR_VALUE         camp::Value

/// \def DR_ARGS
/// \deprecated
#define DR_ARGS          camp::Args

/// \def DR_FUNCTION
/// \deprecated
#define DR_FUNCTION      camp::Function

/// \def DR_SET
/// \deprecated
#define DR_SET           set

/// \def DR_CALL
/// \deprecated
#define DR_CALL          call

/// \def DR_GET
/// \deprecated
#define DR_GET           get

/// \def DR_NOTYPE
/// \deprecated
#define DR_NOTYPE        camp::noType

/// \def DR_BOOLEANTYPE
/// \deprecated
#define DR_BOOLEANTYPE   camp::boolType

/// \def DR_INTEGERTYPE
/// \deprecated
#define DR_INTEGERTYPE   camp::intType

/// \def DR_REALTYPE
/// \deprecated
#define DR_REALTYPE      camp::realType

/// \def DR_STRINGTYPE
/// \deprecated
#define DR_STRINGTYPE    camp::stringType

/// \def DR_ENUMTYPE
/// \deprecated
#define DR_ENUMTYPE      camp::enumType

/// \def DR_ARRAYTYPE
/// \deprecated
#define DR_ARRAYTYPE     camp::arrayType

/// \def DR_USERTYPE
/// \deprecated
#define DR_USERTYPE      camp::userType

/// \def DR_ENUM_VALUE
/// \deprecated
#define DR_ENUM_VALUE    value

/// \def DR_ENUM_NAME
/// \deprecated
#define DR_ENUM_NAME     name

/// \def DR_ENUM_SIZE
/// \deprecated
#define DR_ENUM_SIZE     size

#define DR_ENUM_PAIR(PairName,PairValue)        \
  value( PairName, PairValue )                  \
  /**/

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

/// \def DR_CLASS_VERSION(T, N)
/// \brief Specify the current version number \a N for the class \a T
///        version numbers limited to 8 bits
///
/// @arg \a T the type full identifier (with namespaces)
/// @arg \a N the version number
///
/// This macro must be invoked outside of any namespace scope.
/** Example:
    @code{.cpp}
namespace foo {
  class bar {
    ...
    int val;   // Legacy attribute (original version 0)
    int other; // Attribute added in version 1
  };
}
DR_CLASS_VERSION(foo::bar, 1);
    @endcode
 */
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
                            boost::mpl::int_<256> >));                  \
        };                                                              \
      }}}                                                               \
  /**/

#endif // DATATOOLS_REFLECTION_MACROS_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
