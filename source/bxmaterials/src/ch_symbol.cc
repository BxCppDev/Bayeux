// -*- mode: c++ ; -*- 
/* ch_symbol.cc
 */

#include <materials/ch_symbol.h>

#include <iostream>
#include <sstream>
#include <stdexcept>

namespace mat {

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
  
  int chemical_symbol::Z_from_symbol(const string & symbol_)
  {
    //clog << "DEVEL: Z_from_symbol: symbol='" << symbol_ << "'" << endl;
    bool is_symbol_found = false; 
					    
    int i_Z = 0;
    
    while(!is_symbol_found && i_Z < NB_CHEMICAL_SYMBOLS) 
      {
	//clog << "DEVEL: Z_from_symbol: i_Z=" << i_Z << "   TABLE='" << chemical_symbol::table[i_Z] << "'" << endl;
        if (symbol_ == chemical_symbol::table[i_Z]) 
	  {
	    is_symbol_found = true;
	    //clog << "DEVEL: Z_from_symbol: YES" << endl;	    
	  }
        i_Z++;
      }
    //clog << "DEVEL: Z_from_symbol: i_Z=" << i_Z << endl;
      
    if(! is_symbol_found)
      {
	ostringstream message;
	message << endl << "ch_symbol::Z_from_symbol(): '" 
		<< symbol_ << "' not found !" << endl;
	throw runtime_error (message.str ());
      }	      
    else return(i_Z-1);
  }
  
  const string & chemical_symbol::symbol_from_Z (int Z_)
  {
    if( Z_ >= 1  &&  Z_ < NB_CHEMICAL_SYMBOLS)
      {
        return chemical_symbol::table[Z_];
      }
    else
      {
	ostringstream message;
	message << endl<< "mat::ch_symbol::symbol_from_Z:  Z value : '" << Z_<<"' not tabulated !"<<endl;
	throw runtime_error (message.str ());
      }
  };
 
} // end of namespace mat

// end of ch_symbol.cc
