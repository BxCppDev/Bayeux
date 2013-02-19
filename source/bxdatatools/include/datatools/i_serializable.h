// -*- mode: c++; -*- 
/* i_serializable.h */
/* 
 * Copyright (C) 2011-2012 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
#ifndef DATATOOLS_I_SERIALIZABLE_H_
#define DATATOOLS_I_SERIALIZABLE_H_

// Standard Library
#include <string>
#include <typeinfo>

// Third Party
// - Boost
#include <boost/serialization/access.hpp>
#include <boost/type_traits.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/mpl/has_xxx.hpp>
#include <boost/scoped_ptr.hpp>

// Datatools
#include <datatools/datatools_config.h>
#include <datatools/serialization_macros.h>
#include <datatools/factory_macros.h>
#include <datatools/utils.h>

// Datatools reflection utilities :
#if DATATOOLS_WITH_REFLECTION == 1
#include <datatools/reflection_macros.h>
#endif

/// Main namespace of the datatools library 
namespace datatools {

  //! \brief Base abstract class of all serializable (and possibly introspectable) classes
  class i_serializable {

  public:

    i_serializable() {};

    virtual ~i_serializable() {}

    virtual const std::string& get_serial_tag() const = 0;

    BOOST_SERIALIZATION_BASIC_DECLARATION ();

    // Factory stuff :
    DATATOOLS_FACTORY_SYSTEM_REGISTER_INTERFACE(i_serializable);
    
#if DATATOOLS_WITH_REFLECTION == 1
    DR_CLASS_RTTI();
#endif
    
  };

} // end of namespace datatools 

#if DATATOOLS_WITH_REFLECTION == 1
//#warning Activate reflection layer for the 'datatools::i_serializable' class.
DR_CLASS_INIT(::datatools::i_serializable);
#endif // DATATOOLS_WITH_REFLECTION



/******************
 * Helpful macros *
 ******************/


/** Macro invoked to make a class serializable by inheriting the
 *  ::datatools::i_serializable interface. This
 *  mechanism enables the serialization through base pointer
 *  with the Boost/Serialization library. It is used by the
 *  'datatools::utils::things' class.
 *
 * Example:
 * \code          
 * class Foo : DATATOOLS_SERIALIZABLE_CLASS , ...
 * {
 *   ...
 * };
 * \endcode
 */
#define DATATOOLS_SERIALIZABLE_CLASS            \
  public datatools::i_serializable              \
  /**/


namespace datatools {                                                   
  template <class T>
  const std::string & serial_tag ();
}

/// Template support for serializable type (backward compatibility support) 
BOOST_MPL_HAS_XXX_TRAIT_NAMED_DEF(has_bsts, backward_serial_tag_support, false)

namespace datatools {                                                   
  template <class T>
  const std::string & backward_serial_tag (int i = 0);
}

#define DATATOOLS_SERIALIZATION_EXT_SERIAL_TAG_DECLARATION(ClassName)   \
  namespace datatools {                                                 \
    template <>                                                         \
    const std::string & serial_tag< ClassName > ();                     \
  }                                                                     \
  /**/


/** Intrusive macro invoked from the class declaration to 
 *  declare serial tag support :
 * Example:
 * \code        
 * class Foo : DATATOOLS_SERIALIZABLE_CLASS()
 * {
 *   DATATOOLS_SERIALIZATION_SERIAL_TAG_DECLARATION ()
 * };
 * \endcode
 */
#define DATATOOLS_SERIALIZATION_SERIAL_TAG_DECLARATION()        \
  public:                                                       \
  static const std::string SERIAL_TAG;                          \
  virtual const std::string& get_serial_tag() const;            \
  /**/


/** Macro invoked to implement serial tag support :
 * Example:
 * \code        
 * class Foo : DATATOOLS_SERIALIZABLE_CLASS()
 * {
 *   DATATOOLS_SERIALIZATION_SERIAL_TAG_DECLARATION ()
 * };
 * DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION (::Foo, "Foo");
 * \endcode
 */
#define DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(ClassName,ClassSerialTag) \
  const std::string ClassName::SERIAL_TAG = ClassSerialTag;             \
  const std::string& ClassName::get_serial_tag() const {                \
    return ClassName::SERIAL_TAG;                                       \
  }                                                                     \
  /**/


#define DATATOOLS_SERIALIZATION_EXT_SERIAL_TAG_IMPLEMENTATION(ClassName,ClassSerialTag) \
  namespace datatools {                                                 \
    template <>                                                         \
    const std::string & serial_tag<ClassName> (){                       \
      static boost::scoped_ptr<std::string> g_serial_tag (0);           \
      if ( !g_serial_tag){                                              \
        g_serial_tag.reset (new std::string (ClassSerialTag));          \
      }                                                                 \
      return *g_serial_tag.get ();                                      \
    }                                                                   \
  }                                                                     \
  /**/


/** Intrusive macro invoked from the class declaration to 
 *  declare backward compatibility serial tag support :
 * Example:
 * \code        
 * class Foo : DATATOOLS_SERIALIZABLE_CLASS()
 * {
 *   DATATOOLS_SERIALIZATION_SERIAL_TAG_DECLARATION ()
 *   DATATOOLS_SERIALIZATION_BACKWARD_SERIAL_TAG_DECLARATION ()
 * };
 * \endcode
 */
#define DATATOOLS_SERIALIZATION_BACKWARD_SERIAL_TAG_SUPPORT()   \
  public:                                                       \
  struct backward_serial_tag_support {};                        \
  /**/


#define DATATOOLS_SERIALIZATION_EXT_BACKWARD_SERIAL_TAG_DECLARATION(ClassName) \
  namespace datatools {                                                 \
    template <>                                                         \
    const std::string & backward_serial_tag< ClassName > (int);         \
  }                                                                     \
  /**/


#define DATATOOLS_SERIALIZATION_EXT_BACKWARD_SERIAL_TAG_IMPLEMENTATION(ClassName,ClassBackwardSerialTag) \
  namespace datatools {                                                 \
    template <>                                                         \
    const std::string & backward_serial_tag<ClassName> (int i){         \
      static boost::scoped_ptr<std::string> g_backward_serial_tag (0);  \
      if ( !g_backward_serial_tag){                                     \
        g_backward_serial_tag.reset (new std::string (ClassBackwardSerialTag)); \
      }                                                                 \
      return *g_backward_serial_tag.get ();                             \
    }                                                                   \
  }                                                                     \
  /**/


/** Macro invoked to specify the inheritance to the 
 *  ::datatools::i_serializable within the
 * serialization method :
 *           
 * Example:
 * \code          
 * class Foo : DATATOOLS_SERIALIZABLE_CLASS()
 * {
 *   ...
 * };
 * template<class Archive>
 * void Foo::serialize(Archive& ar, const unsigned int version) {
 *    ar & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
 *    ...
 * }
 * \endcode
 */
#define DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP          \
  boost::serialization::make_nvp("datatool__i_serializable",            \
                                 boost::serialization::base_object<datatools::i_serializable>(*this)) \
                                                                        \
  /**/


/** Macro invoked to specify the inheritance to the 
 *  ::datatools::i_serializable within the
 * serialization method (this is the original version, now obsolete) :
 *           
 * Example:
 * \code          
 * class Foo : DATATOOLS_SERIALIZABLE_CLASS
 * {
 *   ...
 * };
 * template<class Archive>
 * void Foo::serialize(Archive& ar, const unsigned int version) {
 *   if (version <= ThresholdVersion) {
 *     ar & DATATOOLS_SERIALIZATION_OLD_I_SERIALIZABLE_BASE_OBJECT_NVP;
 *   }
 *   else {
 *     ar & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
 *   } 
 *    ...
 * }
 * \endcode
 */
#define DATATOOLS_SERIALIZATION_OLD_I_SERIALIZABLE_BASE_OBJECT_NVP      \
  boost::serialization::make_nvp("datatools__serialization__i_serializable", \
                                 boost::serialization::base_object<datatools::i_serializable>(*this)) \
                                                                        \
  /**/


/** Intrusive macro invoked from the class declaration to activate 
 *  basic serialization support for the class :
 * 
 * Example:
 * \code          
 *   class Foo : public ::datatools::serialization::i_serializable
 *   {
 *      DATATOOLS_SERIALIZATION_DECLARATION();
 *   };
 * \endcode
 */
#define DATATOOLS_SERIALIZATION_DECLARATION()           \
  public:                                               \
  DATATOOLS_SERIALIZATION_SERIAL_TAG_DECLARATION()      \
  BOOST_SERIALIZATION_BASIC_DECLARATION ()              \
  /**/

/** Shortcut macro to generate the proper prototype of the
 * Boost serialization method :
 * @param ClassName The class name (without
 * @param ArchiveId The archive identifier
 * @param VersionId The version version number 
 * Example:
 * \code 
 * namespace Bar {         
 *   class Foo : DATATOOLS_SERIALIZABLE_CLASS()
 *   {
 *     DATATOOLS_SERIALIZATION_DECLARATION_BASIC()
 *   };
 * }
 * namespace Bar {       
 *   DATATOOLS_SERIALIZATION_SERIALIZE_IMPLEMENT_HEADER (Foo, ar, version)
 *   {
 *     ar & ...;
 *   }
 * }
 * \endcode
 */
#define DATATOOLS_SERIALIZATION_SERIALIZE_IMPLEMENT_HEADER(ClassName,ArchiveId,VersionId) \
  template<class Archive>                                               \
  void ClassName::serialize(Archive& ArchiveId, const unsigned int VersionId) \
  /**/


/** Intrusive macro invoked from the class declaration to 
 *  declare the automatic registration the class in a dedicated factory :
 *  @param ClassName The class name (possibly with its full namespace path prefix by ::)         
 * Example:
 * \code         
 * class Foo : DATATOOLS_SERIALIZABLE_CLASS
 * {
 *   DATATOOLS_SERIALIZATION_FACTORY_INTERFACE (::Foo)
 * };
 * \endcode
 */
#define DATATOOLS_SERIALIZATION_FACTORY_INTERFACE(ClassName)            \
  DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_INTERFACE(::datatools::i_serializable,ClassName) \
  /**/


/** Macro invoked to implement the automatic registration the class 
 *  in a dedicated factory :
 *  @param ClassName      The class name (possibly with its full namespace path prefix by ::)  
 *  @param ClassSerialTag The unique class ID used by the factory (including full namespace prefix)
 * Example:
 * \code        
 * class Foo : DATATOOLS_SERIALIZABLE_CLASS
 * {
 *   DATATOOLS_SERIALIZATION_FACTORY_INTERFACE (::Foo)
 * };
 * DATATOOLS_SERIALIZATION_FACTORY_IMPLEMENTATION (::Foo, "Foo")
 * \endcode
 */
#define DATATOOLS_SERIALIZATION_FACTORY_IMPLEMENTATION(ClassName,ClassSerialTag) \
  DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_IMPLEMENTATION(::datatools::i_serializable,ClassName,ClassSerialTag) \
  /**/


/** Intrusive macro invoked to implement basic serialization support for the class :
 *  @param ClassName The class name (preferably with full namespace path prefixed by ::)
 *  @param ClassSerialTag The unique class serialization ID (tag)
 *
 * Example:
 * \code          
 *   class Foo : DATATOOLS_SERIALIZABLE_CLASS
 *   {
 *      DATATOOLS_SERIALIZATION_DECLARATION();
 *   };
 *
 * DATATOOLS_SERIALIZATION_IMPLEMENTATION (::Foo, "Foo")
 * \endcode
 *
 */
#define DATATOOLS_SERIALIZATION_IMPLEMENTATION(ClassName,ClassSerialTag) \
  DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION (ClassName,ClassSerialTag) \
  /**/


/** Intrusive macro invoked from the class declaration to activate 
 *  advanced serialization support for the class :
 *  @param ClassName The class name (without namespace)
 *           
 * Example:
 * \code          
 * class Foo : DATATOOLS_SERIALIZABLE_CLASS
 * {
 *    DATATOOLS_SERIALIZATION_DECLARATION_ADVANCED(Foo);
 * };
 * namespace James {
 *   class Bond : DATATOOLS_SERIALIZABLE_CLASS
 *   { 
 *     DATATOOLS_SERIALIZATION_DECLARATION_ADVANCED(Bond);
 *   };
 * }
 * \endcode
 */
#define DATATOOLS_SERIALIZATION_DECLARATION_ADVANCED(ClassName) \
  DATATOOLS_SERIALIZATION_DECLARATION()                         \
  DATATOOLS_SERIALIZATION_FACTORY_INTERFACE(ClassName)          \
  /**/


/** Intrusive macro invoked to implement advanced serialization support for the class :
 *  @param ClassName The class name (preferably with full namespace path prefixed by ::)
 *  @param ClassSerialTag The unique class serialization ID (tag)
 *
 * Example:
 * \code          
 * class Foo : DATATOOLS_SERIALIZABLE_CLASS
 * {
 *    DATATOOLS_SERIALIZATION_DECLARATION_ADVANCED(::Foo);
 * };
 * DATATOOLS_SERIALIZATION_IMPLEMENTATION_ADVANCED (::Foo, "Foo")
 * namespace James {
 *   class Bond : DATATOOLS_SERIALIZABLE_CLASS
 *   { 
 *     DATATOOLS_SERIALIZATION_DECLARATION_ADVANCED(::James::Bond);
 *   };
 * DATATOOLS_SERIALIZATION_IMPLEMENTATION_ADVANCED (::James::Bond, "James::Bond")
 * }
 * \endcode
 *
 */
#define DATATOOLS_SERIALIZATION_IMPLEMENTATION_ADVANCED(ClassName,ClassSerialTag) \
  DATATOOLS_SERIALIZATION_IMPLEMENTATION (ClassName,ClassSerialTag)     \
  DATATOOLS_SERIALIZATION_FACTORY_IMPLEMENTATION (ClassName,ClassSerialTag) \
  /**/


// Utilities for serial tag checking :
namespace datatools {

  template <typename T>
  bool check_serial_tag(const std::string stag_, 
                        const std::string alt_tag_ = "",
                        typename boost::disable_if< has_bsts<T> >::type* dummy = 0) {
    if (stag_ == T::SERIAL_TAG) return true;
    if (! alt_tag_.empty())
      {
        if (stag_ == alt_tag_) return true;
      }
    return false;
  }
  
  template <typename T>
  bool check_serial_tag(const std::string stag_, 
                        const std::string alt_tag_ = "",
                        typename boost::enable_if< has_bsts<T> >::type* dummy = 0) {
    if (stag_ == T::SERIAL_TAG) return true;
    if (stag_ == ::datatools::backward_serial_tag<T> (0)) return true;
    if (! alt_tag_.empty())
      {
        if (stag_ == alt_tag_) return true;
      }
    return false;
  }

}

#endif // DATATOOLS_I_SERIALIZABLE_H_

