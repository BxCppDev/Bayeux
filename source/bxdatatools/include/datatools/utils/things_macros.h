/* things_macros.h
 * Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2012-03-31
 * Last modified : 2012-03-31
 */
#ifndef DATATOOLS_UTILS_THINGS_MACROS_H_
#define DATATOOLS_UTILS_THINGS_MACROS_H_
// Standard Library

// Third Party
// - A

// This Project
#include <datatools/utils/things.h>

#define DATATOOLS_UTILS_THINGS_ADD_BANK(ThingsVar,BankName,BankType,BankVar) \
  BankType& BankVar = ThingsVar.add<BankType>(BankName);                 \
  /**/

#define DATATOOLS_UTILS_THINGS_CHECK_BANK(ThingsVar,BankName,BankType) \
  (ThingsVar.has(BankName) && ThingsVar.is_a<BankType>(BankName))    \
  /**/


#define DATATOOLS_UTILS_THINGS_CONST_BANK(ThingsVar,BankName,BankType,BankVar)    \
  if (!ThingsVar.is_a<BankType>(BankName)) {                          \
    std::ostringstream message;                                       \
    message << "No bank named '" << BankName << "' in '" << "ThingsVar" << "' !"; \
    throw std::logic_error(message.str());                            \
  }                                                                  \
  const BankType& BankVar = ThingsVar.get<BankType>(BankName);		       \
  /**/

#define DATATOOLS_UTILS_THINGS_MUTABLE_BANK(ThingsVar,BankName,BankType,BankVar)  \
  if (!ThingsVar.is_a<BankType>(BankName)) {  \
    std::ostringstream message;                   \
    message << "No bank named '" << BankName << "' in '" << "ThingsVar" << "' !"; \
    throw std::logic_error(message.str());                         \
  }                                                               \
  BankType& BankVar = ThingsVar.grab<BankType>(BankName);	         \
  /**/

#endif // DATATOOLS_UTILS_THINGS_MACROS_H_

