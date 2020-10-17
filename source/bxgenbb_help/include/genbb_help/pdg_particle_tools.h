/// \file genbb_help/pdg_particle_tools.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2015-08-28
 * Last modified: 2015-08-30
 *
 * License:
 * Copyright 2015 F. Mauger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * Description:
 *
 *   PDF particle code
 *
 * History:
 *
 */

#ifndef GENBB_HELP_PDG_PARTICLE_TOOLS_H
#define GENBB_HELP_PDG_PARTICLE_TOOLS_H

// Standard library:
#include <string>
#include <map>

// Third party:
// - Boost:
#include <boost/cstdint.hpp>
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/exception.h>
#include <datatools/i_tree_dump.h>

namespace genbb {

  namespace pdg {

    /// \brief The identification of a particle with its PDG code
    ///        This follows the scheme from http://pdg.lbl.gov/2014/reviews/rpp2014-rev-monte-carlo-numbering.pdf
    class particle : public datatools::i_tree_dumpable
    {
    public:

      /// \brief Supported PDG particle codes
      enum particle_code {
        INVALID_CODE           =      0, ///< Invalid PDG code

        // Leptons:
        ELECTRON               =     11,
        POSITRON               =    -11,
        NEUTRINO_ELECTRON      =     12,
        ANTI_NEUTRINO_ELECTRON =    -12,
        MUON_MINUS             =     13,
        MUON_PLUS              =    -13,
        NEUTRINO_MUON          =     14,
        ANTI_NEUTRINO_MUON     =    -14,
        TAU_MINUS              =     15,
        TAU_PLUS               =    -15,
        NEUTRINO_TAU           =     16,
        ANTI_NEUTRINO_TAU      =    -16,

        // Bosons:
        GAMMA                  =     22,

        // Monte-Carlo
        MC_GEANTINO            =     81,
        MC_OPTICAL_PHOTON      =     82,
        MC_CERENKOV            =     82,
        MC_SCINTILLATION       =     82,
        MC_UNSPECIFIED         =    100,

        // Mesons:
        PION_ZERO              =    111,
        PION_PLUS              =    211,
        PION_MINUS             =   -211,
        KAON_ZERO_LONG         =    130,
        KAON_ZERO_SHORT        =    310,
        KAON_ZERO              =    311,
        ANTI_KAON_ZERO         =   -311,
        KAON_PLUS              =    321,
        KAON_MINUS             =   -321,
        ETA                    =    221,

        // Barions:
        PROTON                 =   2212,
        ANTI_PROTON            =  -2212,
        NEUTRON                =   2112,
        ANTI_NEUTRON           =  -2112,
        LAMBDA                 =   3122,
        ANTI_LAMBDA            =  -3122,
        SIGMA_PLUS             =   3222,
        ANTI_SIGMA_PLUS        =  -3222,
        SIGMA_ZERO             =   3212,
        ANTI_SIGMA_ZERO        =  -3212,
        SIGMA_MINUS            =   3112,
        ANTI_SIGMA_MINUS       =  -3112,
        XI_ZERO                =   3322,
        ANTI_XI_ZERO           =  -3322,
        XI_MINUS               =   3312,
        ANTI_XI_MINUS          =  -3312,
        OMEGA_MINUS            =   3334,
        ANTI_OMEGA_MINUS       =  -3334,

        // Ions:
        DEUTERON               =  1000010020,
        ANTI_DEUTERON          = -1000010020,
        TRITON                 =  1000010030,
        ANTI_TRITON            = -1000010030,
        HE3                    =  1000020030,
        ANTI_HE3               = -1000020030,
        ALPHA                  =  1000020040,
        ANTI_ALPHA             = -1000020040

      };

      /// Default constructor
      particle();

      /// Constructor
      particle(int32_t pdg_code_,
               const std::string & name_,
               const std::string & latex_repr_);

      /// Constructor
      particle(int32_t pdg_code_,
               const std::string & name_,
               const std::string & alt_name_,
               const std::string & latex_repr_);

      /// Destructor
      ~particle() override;

      /// Check validity
      bool is_valid() const;

      /// Reset
      void reset();

      /// Return the PDG code
      int32_t get_code() const;

      /// Set the PDG code
      particle & set_code(int32_t);

      /// Return the name
      const std::string & get_name() const;

      /// Set the name
      particle & set_name(const std::string &);

      /// Check if an alternative name is defined
      bool has_alt_name() const;

      /// Return the alternative name
      const std::string & get_alt_name() const;

      /// Set the alternative name
      particle & set_alt_name(const std::string &);

      /// Return the LaTeX representation
      const std::string & get_latex_repr() const;

      /// Set the LaTeX representation
      particle & set_latex_repr(const std::string &);

      /// Return a const reference to the embedded auxiliary properties
      const datatools::properties & get_auxiliaries() const;

      /// Return a mutable reference to the embedded auxiliary properties
      datatools::properties & grab_auxiliaries();

      /// Smart print
      void tree_dump(std::ostream & out_ = std::clog,
                             const std::string & title_ = "",
                             const std::string & indent_ = "",
                             bool inherit_ = false) const override;

      /// Build the nuclear code from (Z, A and isomer level).
      /// This is not for single hadrons like p, n,...
      /// I is the isomer level with the following possible values:
      ///  * I = 0 -> ground state (g.s.)
      ///  * I = 1 -> m
      ///  * I = 2 -> n
      ///  * I = 3 -> p
      ///  * I = 4 -> q
     static int32_t build_nuclear_code(int z_, int a_, int i_ = 0);

    private:

      int32_t     _code_;       ///< PDG code
      std::string _name_;       ///< Name
      std::string _alt_name_;   ///< Alternative name
      std::string _latex_repr_; ///< LaTeX formatted representation
      datatools::properties _auxiliaries_; ///< Auxiliary properties

    };

    /// \brief A registry for particle entries
    class particle_registry : public datatools::i_tree_dumpable
    {

    public:

      /// Type of dictionary of particle records
      typedef std::map<int32_t, particle> particle_dict_type;

      /// Default constructor
      particle_registry(uint32_t flags_ = 0);

      /// Destructor
      ~particle_registry() override;

      /// Clear the registry
      void clear();

      /// Registration of a particle record
      particle & registration(const particle & part_);

      /// Unregistration of a particle record given its name
      void unregistration(const std::string & name_);

      /// Unregistration of a particle record given its PDG code
      void unregistration(int32_t pdg_code_);

      /// Check particle
      bool has_particle(int32_t pdg_code_) const;

      /// Return a registered unit
      const particle & get_particle(int32_t pdg_code_) const;

      /// Check particle given its name
      bool has_particle_by_name(const std::string & name_) const;

      /// Return a registered particle given its name
      const particle & get_particle_by_name(const std::string & name_) const;

      /// Smart print
      void tree_dump(std::ostream & out_ = std::clog,
                             const std::string & title_ = "",
                             const std::string & indent_ = "",
                             bool inherit_ = false) const override;

      /// Register standard particles
      void register_standard_particles();

      /// Access to the mutable system singleton registry
      static particle_registry & system_registry();

      /// Access to the non mutable system singleton registry
      static const particle_registry & const_system_registry();

    private:

      particle_dict_type _records_; ///< Dictionary of particle records

      /// Private access to the mutable system singleton registry
      static particle_registry & _access_system_registry_();

    };

  } // end of namespace pdg

} // end of namespace genbb

#endif // GENBB_HELP_PDG_PARTICLE_TOOLS_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
