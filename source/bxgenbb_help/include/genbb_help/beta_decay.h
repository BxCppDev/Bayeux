/// \file genbb_help/beta_decay.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2014-11-06
 * Last modified: 2014-11-06
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
 *   Beta decay driver.
 *
 * History:
 *
 */

#ifndef GENBB_HELP_BETA_DECAY_H
#define GENBB_HELP_BETA_DECAY_H 1

// Standard library:
#include <string>
#include <fstream>
#include <vector>

// Third party:
// - Boost:
#include <boost/scoped_ptr.hpp>
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/i_tree_dump.h>
// - Bayeux/mygsl:
#include <mygsl/rng.h>
// - Bayeux/genbb_help:
#include <genbb_help/primary_event.h>
#include <genbb_help/primary_particle.h>
#include <genbb_help/base_decay_driver.h>
#include <genbb_help/genbb_macros.h>

namespace genbb {

  // Forward declaration:
  class fermi_function;

  /// \brief Description of a beta decay
  class beta_decay  : public base_decay_driver
  {

  public:

    /// \brief Type of the beta decay
    enum decay_type {
      BETA_DECAY_INVALID = 0, //!< Invalid beta decay type
      BETA_DECAY_MINUS   = 1, //!< Beta minus
      BETA_DECAY_PLUS    = 2  //!< Beta plus
    };

    /// Return a label associated to a decay type
    static std::string label_from_decay_type(decay_type);

    /// Return a coupling associated to a label
    static decay_type decay_type_from_label(const std::string &);

    /// \brief Type of the coupling
    enum coupling_type {
      COUPLING_INVALID      = 0, //!< Invalid coupling
      COUPLING_AXIAL_VECTOR = 1, //!< Axial-vector coupling (pure Gamow-Teller)
      COUPLING_TENSOR       = 2, //!< Tensor coupling
      COUPLING_SCALAR       = 3, //!< Scalar coupling
      COUPLING_VECTOR       = 4, //!< Vector coupling (pure Fermi)
      COUPLING_USER         = 5, //!< User defined coupling
      COUPLING_UNKNOWN      = 6  //!< Unknown coupling
    };

    /// \brief Daughter recoil ion's electron shakeoff mode
    enum electron_shakeoff_mode_type {
      ESO_NO_SHAKEOFF   = 0, //!< No electron shakeoff
      ESO_FIXED_CHARGE  = 1, //!< Force daughter recoil ion's charge state
      ESO_RANDOM_CHARGE = 2  //!< Randomize daughter recoil ion's charge state
    };

    /// Invalid electron shakeoff fixed charge
    static const int ESO_INVALID_FIXED_CHARGE = 0xFFFFFFFF;

    /// \brief Entry for a given daughter recoil ion's charge state
    struct electron_shakeoff_entry {
      int    charge;            //!< Daughter recoil ion's charge state
      double probability;       //!< Probability
      double cumul_probability; //!< Cumulative probability
    };

    typedef std::vector<electron_shakeoff_entry> electron_shakeoff_data_type;

    /// Return a label associated to a coupling
    static std::string label_from_coupling(coupling_type);

    /// Return a coupling associated to a label
    static coupling_type coupling_from_label(const std::string &);

    // Return the a value associated to a coupling
    static double a_value(coupling_type);

    /// Default constructor
    beta_decay();

    /// Destructor
    ~beta_decay() override;

    /// Set the type of the beta decay
    void set_type(decay_type);

    /// Return the type of the beta decay
    decay_type get_type() const;

    /// Check if the decay is a beta minus
    bool is_beta_minus() const;

    /// Check if the decay is a beta plus
    bool is_beta_plus() const;

    /// Check if Q beta is set
    bool has_q_beta() const;

    /// Set the Q beta of the decay
    void set_q_beta(double);

    /// Return the Q beta of the decay
    double get_q_beta() const;

    /// Set the A of the decaying nucleus
    void set_A(int);

    /// Return the A of the decaying nucleus
    int get_A() const;

    /// Set the Z of the parent nucleus
    void set_Z_parent(int);

    /// Return the Z of the parent nucleus
    int get_Z_parent() const;

    /// Set the Z of the daughter nucleus
    void set_Z_daughter(int);

    /// Return the Z of the daughter nucleus
    int get_Z_daughter() const;

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

    /// Set the coupling of the beta decay
    void set_coupling(coupling_type coupling_);

    /// Return the coupling of the beta decay
    coupling_type get_coupling() const;

    /// Check if coupling is known
    bool coupling_is_known() const;

    /// Set the beta-neutrino angular correlation coefficient
    void set_a(double);

    /// Return the beta-neutrino angular correlation coefficient
    double get_a() const;

    /// Set the cut on beta kinetic energy
    void set_ke_cut(double);

    /// Return the cut on beta kinetic energy
    double get_ke_cut() const;

    /// Check electron shakeoff mode
    bool is_electron_shakeoff() const;

    /// Set electron shakeoff mode
    void set_electron_shakeoff(electron_shakeoff_mode_type eso_mode_);

    /// Check the fixed electron shakeoff charge of the daughter recoil ion
    bool has_electron_shakeoff_fixed_charge() const;

    /// Return the fixed electron shakeoff charge of the daughter recoil ion
    int get_electron_shakeoff_fixed_charge() const;

    /// Set fixed electron shakeoff charge of the daughter recoil ion
    void set_electron_shakeoff_fixed_charge(int charge_);

    /// Add an charge entry for the random electron shakeoff
    /// @arg charge_ the charge state (in unit of +e : ..., -2, -1 , 0, +1 , +2, ...)
    /// @arg probability_ the associated probability (not necessarily normalized)
    void add_electron_shakeoff_random(int charge_, double probability_);

    /// Return the random for random electron shakeoff
    const electron_shakeoff_data_type & get_electron_shakeoff_data_random() const;

    /// Set the name of the file
    void set_log_filename(const std::string &);

    /// Return the name of the log file
    const std::string & get_log_filename() const;

    /// Check the beta generation flag
    bool is_beta_generated() const;

    /// Set the beta generation flag
    void set_beta_generated(bool);

    /// Check the neutrino generation flag
    bool is_neutrino_generated() const;

    /// Set the neutrino generation flag
    void set_neutrino_generated(bool);

    /// Check the daughter nucleus/ion generation flag
    bool is_daughter_generated() const;

    /// Set the daughter nucleus/ion generation flag
    void set_daughter_generated(bool);

    /// Return the maximum value of the recoil ion momentum
    double get_pr_max() const;

    /// Return the maximum value of the probability density for Von Neumann method
    double get_probability_max() const;

    /// Compute the probability density function for a given electron kinetic energy and a given recoil ion momentum
    double pdf_ke_pr(double ke_ , double pr_) const;

    /// Compute the bounds for the electron kinetic energy
    static void compute_ke_limits(//double mass_parent_,
                                  double q_beta_,
                                  double pr_,
                                  double & ke_min_,
                                  double & ke_max_);

    /// Smart print
    void tree_dump(std::ostream & out_ = std::clog,
                           const std::string & title_  = "",
                           const std::string & indent_ = "",
                           bool inherit_ = false) const override;

    /// Display PDF
    void display_pdf(const std::string & title_ = "") const;

    /// Shoot the kinematics of the decay:
    /// @arg prng_ The pseudo-random number generator
    /// @arg ke_ the kinetic energy of the emitted beta particle
    /// @arg pr_ the momentum energy of the emitted recoil ion
    /// @arg cer_ the cosine of the angle between the beta particle and the recoil ion
    /// @arg cenu_ the cosine of the angle between the beta particle and the (anti-)neutrino
    int fire_event_kinematics(mygsl::rng & prng_,
                              double & ke_,
                              double & pr_,
                              double & pnu_,
                              double & cer_,
                              double & cenu_) const;

    /// Set the transition parameter from the start and stop levels
    void set_levels(const nuclear_level & level_start_,
                            const nuclear_level & level_stop_) override;

    /// Initialization
    void initialize(const datatools::properties & config_) override;

    /// Reset
    void reset() override;

    /// Shoot the kinematics of the decay and fill a primary event
    int fill(mygsl::rng & prng_, genbb::primary_event & event_) override;

  protected:

    /// Compute the maximum value of the recoil ion momentum
    void _compute_pr_max();

    /// Compute some internal data
    void _compute_limits();

    /// Compute some internal data
    void _init();

    /// Initialize electron shakeoff
    void _init_electron_shakeoff();

    /// Set default values (initialization)
    void _set_default();

  private:

    decay_type _type_;        //!< Type of the beta decay
    double _q_beta_;          //!< Effective Qbeta
    double _ke_cut_;          //!< Cut on beta low kinetic energy
    int _A_;                  //!< A of the decaying nucleus
    int _Z_parent_;           //!< Z of the parent decaying nucleus
    int _Z_daughter_;         //!< Z of the daughter nucleus
    double _mass_parent_;     //!< Isotopic mass of the parent decaying nucleus
    double _energy_parent_;   //!< Excitation energy of the parent decaying nucleus
    double _mass_daughter_;   //!< Isotopic mass of the daughter nucleus
    double _energy_daughter_; //!< Excitation energy of the daughter decaying nucleus
    coupling_type _coupling_; //!< Weak coupling
    double _a_;               //!< Beta-neutrino angular correlation coefficient
    electron_shakeoff_mode_type _electron_shakeoff_mode_; //!< Recoil nucleus' electron shakeoff mode
    int                         _electron_shakeoff_fixed_charge_; //!< Fixed charge of the daughter recoil ion
    electron_shakeoff_data_type _electron_shakeoff_data_random_; //!< Data for random electron shakeoff
    std::string _log_filename_; //!< Name of the file to save kinematics data
    boost::scoped_ptr<std::ofstream> _log_file_; //!< Smart pointer to a log file
    boost::scoped_ptr<fermi_function> _F_; //!< Smart pointer to the Fermi object-function
    bool _massive_neutrino_;   //!< Flag to use a massive neutrino
    double _neutrino_mass_;    //!< Specific nonzero neutrino mass
    bool _beta_generated_;     //!< Flag to generate the beta (minus/plus)
    bool _neutrino_generated_; //!< Flag to generate the (anti)neutrino
    bool _daughter_generated_; //!< Flag to generate the daughter nucleus

    // Working data:
    double _M_;               //!< Mass of the parent decaying nucleus
    double _m1_;              //!< Mass of the electron
    double _m2_;              //!< Mass of the neutrino
    double _m3_;              //!< Mass of the daughter nucleus
    double _pr_max_;          //!< Maximum value of the recoil ion momentum
    double _kr_max_;          //!< Maximum value of the recoil ion kinetic energy
    double _probability_max_; //!< Maximum value of the density function

    GENBB_BDD_REGISTRATION_INTERFACE(beta_decay)

  };

} // end of namespace genbb

/***************
 * OCD support *
 ***************/
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(genbb::beta_decay)

#endif // GENBB_HELP_BETA_DECAY_H

// Local Variables: --
// mode: c++ --
// End: --
