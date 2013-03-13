// -*- mode: c++; -*- 
/* chemical_symbol.h
 * Author(s):  Benoit Guillon <guillon@lpccaen.in2p3.fr>
 * Creation date: 2008-02-25
 * Last modified: 2008-02-25
 * 
 * License: 
 * 
 * 
 * Description: 
 *   - Very simple header to handle equivalence between the chemical symbol and the number of proton. 
 *   - 2 inline functions :
 *      > symbol_from_Z (int) : return the chemical symbol (string) corresponding to the number of protons (Z)
 *      > Z_from_symbol (string) : return the number of proton (Z) according to the chemical symbol (string) 
 * History: 
 * 
 *
 *  Author:
 *   - Benoit Guillon <guillon@lpccaen.in2p3.fr>
 *
 *  Mainteners:
 *   - Benoit Guillon <guillon@lpccaen.in2p3.fr>
 *
 */

#ifndef MATERIALS_CHEMICAL_SYMBOL_H_
#define MATERIALS_CHEMICAL_SYMBOL_H_ 1

#include <string>

namespace materials {

  class chemical_symbol 
  {
  public:
    
    static const unsigned int NB_CHEMICAL_SYMBOLS = 119;
  
    static const std::string table[NB_CHEMICAL_SYMBOLS];

    static int z_from_symbol (const std::string & symbol_);
    
    static const std::string & symbol_from_z (int z_);

  };

} // end of namespace materials

#endif // MATERIALS_CHEMICAL_SYMBOL_H_

// end of chemical_symbol.h
