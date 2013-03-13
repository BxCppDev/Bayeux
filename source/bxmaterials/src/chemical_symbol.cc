// -*- mode: c++ ; -*- 
/* chemical_symbol.cc
 */

#include <materials/chemical_symbol.h>

#include <iostream>
#include <sstream>
#include <stdexcept>

namespace materials {

  using namespace std;

  const string chemical_symbol::table[chemical_symbol::NB_CHEMICAL_SYMBOLS] = {
    "n", "H", "He", "Li", "Be", "B","C", "N", "O", "F", "Ne",                   
    "Na", "Mg", "Al", "Si", "P", "S", "Cl", "Ar", "K", "Ca", "Sc", "Ti", "V", "Cr", "Mn", "Fe", 
    "Co", "Ni", "Cu", "Zn", "Ga", "Ge","As", "Se", "Br", "Kr", "Rb", "Sr", "Y", "Zr", "Nb", "Mo", 
    "Tc", "Ru", "Rh", "Pd", "Ag", "Cd", "In", "Sn", "Sb", "Te", "I", "Xe","Cs", "Ba", "La", "Ce", 
    "Pr", "Nd", "Pm", "Sm", "Eu", "Gd", "Tb","Dy", "Ho", "Er", "Tm", "Yb", "Lu", "Hf", "Ta", 
    "W", "Re", "Os","Ir", "Pt", "Au", "Hg", "Tl", "Pb", "Bi", "Po", "At", "Rn", "Fr", "Ra","Ac", 
    "Th", "Pa", "U", "Np", "Pu", "Am", "Cm", "Bk", "Cf", "Es", "Fm", "Md", "No", "Lr", "Rf", 
    "Db", "Sg", "Bh", "Hs", "Mt", "Ds",  "Rg", "Ec", "Ed", "Ee", "Ef", "Eg", "Eh", "Ei" };
  
  int chemical_symbol::z_from_symbol(const string & symbol_)
  {
    bool is_symbol_found = false; 
                                            
    int i_z = 0;
    
    while(!is_symbol_found && i_z < NB_CHEMICAL_SYMBOLS) 
      {
        if (symbol_ == chemical_symbol::table[i_z]) 
          {
            is_symbol_found = true;
          }
        i_z++;
      }
      
    if(! is_symbol_found)
      {
        ostringstream message;
        message << "materials::chemical_symbol::z_from_symbol: '" 
                << symbol_ << "' not found !" << endl;
        throw logic_error (message.str ());
      }       
    else return(i_z-1);
  }
  
  const string & chemical_symbol::symbol_from_z (int z_)
  {
    if( z_ >= 1  &&  z_ < NB_CHEMICAL_SYMBOLS)
      {
        return chemical_symbol::table[z_];
      }
    else
      {
        ostringstream message;
        message << endl<< "materials::chemical_symbol::symbol_from_z:  Z value : '" << z_<<"' not tabulated !"<<endl;
        throw logic_error (message.str ());
      }
  };
 
} // end of namespace materials

// end of chemical_symbol.cc
