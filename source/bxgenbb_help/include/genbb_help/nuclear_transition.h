/// \file genbb_help/nuclear_transition.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2014-07-08
 * Last modified: 2014-07-08
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
 *   Utilities related to gamma transitions.
 *
 * History:
 *
 */

#ifndef GENBB_HELP_NUCLEAR_TRANSITION_H
#define GENBB_HELP_NUCLEAR_TRANSITION_H 1

// Standard library:
#include <string>
#include <fstream>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/i_tree_dump.h>
#include <datatools/bit_mask.h>
// - Bayeux/mygsl:
#include <mygsl/rng.h>

// This project:
#include <genbb_help/base_decay_driver.h>
#include <genbb_help/genbb_macros.h>

namespace genbb {

  // Forward declaration:
  class nuclear_level;

  /// \brief Description of a deexcitation nuclear transition between two levels of the same nuclide (gamma + IC)
  class nuclear_transition  : public base_decay_driver
  {

  public:

    /// \brief Approximation mode
    enum approx_flag_type {
      APPROX_NULL         = 0,                          //!< Null flag
      APPROX_GAMMA_ONLY   = datatools::bit_mask::bit00, //!< Approximation mode with only gamma emission, neglecting  conversion from K/L/M shells and pair conversion
      APPROX_GKLMP_SIMPLE = datatools::bit_mask::bit01, //!< Approximation mode with concurrent gamma emission, internal conversion from K/L/M shells and pair conversion
      APPROX_DEFAULT      = APPROX_GKLMP_SIMPLE         //!< Default approximation mode
    };

    /// \brief Deexcitation mode
    enum mode_type {
      MODE_INVALID = -1, //!< Invalid mode
      MODE_GAMMA = 0, //!< Gamma emission
      MODE_IC_K  = 1, //!< Internal conversion on K-shell
      MODE_IC_L  = 2, //!< Internal conversion on L-shell
      MODE_IC_M  = 3, //!< Internal conversion on M-shell
      MODE_PC    = 4  //!< Pair conversion
    };

    /// Constructor
    nuclear_transition();

    /// Destructor
    ~nuclear_transition() override;

    /// Check if A is set
    bool has_A() const;

    /// Set the A of the excited nucleus
    void set_A(int);

    /// Return the A of the excited nucleus
    int get_A() const;

    /// Check if Z is set
    bool has_Z() const;

    /// Set the Z of the excited nucleus
    void set_Z(int);

    /// Return the Z of the excited nucleus
    int get_Z() const;

    /// Check if energy is set
    bool has_energy() const;

    /// Set the energy
    void set_energy(double);

    /// Return the energy
    double get_energy() const;

    /// Set the binding energy of electron on K-shell
    void set_ic_k_shell_binding_energy(double b_);

    /// Set the binding energy of electron on L-shell
    void set_ic_l_shell_binding_energy(double b_);

    /// Set the binding energy of electron on M-shell
    void set_ic_m_shell_binding_energy(double b_);

    /// Return the binding energy of electron on K-shell
    double get_ic_k_shell_binding_energy() const;

    /// Return the binding energy of electron on L-shell
    double get_ic_l_shell_binding_energy() const;

    /// Return the binding energy of electron on M-shell
    double get_ic_m_shell_binding_energy() const;

    /// Set the IC coefficient of electron on K-shell
    void set_ic_k_shell_coefficient(double b_);

    /// Set the IC coefficient of electron on L-shell
    void set_ic_l_shell_coefficient(double b_);

    /// Set the IC coefficient of electron on M-shell
    void set_ic_m_shell_coefficient(double b_);

    /// Return the IC coefficient of electron on K-shell
    double get_ic_k_shell_coefficient() const;

    /// Return the IC coefficient of electron on L-shell
    double get_ic_l_shell_coefficient() const;

    /// Return the IC coefficient of electron on M-shell
    double get_ic_m_shell_coefficient() const;

    /// Set the pair conversion coefficient
    void set_pc_coefficient(double c_);

    /// Return the pair conversion coefficient
    double get_pc_coefficient() const;

    /// Check the gamma generation flag
    bool is_gamma_generated() const;

    /// Set the gamma generation flag
    void set_gamma_generated(bool);

    /// Check the conversion electron generation flag
    bool is_conversion_electron_generated() const;

    /// Set the conversion electron generation flag
    void set_conversion_electron_generated(bool);

    /// Check the conversion pair generation flag
    bool is_conversion_pair_generated() const;

    /// Set the conversion pair generation flag
    void set_conversion_pair_generated(bool);

    /// Set the transition parameter from the start and stop levels
    void set_levels(const nuclear_level & lstart_, const nuclear_level & lstop_) override;

    /// Smart print
    void tree_dump(std::ostream & out_ = std::clog,
                           const std::string & title_  = "",
                           const std::string & indent_ = "",
                           bool inherit_ = false) const override;

    /// Shoot the kinematics of the gamma emission, internal conversion + X-rays or pair conversion:
    /// @arg prng_ The pseudo-random number generator
    /// @arg mode_ the transition mode (gamma, IC (K/L/M) or pair conversion))
    /// @arg eg_ the kinetic energy of the emitted gamma or X (IC or gamma)
    /// @arg thetag_ the theta angle of the emitted gamma or X (IC or gamma)
    /// @arg phig_ the phi the angle of the emitted gamma or X (IC or gamma)
    /// @arg ke1_ the kinetic energy of the emitted electron (IC or pair)
    /// @arg theta1_ the theta angle of the emitted electron (IC or pair)
    /// @arg phi1_ the phi the angle of the emitted electron (IC or pair)
    /// @arg ke2_ the kinetic energy of the emitted positron (pair)
    /// @arg theta2_ the theta angle of the emitted positron (pair)
    /// @arg phi2_ the phi the angle of the emitted positron (pair)
    int fire_event_kinematics(mygsl::rng & prng_,
                              mode_type & mode_,
                              double & eg_,
                              double & thetag_,
                              double & phig_,
                              double & e1_,
                              double & theta1_,
                              double & phi1_,
                              double & e2_,
                              double & theta2_,
                              double & phi2_) const;

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

    int    _A_;           //!< A of the deexciting level
    int    _Z_;           //!< Z of the deexciting level
    double _energy_;      //!< Energy of the transition
    unsigned int _approx_flags_; //!< Flag for various approximations
    double _ic_k_shell_binding_energy_; //!< Binding energy of electron on K-shell
    double _ic_l_shell_binding_energy_; //!< Binding energy of electron on L-shell
    double _ic_m_shell_binding_energy_; //!< Binding energy of electron on M-shell
    double _ic_k_shell_coefficient_; //!< Internal electron conversion coefficient from K-shell
    double _ic_l_shell_coefficient_; //!< Internal electron conversion coefficient from L-shell
    double _ic_m_shell_coefficient_; //!< Internal electron conversion coefficient from M-shell
    double _pc_coefficient_; //!< Pair conversion coefficient

    bool   _gamma_generated_; //!< Flag to generate the gamma
    bool   _conversion_electron_generated_; //!< Flag to generate the conversion electrons
    bool   _conversion_pair_generated_; //!< Flag to generate the conversion pairs

    // Working data:
    double _cumul_g_;   //!< Cumulative probability of gamma emission
    double _cumul_pK_;  //!< Cumulative probability of internal conversion from K-shell
    double _cumul_pL_;  //!< Cumulative probability of internal conversion from L-shell
    double _cumul_pM_;  //!< Cumulative probability of internal conversion from M-shell
    double _cumul_pp_;  //!< Cumulative probability of pair conversion


    GENBB_BDD_REGISTRATION_INTERFACE(nuclear_transition)

  };

} // end of namespace genbb

/***************
 * OCD support *
 ***************/
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(genbb::nuclear_transition)

#endif // GENBB_HELP_NUCLEAR_TRANSITION_H

// Local Variables: --
// mode: c++ --
// End: --
