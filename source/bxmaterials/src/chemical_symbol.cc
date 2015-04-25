/// chemical_symbol.cc

// Ourselves:
#include <materials/chemical_symbol.h>

// Standard library:
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <algorithm>

// Third party:
// - Boost:
#include <boost/scoped_ptr.hpp>
// - Bayeux/datatools:
#include <datatools/exception.h>

namespace materials {

  // static
  const int chemical_symbol::Z_UNDEFINED;

  // static
  const std::vector<std::string> & chemical_symbol::table_of_symbols()
  {
    static boost::scoped_ptr<std::vector<std::string> > _table_of_symbols;
    if (!_table_of_symbols) {
      _table_of_symbols.reset(new std::vector<std::string>);
      const size_t nb_ch_sym = 119;
      _table_of_symbols->reserve(nb_ch_sym);
      const std::string table[nb_ch_sym] = {
        "n",  "H", "He", "Li", "Be",  "B",  "C",  "N",  "O",  "F", "Ne",
        "Na", "Mg", "Al", "Si",  "P",  "S", "Cl", "Ar",  "K", "Ca", "Sc", "Ti",  "V", "Cr", "Mn", "Fe",
        "Co", "Ni", "Cu", "Zn", "Ga", "Ge", "As", "Se", "Br", "Kr", "Rb", "Sr",  "Y", "Zr", "Nb", "Mo",
        "Tc", "Ru", "Rh", "Pd", "Ag", "Cd", "In", "Sn", "Sb", "Te",  "I", "Xe", "Cs", "Ba", "La", "Ce",
        "Pr", "Nd", "Pm", "Sm", "Eu", "Gd", "Tb", "Dy", "Ho", "Er", "Tm", "Yb", "Lu", "Hf", "Ta",
         "W", "Re", "Os", "Ir", "Pt", "Au", "Hg", "Tl", "Pb", "Bi", "Po", "At", "Rn", "Fr", "Ra", "Ac",
        "Th", "Pa",  "U", "Np", "Pu", "Am", "Cm", "Bk", "Cf", "Es", "Fm", "Md", "No", "Lr", "Rf",
        "Db", "Sg", "Bh", "Hs", "Mt", "Ds", "Rg", "Ec", "Ed", "Ee", "Ef", "Eg", "Eh", "Ei" };
      for (int i = 0; i < (int) nb_ch_sym; i++) {
        const std::string & symbol = table[i];
        _table_of_symbols->push_back(symbol);
      }
    }
    return *_table_of_symbols;
  }

  // static
  unsigned int chemical_symbol::nb_chemical_symbols()
  {
    return table_of_symbols().size();
  }

  // static
  int chemical_symbol::z_from_symbol(const std::string & symbol_)
  {
    bool is_symbol_found = false;
    int i_z = 0;
    while (i_z < (int) table_of_symbols().size()) {
      if (symbol_ == table_of_symbols().at(i_z)) {
        is_symbol_found = true;
        break;
      }
      i_z++;
    }
    DT_THROW_IF(! is_symbol_found, std::logic_error, "Symbol '" << symbol_ << "' is not tabulated !");
    return(i_z);
  }

  // static
  bool chemical_symbol::symbol_is_tabulated(const std::string & symbol_)
  {
    std::vector<std::string>::const_iterator found = std::find(table_of_symbols().begin(),
                                                               table_of_symbols().end(),
                                                               symbol_);
    return found != table_of_symbols().end();
  }

  // static
  bool chemical_symbol::z_is_tabulated(int z_)
  {
    int i_z = z_;
    return (i_z >= 0 && i_z < (int) table_of_symbols().size());
  }

  // static
  const std::string & chemical_symbol::symbol_from_z(int z_)
  {
    DT_THROW_IF(! z_is_tabulated(z_), std::logic_error, "Z value : '" << z_ << "' not tabulated !");
    return table_of_symbols().at(z_);
  };

} // end of namespace materials
