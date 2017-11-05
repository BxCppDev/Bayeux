//! \file  mctools/mcnp/material/material.h
//! \brief Description of MNCP material
//
// Copyright (c) 2015 by François Mauger <mauger@lpccaen.in2p3.fr>
//
// This file is part of Bayeux.
//
// Bayeux is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Bayeux is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Bayeux. If not, see <http://www.gnu.org/licenses/>.

#ifndef MCTOOLS_MCNP_MATERIAL_MATERIAL_H
#define MCTOOLS_MCNP_MATERIAL_MATERIAL_H

// Standard library:
#include <string>
#include <iostream>
#include <vector>

// Third party:
// - Boost:
#include <boost/shared_ptr.hpp>
// - Bayeux:
#include <materials/manager.h>

namespace mctools {

  namespace mcnp {

    namespace material {

      //! \brief Nuclide entry
      class nuclide_entry
      {
      public:

        //! Invalid Z/A values
        static const int INVALID_ZA = -1;

        //! \brief Fraction mode
        enum fraction_mode_type {
          FRACTION_INVALID =  0, //!< Invalid fraction mode
          FRACTION_MASS    = -1, //!< Mass fraction
          FRACTION_NATOMS  = +1  //!< Number of atoms
        };

        //! Default constructor
        nuclide_entry();

        //! Constructor
        nuclide_entry(int z_, int a_, double fraction_);

        //! Reset
        void reset();

        //! Check if the entry is valid
        bool is_valid() const;

        //! Check if the element is natural
        bool is_natural() const;

        //! Set the Z
        void set_z(int z_);

        //! Get the Z
        int get_z() const;

        //! Set the A
        void set_a(int a_);

        //! Get the A
        int get_a() const;

        //! Check if fraction is given by mass
        bool is_mass_fraction() const;

        //! Check if fraction is given by number of atoms
        bool is_number_of_atoms_fraction() const;

        //! Return the fraction mode
        fraction_mode_type get_fraction_mode() const;

        //! Set the fraction
        void set_fraction(double fraction_, fraction_mode_type fraction_mode_);

        //! Return the fraction
        double get_fraction() const;

        //! Print MCNP card
        void print_card(std::ostream & out_, uint32_t flags_ = 0) const;

      private:

        int _z_; //!< Nuclide Z
        int _a_; //!< Nuclide A
        fraction_mode_type _fraction_mode_; //!< Fraction mode
        double _fraction_; //!< Fraction

      };

      class material;

      //! Shared pointer on a material
      typedef boost::shared_ptr<material> material_ptr_type;

      //! \brief Material support
      class material
      {
      public:

        //! Invalid material identifier
        static const int INVALID_ID =     -1;
        static const int MIN_ID     =      1;
        static const int MAX_ID     =  99999;

        //! Default constructor
        material();

        //! Destructor
        ~material();

        //! Reset
        void reset();

        //! Set the name
        void set_name(const std::string &);

        //! Return the name
        const std::string & get_name() const;

        //! Set the identifier
        void set_id(int);

        //! Return the identifier
        int get_id() const;

        //! Add nuclide
        void add_nuclide(int z_,
                         int a_,
                         double fraction_,
                         nuclide_entry::fraction_mode_type fraction_mode,
                         bool lock_ = false);

        //! Make natural_element
        void make_natural_element(int z_);

        //! Check if material is valid
        bool is_valid() const;

        //! Print MCNP card
        void print_card(std::ostream & out_, uint32_t flags_ = 0) const;

        //! Check lock
        bool is_locked() const;

        //! Lock
        void lock();

        //! Unlock
        void unlock();

      private:

        bool                       _locked_;      //!< Locked material
        std::string                _name_;        //!< Material name
        int                        _id_;          //!< Number of the material
        std::vector<nuclide_entry> _composition_; //!< Composition in term of nuclides
        // double                     _rho_;         //!< Density

      };

    } // namespace material

  } // namespace mcnp

} // namespace mctools

#endif // MCTOOLS_MCNP_MATERIAL_MATERIAL_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
