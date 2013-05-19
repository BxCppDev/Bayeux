/* things_macros.h
 * Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2012-03-31
 * Last modified : 2013-04-22
 */
#ifndef DATATOOLS_THINGS_MACROS_H_
#define DATATOOLS_THINGS_MACROS_H_
// Standard Library

// Third Party
// - A

// This Project
#include <datatools/things.h>

#define DATATOOLS_THINGS_ADD_BANK(ThingsVar,BankName,BankType,BankVar)  \
  BankType& BankVar = ThingsVar.add<BankType>(BankName);
/**/

#define DATATOOLS_THINGS_CHECK_BANK(ThingsVar,BankName,BankType)        \
  (ThingsVar.has(BankName) && ThingsVar.is_a<BankType>(BankName))
/**/

#define DATATOOLS_THINGS_CONST_BANK(ThingsVar,BankName,BankType,BankVar) \
  {                                                                     \
    DT_THROW_IF (!ThingsVar.is_a<BankType>(BankName),                   \
                 std::logic_error,                                      \
                 "No bank named '" << BankName << "' in '" << BOOST_PP_STRINGIZE(HandleVar) << "' !"); \
  }                                                             \
  const BankType& BankVar = ThingsVar.get<BankType>(BankName);
/**/

#define DATATOOLS_THINGS_MUTABLE_BANK(ThingsVar,BankName,BankType,BankVar) \
  {                                                                     \
    DT_THROW_IF (!ThingsVar.is_a<BankType>(BankName),                   \
                 std::logic_error,                                      \
                 "No bank named '" << BankName << "' in '" << BOOST_PP_STRINGIZE(HandleVar) << "' !"); \
  }                                                                     \
  BankType& BankVar = ThingsVar.grab<BankType>(BankName);
/**/

#endif // DATATOOLS_THINGS_MACROS_H_

