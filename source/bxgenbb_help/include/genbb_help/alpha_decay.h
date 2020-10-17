/// \file genbb_help/alpha_decay.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2014-11-08
 * Last modified: 2014-11-10
 *
 * License:
 * Copyright 2014 F. Mauger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * Description:
 *
 *   Alpha decay driver.
 *
 * History:
 *
 */

#ifndef GENBB_HELP_ALPHA_DECAY_H
#define GENBB_HELP_ALPHA_DECAY_H 1

// Standard library:
#include <string>
#include <fstream>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/i_tree_dump.h>
// - Bayeux/mygsl:
#include <mygsl/rng.h>

// This project:
#include <genbb_help/base_decay_driver.h>
#include <genbb_help/genbb_macros.h>
#include <genbb_help/two_body_decay.h>

namespace genbb {

  // Forward declaration:
  class nuclear_level;

  /// \brief Description of an alpha decay
  class alpha_decay  : public base_decay_driver
  {

  public:

    /// Default constructor
    alpha_decay();

    /// Destructor
    ~alpha_decay() override;

    /// Check if Q alpha is set
    bool has_q_alpha() const;

    /// Set the Q alpha of the decay
    void set_q_alpha(double);

    /// Return the Q alpha of the decay
    double get_q_alpha() const;

    /// Check if A is set
    bool has_A() const;

    /// Set the A of the parent nucleus
    void set_A(int);

    /// Return the A of the parent nucleus
    int get_A() const;

    /// Check if Z parent is set
    bool has_Z() const;

    /// Set the Z of the parent nucleus
    void set_Z(int);

    /// Return the Z of the parent nucleus
    int get_Z() const;

    /// Return the Z of the parent nucleus
    int get_Z_parent() const;

    /// Return the Z of the daughter nucleus
    int get_Z_daughter() const;

    /// Return the A of the parent nucleus
    int get_A_parent() const;

    /// Return the A of the daughter nucleus
    int get_A_daughter() const;

    /// Set the isotopic mass of the parent nucleus (not the atomic mass)
    void set_mass_parent(double);

    /// Return the isotopic mass of the parent nucleus (not the atomic mass)
    double get_mass_parent() const;

    /// Set the isotopic mass of the daughter nucleus (not the atomic mass)
    void set_mass_daughter(double);

    /// Return the isotopic mass of the daughter nucleus (not the atomic mass)
    double get_mass_daughter() const;

    /// Set the excitation energy of the parent nucleus
    void set_energy_parent(double);

    /// Return the excitation energy of the parent nucleus
    double get_energy_parent() const;

    /// Set the excitation energy of the daughter nucleus
    void set_energy_daughter(double);

    /// Return the isotopic mass of the daughter nucleus
    double get_energy_daughter() const;

    /// Check the alpha generation flag
    bool is_alpha_generated() const;

    /// Set the alpha generation flag
    void set_alpha_generated(bool);

    /// Check the daughter nucleus generation flag
    bool is_daughter_generated() const;

    /// Set the daughter nucleus generation flag
    void set_daughter_generated(bool);

    /// Set the transition parameter from the start and stop levels
    void set_levels(const nuclear_level & lstart_, const nuclear_level & lstop_) override;

    /// Smart print
    void tree_dump(std::ostream & out_ = std::clog,
                           const std::string & title_  = "",
                           const std::string & indent_ = "",
                           bool inherit_ = false) const override;

    /// Shoot the kinematics of the decay:
    /// @arg prng_ The pseudo-random number generator
    /// @arg e_ the kinetic energy of the emitted gamma
    /// @arg theta_ the theta angle of the emitted gamma
    /// @arg phi_ the phi the angle of the emitted gamma
    int fire_event_kinematics(mygsl::rng & prng_,
                              double & ka_,
                              double & thetaa_,
                              double & phia_,
                              double & kr_,
                              double & thetar,
                              double & phir_) const;

    /// Initialization
    void initialize(const datatools::properties & config_) override;

    /// Reset
    void reset() override;

    /// Shoot the kinematics of the decay and fill a primary event
    int fill(mygsl::rng & prng_, genbb::primary_event & event_) override;

  protected:

    /// Set default values (initialization)
    void _set_default();

    /// Compute some internal data
    void _init();

  private:

    double _q_alpha_;         //!< Effective Q alpha
    int    _A_;               //!< A of the parent nucleus
    int    _Z_;               //!< Z of the parent nucleus
    double _mass_parent_;     //!< Isotopic mass of the parent decaying nucleus
    double _energy_parent_;   //!< Excitation energy of the parent decaying nucleus
    double _mass_daughter_;   //!< Isotopic mass of the daughter nucleus
    double _energy_daughter_; //!< Excitation energy of the daughter decaying nucleus
    bool   _alpha_generated_; //!< Flag to generate the alpha
    bool   _daughter_generated_; //!< Flag to generate the daughter nucleus

    // Working data:
    double _M_;               //!< Mass of the parent decaying nucleus
    double _m1_;              //!< Mass of the alpha
    double _m2_;              //!< Mass of the daughter nucleus
    two_body_decay _tbd_;     //!< Two-body decay kinematics

    GENBB_BDD_REGISTRATION_INTERFACE(alpha_decay)

  };

} // end of namespace genbb

/***************
 * OCD support *
 ***************/
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(genbb::alpha_decay)

#endif // GENBB_HELP_ALPHA_DECAY_H

// Local Variables: --
// mode: c++ --
// End: --
