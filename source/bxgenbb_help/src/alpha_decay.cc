// alpha_decay.cc
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
#include <genbb_help/alpha_decay.h>

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
// - Bayeux/materials:
#include <materials/isotope.h>
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

  GENBB_BDD_REGISTRATION_IMPLEMENT(alpha_decay, "genbb::alpha_decay");

  void alpha_decay::_set_default()
  {
    this->base_decay_driver::_set_default();
    datatools::invalidate(_q_alpha_);
    _A_ = 0;
    _Z_ = 0;
    datatools::invalidate(_mass_parent_);
    datatools::invalidate(_energy_parent_);
    datatools::invalidate(_mass_daughter_);
    datatools::invalidate(_energy_daughter_);
    _alpha_generated_ = true;
    _daughter_generated_ = false;

    return;
  }

  alpha_decay::alpha_decay() : base_decay_driver()
  {
    _set_default();
    return;
  }

  alpha_decay::~alpha_decay()
  {
    if (is_initialized()) {
      reset();
    }
    return;
  }

  void alpha_decay::initialize(const datatools::properties & config_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error,
                "Alpha decay is already initialized!");

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

    if (!datatools::is_valid(_mass_parent_)) {
      DT_THROW_IF(! config_.has_key("mass_parent"), std::logic_error,
                  "Missing 'mass_parent' property!");
      double mass_parent = config_.fetch_real("mass_parent");
      if (! config_.has_explicit_unit("mass_parent")) {
        mass_parent *= default_energy_unit;
      }
      std::cerr << "DEVEL: "
                << "alpha_decay::initialize: "
                << "mass_parent = " << mass_parent / CLHEP::keV << " keV"
                << std::endl;
      set_mass_parent(mass_parent);
    }

    double energy_parent = 0.0;
    if (!datatools::is_valid(_energy_parent_)) {
      if (config_.has_key("energy_parent")) {
        energy_parent = config_.fetch_real("energy_parent");
        if (! config_.has_explicit_unit("energy_parent")) {
          energy_parent *= default_energy_unit;
        }
      }
    }
    set_energy_parent(energy_parent);

    if (!datatools::is_valid(_mass_daughter_)) {
      DT_THROW_IF(! config_.has_key("mass_daughter"), std::logic_error, "Missing 'mass_daughter' property!");
      double mass_daughter = config_.fetch_real("mass_daughter");
      if (! config_.has_explicit_unit("mass_daughter")) {
        mass_daughter *= default_energy_unit;
      }
      std::cerr << "DEVEL: "
                << "alpha_decay::initialize: "
                << "mass_daughter = " << mass_daughter / CLHEP::keV << " keV"
                << std::endl;
      set_mass_daughter(mass_daughter);
    }

    double energy_daughter = 0.0;
    if (!datatools::is_valid(_energy_daughter_)) {
      if (config_.has_key("energy_daughter")) {
        energy_daughter = config_.fetch_real("energy_daughter");
        if (! config_.has_explicit_unit("energy_daughter")) {
          energy_daughter *= default_energy_unit;
        }
      }
    }
    set_energy_daughter(energy_daughter);

    // Q-alpha value:
    if (!datatools::is_valid(_q_alpha_)) {
      if (config_.has_key("q_alpha")) {
        double q_alpha = config_.fetch_real("q_alpha");
        if (! config_.has_explicit_unit("q_alpha")) {
          q_alpha *= default_energy_unit;
        }
        set_q_alpha(q_alpha);
      }
    }

    if (config_.has_key("alpha.generated")) {
      set_alpha_generated(config_.fetch_boolean("alpha.generated"));
    }

    if (config_.has_key("daughter.generated")) {
      set_daughter_generated(config_.fetch_boolean("daughter.generated"));
    }

    _init();

    _set_initialized(true);
    return;
  }

  void alpha_decay::_init()
  {
    // DT_THROW_IF(_Z_parent_ != _Z_daughter_ + 1,
    //             std::logic_error,
    //             "Incompatible parent nucleus Z and daughter nucleus Z for alpha+ decay !");

    static double alpha_mass = std::numeric_limits<double>::quiet_NaN();
    if (!datatools::is_valid(alpha_mass)) {
      materials::isotope::id id_alpha(2, 4);
      DT_THROW_IF(! materials::isotope::id_is_tabulated(id_alpha),
                  std::logic_error,
                  "Alpha Id '" << id_alpha.to_string() << "' is not tabulated!");
      const materials::isotope::record_type & rec_alpha =
        materials::isotope::table_record_from_id(id_alpha);
      alpha_mass = rec_alpha.get_nucleus_mass();
    }

    // Q-alpha:
    double delta_mass = (_mass_parent_ + _energy_parent_) - (_mass_daughter_ + _energy_daughter_);
    double check_q_alpha = delta_mass - alpha_mass;
    if (datatools::is_valid(_q_alpha_)) {
      // Check Q-alpha:
      DT_THROW_IF(std::abs(check_q_alpha - _q_alpha_) > 1. * CLHEP::keV,
                  std::logic_error,
                  "Q(alpha)=" << _q_alpha_ / CLHEP::keV
                  << " keV is not compatible with difference of masses (should be "
                  << check_q_alpha / CLHEP::keV << " keV) !");
    } else {
      // Compute Q-alpha:
      set_q_alpha(check_q_alpha);
    }

    _M_  = _mass_parent_;
    _m1_ = alpha_mass;
    _m2_ = _mass_daughter_;
    _tbd_.set(_M_, _m1_, _m2_);

    return;
  }

  void alpha_decay::reset()
  {
    DT_THROW_IF(! is_initialized(), std::logic_error,
                "Alpha decay is not initialized!");
    _set_initialized(false);

    _set_default();
    return;
  }

  bool alpha_decay::has_A() const
  {
    return _A_ > 0;
  }

  void alpha_decay::set_A(int A_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error,
                "Alpha decay is already initialized!");
    DT_THROW_IF(A_ < 5, std::logic_error, "Invalid decaying nucleus A (" << A_ << ") !");
    _A_ = A_;
    return;
  }

  bool alpha_decay::has_q_alpha() const
  {
    return datatools::is_valid(_q_alpha_);
  }

  void alpha_decay::set_q_alpha(double qa_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error,
                "Alpha transition is already initialized!");
    DT_THROW_IF(!datatools::is_valid(qa_) || qa_ <= 0.0, std::logic_error,
                "Q(alpha) is not valid!");
    _q_alpha_ = qa_;
    return;
  }

  double alpha_decay::get_q_alpha() const
  {
    return _q_alpha_;
  }

  int alpha_decay::get_A_parent() const
  {
    return _A_;
  }

  int alpha_decay::get_Z_daughter() const
  {
    return _Z_ - 2;
  }

  int alpha_decay::get_A_daughter() const
  {
    return _A_ - 4;
  }

  int alpha_decay::get_Z_parent() const
  {
    return _Z_;
  }

  int alpha_decay::get_A() const
  {
    return _A_;
  }


  bool alpha_decay::has_Z() const
  {
    return _Z_ > 0;
  }

  void alpha_decay::set_Z(int val_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error,
                "Alpha decay is already initialized!");
    DT_THROW_IF(val_ < 1 || val_ > 113,
                std::logic_error, "Invalid parent nucleus Z !");
    _Z_ = val_;
    return;
  }

  int alpha_decay::get_Z() const
  {
    return _Z_;
  }

  void alpha_decay::set_mass_parent(double val_)
  {
    DT_THROW_IF(val_ <= 0.0,
                std::logic_error, "Invalid parent nucleus mass !");
    _mass_parent_ = val_;
    return;
  }

  double alpha_decay::get_mass_parent() const
  {
    return _mass_parent_;
  }

  void alpha_decay::set_mass_daughter(double val_)
  {
    DT_THROW_IF(val_ <= 0.0,
                std::logic_error, "Invalid daughter nucleus mass !");
    _mass_daughter_ = val_;
    return;
  }

  double alpha_decay::get_mass_daughter() const
  {
    return _mass_daughter_;
  }

  void alpha_decay::set_energy_parent(double val_)
  {
    DT_THROW_IF(val_ < 0.0,
                std::logic_error, "Invalid parent nucleus excitation energy !");
    _energy_parent_ = val_;
    return;
  }

  double alpha_decay::get_energy_parent() const
  {
    return _energy_parent_;
  }

  void alpha_decay::set_energy_daughter(double val_)
  {
    DT_THROW_IF(val_ < 0.0,
                std::logic_error, "Invalid daughter nucleus excitation energy !");
    _energy_daughter_ = val_;
    return;
  }

  double alpha_decay::get_energy_daughter() const
  {
    return _energy_daughter_;
  }

  bool alpha_decay::is_alpha_generated() const
  {
    return _alpha_generated_;
  }

  void alpha_decay::set_alpha_generated(bool g_)
  {
    _alpha_generated_ = g_;
    return;
  }

  bool alpha_decay::is_daughter_generated() const
  {
    return _daughter_generated_;
  }

  void alpha_decay::set_daughter_generated(bool g_)
  {
    _daughter_generated_ = g_;
    return;
  }

  void alpha_decay::set_levels(const nuclear_level & level_initial_,
                               const nuclear_level & level_final_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error,
                "Alpha decay is already initialized!");
    DT_THROW_IF(level_initial_.get_A() != level_final_.get_A() - 4
                || level_initial_.get_Z() != level_final_.get_Z() - 2,
                std::logic_error,
                "Unmatching nuclear levels '" << level_initial_.to_string() << "' and '" <<
                level_final_.to_string() << "'!");

    set_A(level_initial_.get_A());
    set_Z(level_initial_.get_Z());
    materials::isotope::id id_parent(_Z_, _A_);
    materials::isotope::id id_daughter(_Z_ - 2, _A_ - 4);
    DT_THROW_IF(! materials::isotope::id_is_tabulated(id_parent),
                std::logic_error,
                "Parent isotope Id '" << id_parent.to_string() << "' is not tabulated!");
    DT_THROW_IF(! materials::isotope::id_is_tabulated(id_daughter),
                std::logic_error,
                "Daughter isotope Id '" << id_daughter.to_string() << "' is not tabulated!");
    const materials::isotope::record_type & rec_parent =
      materials::isotope::table_record_from_id(id_parent);
    const materials::isotope::record_type & rec_daughter =
      materials::isotope::table_record_from_id(id_daughter);
    double mass_parent = rec_parent.get_nucleus_mass();
    double mass_daughter = rec_daughter.get_nucleus_mass();
    double energy_parent = level_initial_.get_energy();
    double energy_daughter = level_final_.get_energy();
    set_mass_parent(mass_parent);
    set_energy_parent(energy_parent);
    set_mass_daughter(mass_daughter);
    set_energy_daughter(energy_daughter);

    return;
  }

  void alpha_decay::tree_dump(std::ostream& out_,
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
         << "Mass(parent)     : " << _mass_parent_ / CLHEP::MeV << " MeV" << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Energy(parent)   : " << _energy_parent_ / CLHEP::MeV << " MeV" << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Mass(daughter)   : " << _mass_daughter_ / CLHEP::MeV << " MeV" << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Energy(daughter) : " << _energy_daughter_ / CLHEP::MeV << " MeV" << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Q(alpha) : " << _q_alpha_ / CLHEP::keV << " keV" << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Alpha generated  : " << _alpha_generated_ << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
         << "Daughter generated : " << _daughter_generated_ << std::endl;


    return;
  }

  int alpha_decay::fire_event_kinematics(mygsl::rng & prng_,
                                         double & ka_,
                                         double & thetaa_,
                                         double & phia_,
                                         double & kr_,
                                         double & thetar_,
                                         double & phir_) const
  {
    // Alpha
    double ea = _tbd_.daughter1().w(); // (_M_ * _M_ - _m2_ * _m2_ + _m1_ * _m1_) / (2 * _M_);
    ka_ = _tbd_.daughter1().k();
    double costa =  prng_.flat(-1.0, +1.0);
    double sinta = std::sqrt(1 - costa * costa);
    thetaa_ = std::atan2(sinta, costa);
    phia_ = prng_.flat(0.0, 2 * M_PI);
    double er = _tbd_.daughter2().w(); // (_M_ * _M_ - _m2_ * _m2_ + _m1_ * _m1_) / (2 * _M_);
    kr_ = _tbd_.daughter2().k();
    // double er = (_M_ * _M_ - _m1_ * _m1_ + _m2_ * _m2_) / (2 * _M_);
    // kr_ = er - _m2_;
    thetar_ = M_PI - thetaa_;
    phir_ = phia_ + M_PI;
    return 0;
  }

  int alpha_decay::fill(mygsl::rng & prng_, primary_event & event_)
  {
    // Fire kinematics:
    double alpha_kinetic_energy;
    double alpha_theta;
    double alpha_phi;
    double recoil_kinetic_energy;
    double recoil_theta;
    double recoil_phi;
    fire_event_kinematics(prng_,
                          alpha_kinetic_energy, alpha_theta, alpha_phi,
                          recoil_kinetic_energy, recoil_theta, recoil_phi);

    if (_alpha_generated_) {
      // Alpha:
      primary_particle & alpha = event_.add_particle();
      alpha.set_type(primary_particle::ALPHA);
      alpha.set_time(0.0);
      //alpha.grab_auxiliaries().store("DPId", "genbb::alpha_decay@alpha");
      kinematics alpha_kine;
      alpha_kine.initialize_from_m_k(0.0, alpha_kinetic_energy);
      geomtools::vector_3d alpha_momentum;
      geomtools::set_r_theta_phi(alpha.grab_momentum(), alpha_kine.p(), alpha_theta, alpha_phi);
    }

    if (_daughter_generated_) {
      // Recoil:
      primary_particle & recoil = event_.add_particle();
      recoil.set_nucleus(_Z_ - 2, _A_ - 4, 0.0);
      //recoil.grab_auxiliaries().store("DPId", "genbb::alpha_decay@recoil");
      kinematics recoil_kine;
      recoil_kine.initialize_from_m_k(0.0, recoil_kinetic_energy);
      geomtools::set_r_theta_phi(recoil.grab_momentum(), recoil_kine.p(), recoil_theta, recoil_phi);
    }

    return 0;
  }

} // end of namespace genbb
