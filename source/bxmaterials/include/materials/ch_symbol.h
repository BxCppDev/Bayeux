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

namespace mat {
   
  static const string chemical_symbol[119]={"n", "H", "He", "Li", "Be", "B","C", "N", "O", "F", "Ne",			
   "Na", "Mg", "Al", "Si", "P", "S", "Cl", "Ar", "K", "Ca", "Sc", "Ti", "V", "Cr", "Mn", "Fe", 
   "Co", "Ni", "Cu", "Zn", "Ga", "Ge","As", "Se", "Br", "Kr", "Rb", "Sr", "Y", "Zr", "Nb", "Mo", 
   "Tc", "Ru", "Rh", "Pd", "Ag", "Cd", "In", "Sn", "Sb", "Te", "I", "Xe","Cs", "Ba", "La", "Ce", 
   "Pr", "Nd", "Pm", "Sm", "Eu", "Gd", "Tb","Dy", "Ho", "Er", "Tm", "Yb", "Lu", "Hf", "Ta", 
   "W", "Re", "Os","Ir", "Pt", "Au", "Hg", "Tl", "Pb", "Bi", "Po", "At", "Rn", "Fr", "Ra","Ac", 
   "Th", "Pa", "U", "Np", "Pu", "Am", "Cm", "Bk", "Cf", "Es", "Fm", "Md", "No", "Lr", "Rf", 
   "Db", "Sg", "Bh", "Hs", "Mt", "Ds",  "Rg", "Ec", "Ed", "Ee", "Ef", "Eg", "Eh", "Ei" };

   static inline const int Z_from_symbol(const string & symbol_)
    {
       bool is_symbol_found = false; 
					    
       int i_Z=0;
    
      while(!is_symbol_found && i_Z < 119) 
        {
        if(symbol_ == chemical_symbol[i_Z]) is_symbol_found = true;
        i_Z++;
        }
      
        if(!is_symbol_found)
          {
	    ostringstream message;
	    message << endl<<"ch_symbol::Z_from_symbol(): '" << symbol_<<"' not founded !"<<endl;
	    throw runtime_error (message.str ());
	 }	      
      else return(i_Z-1);
    }
    
    

   static const inline string symbol_from_Z(const int Z_)
    {
      if(  Z_ >= 1  &&  Z_ < 119)
       {
        return chemical_symbol[Z_];
       }
     else
      {
	    ostringstream message;
	    message << endl<< "mat::ch_symbol::symbol_from_Z:  Z value : '" << Z_<<"' not tabulated !"<<endl;
	    throw runtime_error (message.str ());
       }
   };

} // end of namespace mat

#endif // __materials__ch_symbol_h

// end of ch_symbol.h
