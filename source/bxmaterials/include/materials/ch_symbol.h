// -*- mode: c++; -*- 
/* ch_symbol.h
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

#ifndef __materials__ch_symbol_h
#define __materials__ch_symbol_h 1

#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>

using namespace std;

namespace mat {

  class chemical_symbol 
  {
  public:
    
    static const unsigned int NB_CHEMICAL_SYMBOLS = 119;
  
    static const string table[NB_CHEMICAL_SYMBOLS];

    static int Z_from_symbol (const string & symbol_);
    
    static const string & symbol_from_Z (int Z_);

  };

} // end of namespace mat

#endif // __materials__ch_symbol_h

// end of ch_symbol.h
