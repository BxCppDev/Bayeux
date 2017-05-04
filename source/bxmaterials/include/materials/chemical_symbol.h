/// \file materials/chemical_symbol.h
/* Author(s):  Benoit Guillon <guillon@lpccaen.in2p3.fr>
 *             François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2008-02-25
 * Last modified: 2014-06-11
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
 *  Authors:
 *   - Benoit Guillon <guillon@lpccaen.in2p3.fr>
 *   - François Mauger <mauger@lpccaen.in2p3.fr>
 *
 *  Mainteners:
 *   - François Mauger <mauger@lpccaen.in2p3.fr>
 *
 */

#ifndef MATERIALS_CHEMICAL_SYMBOL_H
#define MATERIALS_CHEMICAL_SYMBOL_H 1

// Standard library:
#include <string>
#include <vector>

/// \brief Top-level namespace of the Bayeux/materials module library
namespace materials {

  /// Utilities for chemical symbols
  class chemical_symbol
  {
  public:

    /// Invalid Z value
    static const int Z_UNDEFINED = -1;

    /// Return the vector of tabulated chemical symbols
    static const std::vector<std::string> & table_of_symbols();

    /// Return the number of tabulated chemical symbols
    static unsigned int nb_chemical_symbols();

    /// Check is a chemical symbol is tabulated
    static bool symbol_is_tabulated(const std::string & symbol_);

    /// Check is a Z value is tabulated
    static bool z_is_tabulated(int z_);

    /// Return the Z value from a chemical symbol
    static int z_from_symbol(const std::string & symbol_);

    /// Return the chemical symbol associated to a Z value
    static const std::string & symbol_from_z(int z_);

  };

} // end of namespace materials

#endif // MATERIALS_CHEMICAL_SYMBOL_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
