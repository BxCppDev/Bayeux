/* datatools/factory/factory_macros.h */
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 *               : Ben Morgan <Ben.Morgan@warwick.ac.uk>
 * Creation date : 2012-03-19
 * Last modified : 2012-03-19
 *
 */
#ifndef DATATOOLS_FACTORY_FACTORY_MACROS_H_
#define DATATOOLS_FACTORY_FACTORY_MACROS_H_

// Standard Library

// Third Party
#include <boost/preprocessor/stringize.hpp>

// Datatools
#include <datatools/factory/factory.h>

/**
 * These macros provide some automated mechanisms to :
 *  - setup a global factory register associated to a given base class
 *  - automate the registration of the factory associated to any daughter class
 *    in the global factory register associated to the mother base class
 *
 * \code
 *
 * class Base {
 *   DATATOOLS_FACTORY_INTERFACE(Base)
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

//
// #define DATATOOLS_FACTORY_CLASS_TYPEID (T, K)          
// namespace datatools {                              
// namespace factory {                      
// template<>                                     
// struct typeid_defined< T > : boost::mpl::true_ {}; 
// template<>                                     
// inline const char * id< T >(){               
//     return K;                                  
// }                                              
// } /* factory */                          
// } /* datatools */                                  
//

/// Alias for a factory register associated to a base class
#define DATATOOLS_FACTORY_INTERFACE(BaseType)                                     \
  public:                                                                         \
    typedef datatools::factory::factory_register<BaseType> factory_register_type; \
  /**/

/// Declaration of a system (allocator/functor) factory register as a static member of a base class and static accessor methods
#define DATATOOLS_FACTORY_SYSTEM_REGISTER_INTERFACE(BaseType)                      \
  DATATOOLS_FACTORY_INTERFACE(BaseType)                                         \
  public:                                                                          \
    typedef boost::scoped_ptr<factory_register_type> scoped_factory_register_type; \
    static factory_register_type& grab_system_factory_register();                \
    static const factory_register_type& get_system_factory_register();           \
    /**/

/// Instantiate the system (allocator/functor) factory register and its associated accessors
#define DATATOOLS_FACTORY_SYSTEM_REGISTER_IMPLEMENTATION(BaseType, RegisterLabel)                 \
  BaseType::factory_register_type& BaseType::grab_system_factory_register()                     \
  {                                                                                               \
    static scoped_factory_register_type g_system_factory_register(0);                            \
    if (g_system_factory_register.get() == 0)                                                    \
      {                                                                                           \
        g_system_factory_register.reset(new BaseType::factory_register_type(RegisterLabel, 0)); \
      }                                                                                           \
    return *(g_system_factory_register.get());                                                   \
  }                                                                                               \
  const BaseType::factory_register_type& BaseType::get_system_factory_register()                \
  {                                                                                               \
    BaseType::factory_register_type& fr = BaseType::grab_system_factory_register();             \
    return const_cast<BaseType::factory_register_type&>(fr);                                    \
  }                                                                                               \
  /**/

/// Useful macros
#define DATATOOLS_FACTORY_GRAB_SYSTEM_REGISTER(BaseType) \
  BaseType::grab_system_factory_register()              \
  /**/

#define DATATOOLS_FACTORY_GET_SYSTEM_REGISTER(BaseType) \
  BaseType::get_system_factory_register()              \
  /**/

/// Automated registration for derived classes:
#define DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_INTERFACE(BaseType, DerivedType)                             \
  private:                                                                                                      \
  static datatools::factory::_system_factory_registrator<BaseType,DerivedType> _g_system_factory_registration_; \
  static std::string get_system_factory_auto_registration_id();                                                \
  /**/

#define DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_IMPLEMENTATION(BaseType, DerivedType, TypeId)                               \
  datatools::factory::_system_factory_registrator<BaseType,DerivedType> DerivedType::_g_system_factory_registration_(TypeId); \
  std::string DerivedType::get_system_factory_auto_registration_id()                                                          \
  {                                                                                                                            \
    return std::string(TypeId);                                                                                               \
  }                                                                                                                            \
  /**/

#endif // DATATOOLS_FACTORY_FACTORY_MACROS_H_
