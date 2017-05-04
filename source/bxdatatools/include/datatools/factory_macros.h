/// \file datatools/factory_macros.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 *               : Ben Morgan <Ben.Morgan@warwick.ac.uk>
 * Creation date : 2012-03-19
 * Last modified : 2012-12-15
 *
 */
#ifndef DATATOOLS_FACTORY_MACROS_H
#define DATATOOLS_FACTORY_MACROS_H

// Third Party:
#include <boost/preprocessor/stringize.hpp>

// This project:
#include <datatools/factory.h>

/**
 * These macros provide some automated mechanisms to :
 *  - setup a global factory register associated to a given base class
 *  - automate the registration of the factory associated to any daughter class
 *    in the global factory register associated to the mother base class
 *
 * \code
 *
 * class Base {
 *   DATATOOLS_FACTORY_SYSTEM_REGISTER_INTERFACE(Base)
 * };
 *
 * DATATOOLS_FACTORY_SYSTEM_REGISTER_IMPLEMENTATION(Base)
 *
 * class Derived1 : public Base {
 *   DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_INTERFACE(Base, Derived1)
 * };
 *
 * DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_IMPLEMENTATION(Base, Derived1, "Derived1")
 *
 * class Derived2 : public Base {
 *   DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_INTERFACE(Base, Derived2)
 * };
 *
 * DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_IMPLEMENTATION(Base, Derived2, "Derived2")
 * \endcode
 */

/// Alias for a factory register associated to a base class
#define DATATOOLS_FACTORY_INTERFACE(BaseType)                           \
  public:                                                               \
  typedef ::datatools::factory_register< BaseType > factory_register_type; \
  /**/

/// Declaration of a system (allocator/functor) factory register as a static member of a base class and static accessor methods
#define DATATOOLS_FACTORY_SYSTEM_REGISTER_INTERFACE(BaseType)           \
  DATATOOLS_FACTORY_INTERFACE(BaseType)                                 \
  public:                                                               \
  typedef ::boost::scoped_ptr< factory_register_type > scoped_factory_register_type; \
  static factory_register_type& grab_system_factory_register();         \
  static const factory_register_type& get_system_factory_register();    \
  /**/

/// Instantiate the system (allocator/functor) factory register and its associated accessors
#define DATATOOLS_FACTORY_SYSTEM_REGISTER_IMPLEMENTATION(BaseType, RegisterLabel) \
  BaseType::factory_register_type& BaseType::grab_system_factory_register() \
  {                                                                     \
    static scoped_factory_register_type _system_factory_register;       \
    if (!_system_factory_register) {                                    \
      _system_factory_register.reset(new BaseType::factory_register_type(RegisterLabel, 0)); \
    }                                                                   \
    return *_system_factory_register.get();                             \
  }                                                                     \
  const BaseType::factory_register_type& BaseType::get_system_factory_register() \
  {                                                                     \
    BaseType::factory_register_type& fr = BaseType::grab_system_factory_register(); \
    return const_cast< BaseType::factory_register_type& >(fr);          \
  }                                                                     \
  /**/

// Useful macros
#define DATATOOLS_FACTORY_GRAB_SYSTEM_REGISTER(BaseType)  \
  BaseType::grab_system_factory_register()                \
  /**/

#define DATATOOLS_FACTORY_GET_SYSTEM_REGISTER(BaseType) \
  BaseType::get_system_factory_register()               \
  /**/

/// Interface macro of the automated registration for derived classes
#define DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_INTERFACE(BaseType, DerivedType) \
  private:                                                              \
  static ::datatools::_system_factory_registrator< BaseType , DerivedType > _g_system_factory_auto_registration_; \
public:                                                                 \
 static const std::string & system_factory_auto_registration_id();      \
 /**/

/// Implementation macro of the automated registration for derived classes
#define DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_IMPLEMENTATION(BaseType, DerivedType, DerivedTypeId) \
  ::datatools::_system_factory_registrator< BaseType , DerivedType > DerivedType::_g_system_factory_auto_registration_(DerivedTypeId); \
  const std::string & DerivedType::system_factory_auto_registration_id() \
  {                                                                     \
    static const std::string _id(DerivedTypeId);                        \
    return _id;                                                         \
  }                                                                     \
  /**/

#endif // DATATOOLS_FACTORY_FACTORY_MACROS_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
