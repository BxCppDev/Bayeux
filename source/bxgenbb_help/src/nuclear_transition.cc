// nuclear_transition.cc
/*
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
 */

// Ourselves:
#include <genbb_help/nuclear_transition.h>

// Standard library:
#include <limits>
#include <cmath>

// Third party:
// - CLHEP:
#include<CLHEP/Units/PhysicalConstants.h>
#include<CLHEP/Units/SystemOfUnits.h>
// - GSL:
#include<gsl/gsl_math.h>
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/utils.h>
#include <datatools/exception.h>
// - Bayeux/mygsl:
#include <mygsl/rng.h>
// - Bayeux/geomtools:
#include <geomtools/utils.h>

// This project:
#include <genbb_help/nuclear_level.h>
#include <genbb_help/primary_particle.h>
#include <genbb_help/primary_event.h>
#include <genbb_help/kinematics.h>

namespace genbb {

  GENBB_BDD_REGISTRATION_IMPLEMENT(nuclear_transition, "genbb::nuclear_transition");

  void nuclear_transition::_set_default()
  {
    this->base_decay_driver::_set_default();
    _A_ = 0;
    _Z_ = 0;
    datatools::invalidate(_energy_);
    _approx_flags_ = 0;
    datatools::invalidate(_ic_k_shell_binding_energy_);
    datatools::invalidate(_ic_l_shell_binding_energy_);
    datatools::invalidate(_ic_m_shell_binding_energy_);
    datatools::invalidate(_ic_k_shell_coefficient_);
    datatools::invalidate(_ic_l_shell_coefficient_);
    datatools::invalidate(_ic_m_shell_coefficient_);
    datatools::invalidate(_pc_coefficient_);
    datatools::invalidate(_cumul_g_);
    datatools::invalidate(_cumul_pK_);
    datatools::invalidate(_cumul_pL_);
    datatools::invalidate(_cumul_pM_);
    datatools::invalidate(_cumul_pp_);
    _gamma_generated_ = true;
    _conversion_electron_generated_ = true;
    _conversion_pair_generated_ = true;
    return;
  }

  void nuclear_transition::set_ic_k_shell_binding_energy(double b_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error,
                "Nuclear transition is already initialized!");
    DT_THROW_IF(b_ < 0.0, std::logic_error,
                "Invalid K-shell binding energy!");
    _ic_k_shell_binding_energy_ = b_;
    return;
  }

  void nuclear_transition::set_ic_l_shell_binding_energy(double b_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error,
                "Nuclear transition is already initialized!");
    DT_THROW_IF(b_ < 0.0, std::logic_error,
                "Invalid L-shell binding energy!");
    _ic_l_shell_binding_energy_ = b_;
    return;
  }

  void nuclear_transition::set_ic_m_shell_binding_energy(double b_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error,
                "Nuclear transition is already initialized!");
    DT_THROW_IF(b_ < 0.0, std::logic_error,
                "Invalid M-shell binding energy!");
    _ic_m_shell_binding_energy_ = b_;
    return;
  }

  double nuclear_transition::get_ic_k_shell_binding_energy() const
  {
    return _ic_k_shell_binding_energy_;
  }

  double nuclear_transition::get_ic_l_shell_binding_energy() const
  {
    return _ic_l_shell_binding_energy_;
  }

  double nuclear_transition::get_ic_m_shell_binding_energy() const
  {
    return _ic_m_shell_binding_energy_;
  }

  void nuclear_transition::set_ic_k_shell_coefficient(double c_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error,
                "Nuclear transition is already initialized!");
    DT_THROW_IF(c_ < 0.0, std::logic_error,
                "Invalid IC K-shell coefficient!");
    _ic_k_shell_coefficient_ = c_;
    return;
  }

  void nuclear_transition::set_ic_l_shell_coefficient(double c_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error,
                "Nuclear transition is already initialized!");
    DT_THROW_IF(c_ < 0.0, std::logic_error,
                "Invalid IC L-shell coefficient!");
    _ic_l_shell_coefficient_ = c_;
    return;
  }

  void nuclear_transition::set_ic_m_shell_coefficient(double c_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error,
                "Nuclear transition is already initialized!");
    DT_THROW_IF(c_ < 0.0, std::logic_error,
                "Invalid IC M-shell coefficient!");
    _ic_m_shell_coefficient_ = c_;
    return;
  }

  double nuclear_transition::get_ic_k_shell_coefficient() const
  {
    return _ic_k_shell_coefficient_;
  }

  double nuclear_transition::get_ic_l_shell_coefficient() const
  {
    return _ic_l_shell_coefficient_;
  }

  double nuclear_transition::get_ic_m_shell_coefficient() const
  {
    return _ic_m_shell_coefficient_;
  }

  void nuclear_transition::set_pc_coefficient(double c_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error,
                "Nuclear transition is already initialized!");
    DT_THROW_IF(c_ < 0.0, std::logic_error,
                "Invalid pair conversion coefficient!");
    _pc_coefficient_ = c_;
    return;
  }

  double nuclear_transition::get_pc_coefficient() const
  {
    return _pc_coefficient_;
  }

  bool nuclear_transition::is_gamma_generated() const
  {
    return _gamma_generated_;
  }

  void nuclear_transition::set_gamma_generated(bool g_)
  {
    _gamma_generated_ = g_;
    return;
  }

  bool nuclear_transition::is_conversion_electron_generated() const
  {
    return _conversion_electron_generated_;
  }

  void nuclear_transition::set_conversion_electron_generated(bool g_)
  {
    _conversion_electron_generated_ = g_;
    return;
  }

  bool nuclear_transition::is_conversion_pair_generated() const
  {
    return _conversion_pair_generated_;
  }

  void nuclear_transition::set_conversion_pair_generated(bool g_)
  {
    _conversion_pair_generated_ = g_;
    return;
  }

  nuclear_transition::nuclear_transition() : base_decay_driver()
  {
    _set_default();
    return;
  }

  nuclear_transition::~nuclear_transition()
  {
    if (is_initialized()) {
      reset();
    }
    return;
  }

  void nuclear_transition::initialize(const datatools::properties & config_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error,
                "Nuclear transition is already initialized!");

    this->base_decay_driver::_initialize(config_);

    double default_energy_unit = CLHEP::keV;

    if (! has_A()) {
      if (config_.has_key("A")) {
        int a = config_.fetch_integer("A");
        set_A(a);
      }
    }

    if (! has_Z()) {
      if (config_.has_key("Z")) {
        int z = config_.fetch_integer("Z");
        set_Z(z);
      }
    }

    if (! has_energy()) {
      DT_THROW_IF(!config_.has_key("energy"), std::logic_error, "Missing 'energy' key!");
      double energy = config_.fetch_real("energy");
      DT_THROW_IF(energy <= 0.0, std::logic_error, "Invalid gamma transition 'energy'!");
      if (! config_.has_explicit_unit("energy")) {
        energy *= default_energy_unit;
      }
      set_energy(energy);
    }

    if (_approx_flags_ == APPROX_NULL) {

      if (config_.has_flag("approximation.gamma_only")) {
        _approx_flags_ |= APPROX_GAMMA_ONLY;
      }

      if (config_.has_flag("approximation.gklmp_simple")) {
        _approx_flags_ |= APPROX_GKLMP_SIMPLE;
      }

    }

    if (_approx_flags_ == APPROX_NULL) {
      _approx_flags_ = APPROX_DEFAULT;
    }

    // Internal conversion:
    if (! datatools::is_valid(_ic_k_shell_coefficient_)) {
      // IC from K-shell:
      if (config_.has_key("ic.k_shell.coefficient")) {
        double ickc = config_.fetch_real("ic.k_shell.coefficient");
        set_ic_k_shell_coefficient(ickc);
      }
    }
    if (datatools::is_valid(_ic_k_shell_coefficient_)) {
      if (! datatools::is_valid(_ic_k_shell_binding_energy_)) {
        if (config_.has_key("ic.k_shell.binding_energy")) {
          double ickbe = config_.fetch_real("ic.k_shell.binding_energy");
          if (! config_.has_explicit_unit("ic.k_shell.binding_energy")) {
            ickbe *= default_energy_unit;
          }
          set_ic_k_shell_binding_energy(ickbe);
        }
      }
    } else {
      set_ic_k_shell_binding_energy(0.0);
    }

    if (! datatools::is_valid(_ic_l_shell_coefficient_)) {
      // IC from L-shell:
      if (config_.has_key("ic.l_shell.coefficient")) {
        double iclc = config_.fetch_real("ic.l_shell.coefficient");
        set_ic_l_shell_coefficient(iclc);
      }
    }
    if (datatools::is_valid(_ic_l_shell_coefficient_)) {
      if (! datatools::is_valid(_ic_l_shell_binding_energy_)) {
        if (config_.has_key("ic.l_shell.binding_energy")) {
          double iclbe = config_.fetch_real("ic.l_shell.binding_energy");
          if (! config_.has_explicit_unit("ic.l_shell.binding_energy")) {
            iclbe *= default_energy_unit;
          }
          set_ic_l_shell_binding_energy(iclbe);
        }
      }
    } else {
      set_ic_l_shell_binding_energy(0.0);
    }

    if (! datatools::is_valid(_ic_m_shell_coefficient_)) {
      // IC from M-shell:
      if (config_.has_key("ic.m_shell.coefficient")) {
        double icmc = config_.fetch_real("ic.m_shell.coefficient");
        set_ic_m_shell_coefficient(icmc);
      }
    }
    if (datatools::is_valid(_ic_m_shell_coefficient_)) {
      if (! datatools::is_valid(_ic_m_shell_binding_energy_)) {
        if (config_.has_key("ic.m_shell.binding_energy")) {
          double icmbe = config_.fetch_real("ic.m_shell.binding_energy");
          if (! config_.has_explicit_unit("ic.m_shell.binding_energy")) {
            icmbe *= default_energy_unit;
          }
          set_ic_m_shell_binding_energy(icmbe);
        }
      }
    } else {
      set_ic_m_shell_binding_energy(0.0);
    }

    // Pair conversion:
    if (! datatools::is_valid(_pc_coefficient_)) {
     if (config_.has_key("pair.coefficient")) {
        double pairc = config_.fetch_real("pair.coefficient");
        set_pc_coefficient(pairc);
      }
    }
    if (! datatools::is_valid(_pc_coefficient_)) {
      set_pc_coefficient(0.0);
    }

    _init();

    _set_initialized(true);
    return;
  }


  void nuclear_transition::_init()
  {
    double pg = 1.0;
    double pK = 0.0;
    double pL = 0.0;
    double pM = 0.0;
    double pp = 0.0;
    if (! (_approx_flags_ & APPROX_GAMMA_ONLY)) {

      if (datatools::is_valid(_ic_k_shell_coefficient_)) {
        DT_THROW_IF(_energy_ < _ic_k_shell_binding_energy_,
                    std::logic_error,
                    "Nuclear transition energy (" << _energy_ / CLHEP::keV << " keV) is not large enough for IC from K-shell (" << _ic_k_shell_binding_energy_ / CLHEP::keV << " keV) !");
        pK = _ic_k_shell_coefficient_;
      }
      if (datatools::is_valid(_ic_l_shell_coefficient_)) {
        DT_THROW_IF(_energy_ < _ic_l_shell_binding_energy_,
                    std::logic_error,
                    "Nuclear transition energy (" << _energy_ / CLHEP::keV << " keV) is not large enough for IC from L-shell (" << _ic_l_shell_binding_energy_ / CLHEP::keV << " keV)!");
        pL = _ic_l_shell_coefficient_;
      }
      if (datatools::is_valid(_ic_m_shell_coefficient_)) {
        DT_THROW_IF(_energy_ < _ic_m_shell_binding_energy_,
                    std::logic_error,
                    "Nuclear transition energy (" << _energy_ / CLHEP::keV << " keV) is not large enough for IC from M-shell (" << _ic_m_shell_binding_energy_ / CLHEP::keV << " keV)!");
        pM = _ic_m_shell_coefficient_;
      }
      if (datatools::is_valid(_pc_coefficient_)) {
        DT_THROW_IF(pp > 0.0 && _energy_ < 2 * CLHEP::electron_mass_c2,
                    std::logic_error,
                    "Nuclear transition energy (" << _energy_ / CLHEP::keV << " keV) is not large enough for pair conversion!");
        pp = _pc_coefficient_;
      }
    }
    _cumul_g_  = pg;
    _cumul_pK_ = _cumul_g_  + pK;
    _cumul_pL_ = _cumul_pK_ + pL;
    _cumul_pM_ = _cumul_pL_ + pM;
    _cumul_pp_ = _cumul_pM_ + pp;

    _cumul_g_  /= _cumul_pp_;
    _cumul_pK_ /= _cumul_pp_;
    _cumul_pL_ /= _cumul_pp_;
    _cumul_pM_ /= _cumul_pp_;
    _cumul_pp_  = 1.0;

    return;
  }

  void nuclear_transition::reset()
  {
    DT_THROW_IF(! is_initialized(), std::logic_error,
                "Nuclear transition is not initialized!");
    _set_initialized(false);

    _set_default();
    return;
  }

  bool nuclear_transition::has_A() const
  {
    return _A_ > 0;
  }

  void nuclear_transition::set_A(int A_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error,
                "Nuclear transition is already initialized!");
    DT_THROW_IF(A_ < 1, std::logic_error, "Invalid decaying nucleus' A (" << A_ << ") !");
    _A_ = A_;
    return;
  }

  int nuclear_transition::get_A() const
  {
    return _A_;
  }

  bool nuclear_transition::has_Z() const
  {
    return _Z_ > 0;
  }

  void nuclear_transition::set_Z(int val_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error,
                "Nuclear transition is already initialized!");
    DT_THROW_IF(val_ < 1 || val_ > 113,
                std::logic_error, "Invalid parent nucleus' Z !");
    _Z_ = val_;
    return;
  }

  int nuclear_transition::get_Z() const
  {
    return _Z_;
  }

  void nuclear_transition::set_levels(const nuclear_level & level_initial_,
                                      const nuclear_level & level_final_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error,
                "Nuclear transition is already initialized!");
    DT_THROW_IF(level_initial_.get_A() != level_final_.get_A() || level_initial_.get_Z() != level_final_.get_Z(),
                std::logic_error,
                "Unmatching nuclear levels '" << level_initial_.to_string() << "' and '" <<
                level_final_.to_string() << "'!");
    DT_THROW_IF(level_initial_.get_energy() <= level_final_.get_energy(), std::logic_error,
                "Invalid levels for gamma transition energy!");

    set_A(level_initial_.get_A());
    set_Z(level_initial_.get_Z());
    set_energy(level_initial_.get_energy() - level_final_.get_energy());

    return;
  }

  bool nuclear_transition::has_energy() const
  {
    return datatools::is_valid(_energy_);
  }

  void nuclear_transition::set_energy(double e_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error,
                "Nuclear transition is already initialized!");
    DT_THROW_IF(!datatools::is_valid(e_) && e_ <= 0.0, std::logic_error,
                "Nuclear transition energy is not valid!");
    _energy_ = e_;
    return;
  }

  double nuclear_transition::get_energy() const
  {
    return _energy_;
  }

  void nuclear_transition::tree_dump(std::ostream& out_,
                                     const std::string& title_,
                                     const std::string& indent_,
                                     bool inherit_) const
  {

    this->base_decay_driver::tree_dump(out_, title_, indent_, true);

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "A : " << _A_ << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Z : " << _Z_ << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Energy  : " << _energy_ / CLHEP::keV << " keV" << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Approx : " << _approx_flags_ << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "K-shell binding energy : " << _ic_k_shell_binding_energy_ / CLHEP::keV << " keV" << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "K-shell IC coefficient : " << _ic_k_shell_coefficient_ << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "L-shell binding energy : " << _ic_l_shell_binding_energy_ / CLHEP::keV << " keV" << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "L-shell IC coefficient : " << _ic_l_shell_coefficient_ << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "M-shell binding energy : " << _ic_m_shell_binding_energy_ / CLHEP::keV << " keV" << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "M-shell IC coefficient : " << _ic_m_shell_coefficient_ << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Pair conversion coefficient : " << _pc_coefficient_ << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Nuclear transition energy : " << _energy_ / CLHEP::keV << " keV" << std::endl;

    bool initialized = datatools::is_valid(_cumul_g_);
    if (initialized) {
      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Cumul prob (gamma)      : " << _cumul_g_ << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Cumul prob (IC K-shell) : " << _cumul_pK_ << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Cumul prob (IC L-shell) : " << _cumul_pL_ << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Cumul prob (IC M-shell) : " << _cumul_pM_ << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Cumul prob (pair)       : " << _cumul_pp_ << std::endl;

    }

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Gamma generated : " << _gamma_generated_ << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Conversion e- generated : " << _conversion_electron_generated_ << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
         << "Conversion e+/e- pair generated : " << _conversion_pair_generated_ << std::endl;

    return;
  }

  int nuclear_transition::fire_event_kinematics(mygsl::rng & prng_,
                                                mode_type & mode_,
                                                double & eg_,
                                                double & thetag_,
                                                double & phig_,
                                                double & k1_,
                                                double & theta1_,
                                                double & phi1_,
                                                double & k2_,
                                                double & theta2_,
                                                double & phi2_) const
  {
    mode_ = MODE_INVALID;
    datatools::invalidate(eg_);
    datatools::invalidate(thetag_);
    datatools::invalidate(phig_);
    datatools::invalidate(k1_);
    datatools::invalidate(theta1_);
    datatools::invalidate(phi1_);
    datatools::invalidate(k2_);
    datatools::invalidate(theta2_);
    datatools::invalidate(phi2_);
    double p = prng_.flat(0.0, +1.0);
    // if (_approx_flags_ & APPROX_GAMMA_ONLY) {
    //   p = 0.0;
    // }
    if (p < _cumul_g_) {
      // Fire a gamma ray:
      // std::cerr << "DEVEL: nuclear_transition::fire_event_kinematics: "
      //           << "Fire a gamma ray..."
      //           << std::endl;
      mode_ = MODE_GAMMA;
      eg_ = _energy_;
      double cost =  prng_.flat(-1.0, +1.0);
      double sint = std::sqrt(1 - cost * cost);
      thetag_ = std::atan2(sint, cost);
      phig_ = prng_.flat(0.0, 2 * M_PI);
    } else if (p < _cumul_pK_) {
      mode_ = MODE_IC_K;
      {
        // Fire an internal conversion electron from the K-shell:
        k1_ = _energy_ - _ic_k_shell_binding_energy_;
        double cost =  prng_.flat(-1.0, +1.0);
        double sint = std::sqrt(1 - cost * cost);
        theta1_ = std::atan2(sint, cost);
        phi1_ = prng_.flat(0.0, 2 * M_PI);
      }
      {
        // Fire a deexcitation X-ray:
        eg_ = _ic_k_shell_binding_energy_;
        double cost =  prng_.flat(-1.0, +1.0);
        double sint = std::sqrt(1 - cost * cost);
        thetag_ = std::atan2(sint, cost);
        phig_ = prng_.flat(0.0, 2 * M_PI);
      }
    } else if (p < _cumul_pL_) {
      mode_ = MODE_IC_L;
      {
        // Fire an internal conversion electron from the L-shell:
        k1_ = _energy_ - _ic_l_shell_binding_energy_;
        double cost =  prng_.flat(-1.0, +1.0);
        double sint = std::sqrt(1 - cost * cost);
        theta1_ = std::atan2(sint, cost);
        phi1_ = prng_.flat(0.0, 2 * M_PI);
      }
      {
        // Fire a deexcitation X-ray:
        eg_ = _ic_l_shell_binding_energy_;
        double cost =  prng_.flat(-1.0, +1.0);
        double sint = std::sqrt(1 - cost * cost);
        thetag_ = std::atan2(sint, cost);
        phig_ = prng_.flat(0.0, 2 * M_PI);
      }
    } else if (p < _cumul_pM_) {
      mode_ = MODE_IC_M;
      {
        // Fire an internal conversion electron from the M-shell:
        k1_ = _energy_ - _ic_m_shell_binding_energy_;
        double cost =  prng_.flat(-1.0, +1.0);
        double sint = std::sqrt(1 - cost * cost);
        theta1_ = std::atan2(sint, cost);
        phi1_ = prng_.flat(0.0, 2 * M_PI);
      }
      {
        // Fire a deexcitation X-ray:
        eg_ = _ic_m_shell_binding_energy_;
        double cost =  prng_.flat(-1.0, +1.0);
        double sint = std::sqrt(1 - cost * cost);
        thetag_ = std::atan2(sint, cost);
        phig_ = prng_.flat(0.0, 2 * M_PI);
      }
    } else {
      // Fire a conversion e+/e- pair :
      mode_ = MODE_PC;
      {
        k1_ = 0.5 * (_energy_ - 2.0 * CLHEP::electron_mass_c2);
        double cost =  prng_.flat(-1.0, +1.0);
        double sint = std::sqrt(1 - cost * cost);
        theta1_ = std::atan2(sint, cost);
        phi1_ = prng_.flat(0.0, 2 * M_PI);
        k2_ = k1_;
        // Same direction for now:
        theta2_ = theta1_;
        phi2_ = phi1_;
       }
    }

    return 0;
  }

  int nuclear_transition::fill(mygsl::rng & prng_, primary_event & event_)
  {
    // Fire kinematics:
    mode_type mode;
    double eg; // gamma or X
    double thetag;
    double phig;
    double ke1; // electron
    double theta1;
    double phi1;
    double ke2; // positron
    double theta2;
    double phi2;
    fire_event_kinematics(prng_, mode,
                          eg, thetag, phig,
                          ke1, theta1, phi1,
                          ke2, theta2, phi2);

    if (mode == MODE_GAMMA) {
      if (is_gamma_generated()) {
        // Gamma:
        primary_particle & gamma = event_.add_particle();
        gamma.set_type(primary_particle::GAMMA);
        gamma.set_time(0.0);
        //gamma.grab_auxiliaries().store("DPId", "genbb::nuclear_transition@gamma");
        kinematics gamma_kine;
        gamma_kine.initialize_from_m_k(0.0, eg);
        geomtools::set_r_theta_phi(gamma.grab_momentum(), gamma_kine.p(), thetag, phig);
      }
    } else if (mode == MODE_IC_K || mode == MODE_IC_L || mode == MODE_IC_M) {
      if (is_conversion_electron_generated()) {
        // Conversion electron:
        primary_particle & conversion_electron = event_.add_particle();
        conversion_electron.set_type(primary_particle::ELECTRON);
        conversion_electron.set_time(0.0);
        //conversion_electron.grab_auxiliaries().store("DPId", "genbb::nuclear_transition@conversion_electron");
        kinematics electron_kine;
        electron_kine.initialize_from_m_k(0.0, ke1);
        geomtools::set_r_theta_phi(conversion_electron.grab_momentum(), electron_kine.p(), theta1, phi1);
      }
      if (is_gamma_generated()) {
        // X:
        primary_particle & xray = event_.add_particle();
        xray.set_type(primary_particle::GAMMA);
        xray.set_time(0.0);
        //xray.grab_auxiliaries().store("DPId", "genbb::nuclear_transition@xray");
        kinematics xray_kine;
        xray_kine.initialize_from_m_k(0.0, eg);
        geomtools::set_r_theta_phi(xray.grab_momentum(), xray_kine.p(), thetag, phig);
      }
    } else {
      if (is_conversion_pair_generated()) {
        // Electron:
        primary_particle & electron = event_.add_particle();
        electron.set_type(primary_particle::ELECTRON);
        electron.set_time(0.0);
        //electron.grab_auxiliaries().store("DPId", "genbb::nuclear_transition@electron");
        kinematics electron_kine;
        electron_kine.initialize_from_m_k(0.0, ke1);
        geomtools::set_r_theta_phi(electron.grab_momentum(), electron_kine.p(), theta1, phi1);
        // Positron:
        primary_particle & positron = event_.add_particle();
        positron.set_type(primary_particle::POSITRON);
        positron.set_time(0.0);
        //positron.grab_auxiliaries().store("DPId", "genbb::nuclear_transition@positron");
        kinematics positron_kine;
        positron_kine.initialize_from_m_k(0.0, ke2);
        geomtools::set_r_theta_phi(positron.grab_momentum(), positron_kine.p(), theta2, phi2);
      }
    }

    return 0;
  }

} // end of namespace genbb
