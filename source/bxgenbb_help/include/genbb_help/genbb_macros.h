/// \file genbb_help/genbb_macros.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2012-04-24
 * Last modified : 2016-02-25
 */

/* \brief Macros to automatically generate some parts of the particle generator class interface.
 *
 */

#ifndef GENBB_GENBB_MACROS_H
#define GENBB_GENBB_MACROS_H 1

// This project:
#include <genbb_help/i_genbb.h>
#include <genbb_help/base_decay_driver.h>

/* Macros for interface/implementation of static creator methods in particle generator classes */

/** Registration */

#define GENBB_PG_REGISTRATION_INTERFACE(GENBB_CLASS_NAME)               \
  private:                                                              \
  DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_INTERFACE (::genbb::i_genbb,GENBB_CLASS_NAME); \
  /**/

#define GENBB_PG_REGISTRATION_IMPLEMENT(GENBB_CLASS_NAME,GENBB_ID)      \
  DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_IMPLEMENTATION (::genbb::i_genbb,GENBB_CLASS_NAME,GENBB_ID); \
  /**/

/* Macros for interface/implementation of static creator methods in decay driver classes */

/** Registration */

#define GENBB_BDD_REGISTRATION_INTERFACE(GENBB_CLASS_NAME)              \
  private:                                                              \
  DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_INTERFACE (::genbb::base_decay_driver,GENBB_CLASS_NAME); \
  /**/

#define GENBB_BDD_REGISTRATION_IMPLEMENT(GENBB_CLASS_NAME,GENBB_ID)     \
  DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_IMPLEMENTATION (::genbb::base_decay_driver,GENBB_CLASS_NAME,GENBB_ID); \
  /**/

/* Macros for interface/implementation of static creator methods in Lorentz Boost generator classes */

/** Registration */

#define GENBB_LBG_REGISTRATION_INTERFACE(GENBB_CLASS_NAME)              \
  private:                                                              \
  DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_INTERFACE (::genbb::i_lorentz_boost_generator,GENBB_CLASS_NAME); \
  /**/

#define GENBB_LBG_REGISTRATION_IMPLEMENT(GENBB_CLASS_NAME,GENBB_ID)     \
  DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_IMPLEMENTATION (::genbb::i_lorentz_boost_generator,GENBB_CLASS_NAME,GENBB_ID); \
  /**/

#endif // GENBB_GENBB_MACROS_H

// Local Variables: --
// mode: c++ --
// End: --
