// beta_decay.cc
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
#include <genbb_help/beta_decay.h>

// Standard library:
#include <limits>
#include <cmath>
#include <set>

// Third party:
// - CLHEP:
#include<CLHEP/Units/PhysicalConstants.h>
#include<CLHEP/Units/SystemOfUnits.h>
// - GSL:
#include<gsl/gsl_math.h>
// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/logger.h>
#include <datatools/exception.h>
#include <datatools/temporary_files.h>
// - Bayeux/materials:
#include <materials/isotope.h>
// - Bayeux/geomtools:
#include <geomtools/gnuplot_i.h>
#include <geomtools/gnuplot_drawer.h>
// - Bayeux/genbb_help:
#include <genbb_help/kinematics.h>
#include <genbb_help/primary_particle.h>
#include <genbb_help/fermi_function.h>
#include <genbb_help/nuclear_level.h>

namespace genbb {

  GENBB_BDD_REGISTRATION_IMPLEMENT(beta_decay, "genbb::beta_decay")

  // static
  const int beta_decay::ESO_INVALID_FIXED_CHARGE;

  // static
  std::string beta_decay::label_from_decay_type(decay_type type_)
  {
    switch(type_) {
    case BETA_DECAY_MINUS:
      return "beta-";
    case BETA_DECAY_PLUS:
      return "beta+";
    default:
      return "";
    }
  }

  // static
  beta_decay::decay_type
  beta_decay::decay_type_from_label(const std::string & label_)
  {
    if (label_ == label_from_decay_type(BETA_DECAY_MINUS)) return BETA_DECAY_MINUS;
    else if (label_ == label_from_decay_type(BETA_DECAY_PLUS)) return BETA_DECAY_PLUS;
    else return BETA_DECAY_INVALID;
  }

  // static
  std::string beta_decay::label_from_coupling(coupling_type coupling_)
  {
    switch(coupling_) {
    case COUPLING_AXIAL_VECTOR:
      return "A";
    case COUPLING_TENSOR:
      return "T";
    case COUPLING_SCALAR:
      return "S";
    case COUPLING_VECTOR:
      return "V";
    case COUPLING_USER:
      return "user";
    case COUPLING_UNKNOWN:
      return "unknown";
    default:
      return "";
    }
  }

  // static
  beta_decay::coupling_type
  beta_decay::coupling_from_label(const std::string & label_)
  {
    if (label_ == label_from_coupling(COUPLING_AXIAL_VECTOR)) return COUPLING_AXIAL_VECTOR;
    else if (label_ == label_from_coupling(COUPLING_TENSOR)) return COUPLING_TENSOR;
    else if (label_ == label_from_coupling(COUPLING_SCALAR)) return COUPLING_SCALAR;
    else if (label_ == label_from_coupling(COUPLING_VECTOR)) return COUPLING_VECTOR;
    else if (label_ == label_from_coupling(COUPLING_USER)) return COUPLING_USER;
    else return COUPLING_INVALID;
  }


  // static
  double beta_decay::a_value(coupling_type coupling_)
  {
    switch(coupling_) {
    case COUPLING_AXIAL_VECTOR:
      return -1./3;
    case COUPLING_TENSOR:
      return +1./3;
    case COUPLING_SCALAR:
      return -1.;
    case COUPLING_VECTOR:
      return +1.;
    default:
      return std::numeric_limits<double>::quiet_NaN();
    }
  }

  void beta_decay::set_log_filename(const std::string & log_)
  {
    _log_filename_ = log_;
    return;
  }

  const std::string & beta_decay::get_log_filename() const
  {
    return _log_filename_;
  }

  bool beta_decay::is_beta_generated() const
  {
    return _beta_generated_;
  }

  void beta_decay::set_beta_generated(bool g_)
  {
    _beta_generated_ = g_;
    return;
  }

  bool beta_decay::is_neutrino_generated() const
  {
    return _neutrino_generated_;
  }

  void beta_decay::set_neutrino_generated(bool g_)
  {
    _neutrino_generated_ = g_;
    return;
  }

  bool beta_decay::is_daughter_generated() const
  {
    return _daughter_generated_;
  }

  void beta_decay::set_daughter_generated(bool g_)
  {
    _daughter_generated_ = g_;
    return;
  }


  void beta_decay::_set_default()
  {
    this->base_decay_driver::_set_default();
    _type_ = BETA_DECAY_INVALID;
    datatools::invalidate(_q_beta_);
    _A_ = 0;
    _Z_parent_ = 0;
    _Z_daughter_ = 0;
    datatools::invalidate(_mass_parent_);
    datatools::invalidate(_energy_parent_);
    datatools::invalidate(_mass_daughter_);
    datatools::invalidate(_energy_daughter_);
    _coupling_ = COUPLING_INVALID;
    datatools::invalidate(_a_);
    datatools::invalidate(_ke_cut_);
    _electron_shakeoff_mode_ = ESO_NO_SHAKEOFF;
    _electron_shakeoff_fixed_charge_ = ESO_INVALID_FIXED_CHARGE;

    _massive_neutrino_   = false;
    datatools::invalidate(_neutrino_mass_);
    _beta_generated_     = true;
    _neutrino_generated_ = false;
    _daughter_generated_ = false;

    // Reset working data:
    datatools::invalidate(_M_);
    datatools::invalidate(_m1_);
    datatools::invalidate(_m2_);
    datatools::invalidate(_m3_);
    datatools::invalidate(_pr_max_);
    datatools::invalidate(_kr_max_);
    datatools::invalidate(_probability_max_);
    return;
  }

  beta_decay::beta_decay() : base_decay_driver()
  {
    _set_default();
    return;
  }

  beta_decay::~beta_decay()
  {
    if (is_initialized()) {
      reset();
    }
    return;
  }

  void beta_decay::reset()
  {
    DT_THROW_IF(! is_initialized(), std::logic_error,
                "Beta decay is not initialized!");
    _set_initialized(false);

    if (_log_file_) {
      *_log_file_ << "# end of log file" << std::endl;
      _log_file_.reset();
    }

    if (_F_) {
      _F_.reset();
    }

    _electron_shakeoff_data_random_.clear();

    _set_default();
    return;
  }

  void beta_decay::set_levels(const nuclear_level & level_initial_,
                              const nuclear_level & level_final_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error,
                "Beta decay transition is already initialized!");

    if (get_logging() == datatools::logger::PRIO_DEBUG) {
      DT_LOG_DEBUG(get_logging(), "Initial level:");
      level_initial_.tree_dump(std::cerr, "", "[debug] ");
      DT_LOG_DEBUG(get_logging(), "Final level:");
      level_final_.tree_dump(std::cerr, "", "[debug] ");
    }

    DT_THROW_IF(level_initial_.get_A() != level_final_.get_A(),
                std::logic_error,
                "Incompatible beta decay A for levels  '" << level_initial_.to_string() << "' and '" <<
                level_final_.to_string() << "'!");

    int dz = level_initial_.get_Z() - level_final_.get_Z();
    if (dz == -1) {
      // (Z,A) --> (Z+1,A) + e- + nu_e_bar
      set_type(BETA_DECAY_MINUS);
    } else if (dz == +1) {
      // (Z,A) --> (Z-1,A) + e+ + nu_e
      set_type(BETA_DECAY_PLUS);
    } else {
      DT_THROW(std::logic_error,
               "Incompatible beta decay Z for levels  '" << level_initial_.to_string() << "' and '" <<
               level_final_.to_string() << "'!");
    }
    set_A(level_initial_.get_A());
    set_Z_parent(level_initial_.get_Z());
    set_Z_daughter(level_final_.get_Z());
    materials::isotope::id id_parent(level_initial_.get_Z(), level_initial_.get_A());
    materials::isotope::id id_daughter(level_final_.get_Z(), level_initial_.get_A());
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
    DT_LOG_DEBUG(get_logging(), "Energy parent= " << energy_parent / CLHEP::keV << " keV");
    double energy_daughter = level_final_.get_energy();
    DT_LOG_DEBUG(get_logging(), "Energy daughter= " << energy_daughter / CLHEP::keV << " keV");
    set_mass_parent(mass_parent);
    set_energy_parent(energy_parent);
    set_mass_daughter(mass_daughter);
    set_energy_daughter(energy_daughter);

    return;
  }

  void beta_decay::initialize(const datatools::properties & config_)
  {
    DT_LOG_DEBUG(get_logging(), "TEST: Entering...");
    DT_THROW_IF(is_initialized(), std::logic_error, "Beta decay is a already initialized!");

    this->base_decay_driver::_initialize(config_);

    // config_.tree_dump(std::cerr, "beta_decay::initialize: ", "$$$ DEVEL: ");

    double default_energy_unit = CLHEP::keV;

    if (_type_ == BETA_DECAY_INVALID) {
      DT_THROW_IF(! config_.has_key("type"), std::logic_error, "Missing 'type' property!");
      std::string type_label = config_.fetch_string("type");
      if (type_label == "beta+") {
        set_type(BETA_DECAY_PLUS);
      } else if (type_label == "beta-") {
        set_type(BETA_DECAY_MINUS);
      } else {
        DT_THROW_IF(true, std::logic_error, "Invalid 'type' property!");
      }
    }

    if (_A_ == 0) {
      DT_THROW_IF(! config_.has_key("A"), std::logic_error, "Missing 'A' property!");
      int A = config_.fetch_integer("A");
      set_A(A);
    }

    if (_Z_parent_ == 0) {
      DT_THROW_IF(! config_.has_key("Z_parent"), std::logic_error, "Missing 'Z_parent' property!");
      int Z_parent = config_.fetch_integer("Z_parent");
      set_Z_parent(Z_parent);
    }

    if (_Z_daughter_ == 0) {
      DT_THROW_IF(! config_.has_key("Z_daughter"), std::logic_error,
                  "Missing 'Z_daughter' property!");
      int Z_daughter = config_.fetch_integer("Z_daughter");
      set_Z_daughter(Z_daughter);
    }

    if (!datatools::is_valid(_mass_parent_)) {
      DT_THROW_IF(! config_.has_key("mass_parent"), std::logic_error,
                  "Missing 'mass_parent' property!");
      double mass_parent = config_.fetch_real("mass_parent");
      if (! config_.has_explicit_unit("mass_parent")) {
        mass_parent *= default_energy_unit;
      }
      // std::cerr << "DEVEL: "
      //           << "beta_decay::initialize: "
      //           << "mass_parent = " << mass_parent / CLHEP::keV << " keV"
      //           << std::endl;
      set_mass_parent(mass_parent);
    }

    DT_LOG_DEBUG(get_logging(), "Energy parent   = " << _energy_parent_   / CLHEP::keV << " keV");
    DT_LOG_DEBUG(get_logging(), "Energy daughter = " << _energy_daughter_ / CLHEP::keV << " keV");

    if (!datatools::is_valid(_energy_parent_)) {
      double energy_parent = 0.0;
      if (config_.has_key("energy_parent")) {
        energy_parent = config_.fetch_real("energy_parent");
        if (! config_.has_explicit_unit("energy_parent")) {
          energy_parent *= default_energy_unit;
        }
      }
      set_energy_parent(energy_parent);
    }

    if (!datatools::is_valid(_mass_daughter_)) {
      DT_THROW_IF(! config_.has_key("mass_daughter"), std::logic_error, "Missing 'mass_daughter' property!");
      double mass_daughter = config_.fetch_real("mass_daughter");
      if (! config_.has_explicit_unit("mass_daughter")) {
        mass_daughter *= default_energy_unit;
      }
      // std::cerr << "DEVEL: "
      //           << "beta_decay::initialize: "
      //           << "mass_daughter = " << mass_daughter / CLHEP::keV << " keV"
      //           << std::endl;
      set_mass_daughter(mass_daughter);
    }

    if (!datatools::is_valid(_energy_daughter_)) {
      double energy_daughter = 0.0;
      if (config_.has_key("energy_daughter")) {
        energy_daughter = config_.fetch_real("energy_daughter");
        if (! config_.has_explicit_unit("energy_daughter")) {
          energy_daughter *= default_energy_unit;
        }
      }
      set_energy_daughter(energy_daughter);
    }

    // Q-beta value:
    if (!datatools::is_valid(_q_beta_)) {
      // DT_THROW_IF(! config_.has_key("q_beta"), std::logic_error, "Missing 'q_beta' property!");
      if (config_.has_key("q_beta")) {
        double q_beta = config_.fetch_real("q_beta");
        if (! config_.has_explicit_unit("q_beta")) {
          q_beta *= default_energy_unit;
        }
        set_q_beta(q_beta);
      }
    }

    if (_coupling_ == COUPLING_INVALID) {
      DT_THROW_IF(! config_.has_key("coupling"), std::logic_error, "Missing 'coupling' property!");
      std::string coupling_label = config_.fetch_string("coupling");
      coupling_type coupling = coupling_from_label(coupling_label);
      DT_THROW_IF(coupling == COUPLING_INVALID, std::logic_error, "Invalid 'coupling' property!");
      set_coupling(coupling);
    }

    if (_coupling_ == COUPLING_USER) {
      if (!datatools::is_valid(_a_)) {
        DT_THROW_IF(! config_.has_key("a"), std::logic_error, "Missing 'a' property!");
        double a = config_.fetch_real("a");
        set_a(a);
      }
    }

    if (!datatools::is_valid(_ke_cut_)) {
      if(config_.has_key("ke_cut")) {
        double ke_cut = config_.fetch_real("ke_cut");
        if (! config_.has_explicit_unit("ke_cut")) {
          ke_cut *= default_energy_unit;
        }
        set_ke_cut(ke_cut);
      }
    }

    if (! is_electron_shakeoff()) {

      if (config_.has_key("electron.shakeoff")) {
        std::string eso_mode = config_.fetch_string("electron.shakeoff");
        if (eso_mode == "fixed") {
          set_electron_shakeoff(ESO_FIXED_CHARGE);
        } else if (eso_mode == "random") {
          set_electron_shakeoff(ESO_RANDOM_CHARGE);
        } else if (eso_mode == "no") {
          set_electron_shakeoff(ESO_NO_SHAKEOFF);
        }
      }

      if (_electron_shakeoff_mode_ == ESO_FIXED_CHARGE) {

        if (config_.has_key("electron.shakeoff.fixed.charge")) {
          int eso_fixed_charge = config_.fetch_integer("electron.shakeoff.fixed.charge");
          set_electron_shakeoff_fixed_charge(eso_fixed_charge);
        } else {
          DT_THROW(std::logic_error, "Missing 'electron.shakeoff.fixed.charge' property!");
        }

      } else if (_electron_shakeoff_mode_ == ESO_RANDOM_CHARGE) {

        std::vector<int>    so_charges;       // Electron shakeoff final charge states
        std::vector<double> so_probabilities; // Associated probabilities

        if (config_.has_key("electron.shakeoff.random.charges")) {
          config_.fetch("electron.shakeoff.random.charges", so_charges);
        } else {
          DT_THROW(std::logic_error, "Missing 'electron.shakeoff.random.charges' property!");
        }
        if (config_.has_key("electron.shakeoff.random.probabilities")) {
          // Hmm! We should make sure no unit is passed...
          config_.fetch("electron.shakeoff.random.probabilities", so_probabilities);
        } else {
          DT_THROW(std::logic_error, "Missing 'electron.shakeoff.random.probabilities' property!");
        }
        DT_THROW_IF(so_charges.size() != so_probabilities.size(),
                    std::logic_error,
                    "Unmatching electron shakeoff random charge/probabilities!");
        for (int i = 0; i < (int) so_charges.size(); i++) {
          add_electron_shakeoff_random(so_charges[i], so_probabilities[i]);
        }

      }

    }

    if (config_.has_key("beta.generated")) {
      set_beta_generated(config_.fetch_boolean("beta.generated"));
    }

    if (coupling_is_known()) {

      if (config_.has_key("neutrino.generated")) {
        set_neutrino_generated(config_.fetch_boolean("neutrino.generated"));
      }

      if (config_.has_key("daughter.generated")) {
        set_daughter_generated(config_.fetch_boolean("daughter.generated"));
      }

    } else {
      set_neutrino_generated(false);
      set_daughter_generated(false);
    }

    // Hidden initialization
    _init();

    if (get_logging() == datatools::logger::PRIO_DEBUG) {
      DT_LOG_DEBUG(get_logging(), "Beta decay: ");
      this->tree_dump(std::clog, "", "[debug] ");
      // display_pdf("Beta kinematic limits");
    }
    _set_initialized(true);
    DT_LOG_DEBUG(get_logging(), "TEST: Exiting.");
    return;
  }

  void beta_decay::_init_electron_shakeoff()
  {
    if (_electron_shakeoff_mode_ == ESO_FIXED_CHARGE) {
      DT_THROW_IF(_electron_shakeoff_fixed_charge_ > _Z_daughter_,
                  std::logic_error,
                  "Invalid electron shakeoff fixed charge state ["
                  << _electron_shakeoff_fixed_charge_ << "] for the recoil ion!");
    }

    if (_electron_shakeoff_mode_ == ESO_RANDOM_CHARGE) {
      double prob_cumul = 0.0;
      std::set<int> charges;
      for (int i = 0; i < (int) _electron_shakeoff_data_random_.size(); i++) {
        int charge = _electron_shakeoff_data_random_[i].charge;
        DT_THROW_IF(charges.count(charge),
                    std::logic_error,
                    "Electron shakeoff random charge state ["
                    << charge << "] is already registered!");
        charges.insert(charge);
        DT_THROW_IF(charge > _Z_daughter_,
                    std::logic_error,
                    "Invalid electron shakeoff random charge state ["
                    << _electron_shakeoff_fixed_charge_ << "] for the recoil ion!");
        prob_cumul += _electron_shakeoff_data_random_[i].probability;
        _electron_shakeoff_data_random_[i].cumul_probability = prob_cumul;
      }
      for (int i = 0; i < (int) _electron_shakeoff_data_random_.size(); i++) {
        _electron_shakeoff_data_random_[i].cumul_probability /= prob_cumul;
      }
    }

    return;
  }

  void beta_decay::_init()
  {
    DT_LOG_DEBUG(get_logging(), "TEST: Entering...");
    if (_massive_neutrino_) {
      DT_THROW(std::logic_error,
               "Beta decay with massive netrino is not supported yet !");
    }
    if (is_beta_plus()) {
      DT_THROW_IF(_Z_parent_ != _Z_daughter_ + 1,
                  std::logic_error,
                  "Incompatible parent nucleus' Z and aughter nucleus' Z for beta+ decay !");
    }
    else if (is_beta_minus()) {
      DT_THROW_IF(_Z_parent_ != _Z_daughter_ - 1,
                  std::logic_error,
                  "Incompatible parent nucleus' Z and aughter nucleus' Z for beta- decay !");
    }

    // Q-beta:
    DT_LOG_DEBUG(get_logging(), "Mass parent = " << _mass_parent_ / CLHEP::keV << " keV");
    DT_LOG_DEBUG(get_logging(), "Mass daughter = " << _mass_daughter_ / CLHEP::keV << " keV");
    DT_LOG_DEBUG(get_logging(), "Energy parent = " << _energy_parent_ / CLHEP::keV << " keV");
    DT_LOG_DEBUG(get_logging(), "Energy daughter = " << _energy_daughter_ / CLHEP::keV << " keV");
    double delta_mass = (_mass_parent_ + _energy_parent_) - (_mass_daughter_ + _energy_daughter_);
    DT_LOG_DEBUG(get_logging(), "Delta mass = " << delta_mass / CLHEP::keV << " keV");
    double check_q_beta = delta_mass - CLHEP::electron_mass_c2;
    DT_LOG_DEBUG(get_logging(), "Checked Q beta = " << check_q_beta / CLHEP::keV << " keV");
    if (datatools::is_valid(_q_beta_)) {
      // Check Q-beta:
      DT_THROW_IF(std::abs(check_q_beta - _q_beta_) > 1. * CLHEP::keV,
                  std::logic_error,
                  "Q(beta)=" << _q_beta_ / CLHEP::keV
                  << " keV is not compatible with difference of masses (should be "
                  << check_q_beta / CLHEP::keV << " keV) !");
    } else {
      // Compute Q-beta:
      set_q_beta(check_q_beta);
    }

    // XXX TEST
    // if (get_logging() == datatools::logger::PRIO_DEBUG) {
    //   // Force arbitrary Q beta:
    //   _q_beta_ = 3.0 * CLHEP::MeV;
    //   set_log_filename("/tmp/_genbb.log");
    // }

    if (!datatools::is_valid(_ke_cut_)) {
      if (is_beta_plus()) {
        _ke_cut_ = 0.0;
      } else if (is_beta_minus()) {
        // Force low energy cut for beta- decay:
        _ke_cut_ = _q_beta_ / 500;
        if (_ke_cut_ > 10.0 * CLHEP::keV) {
          _ke_cut_ = 10.0 * CLHEP::keV;
        }
      }
    }

    if (! _log_filename_.empty()) {
      std::string log_filename = _log_filename_;
      datatools::fetch_path_with_env(log_filename);
      _log_file_.reset(new std::ofstream(log_filename.c_str()));
      DT_THROW_IF (!*_log_file_, std::runtime_error,
                   "Cannot open output log file '" << _log_filename_ << "'!");
      _log_file_->precision(15);
    }

    if (is_electron_shakeoff()) {
      _init_electron_shakeoff();
    }

    unsigned int mode_SS = fermi_function::MODE_SPHERICALLY_SYMMETRIC;
    // mode_SS = fermi_function::MODE_NON_RELATIVISTIC;
    mode_SS |= fermi_function::MODE_SHAPE_ONLY;
    // double ke_cut = 1.0 * CLHEP::keV;
    double ke_cut = _ke_cut_;
    _F_.reset(new fermi_function(this->get_Z_daughter(),
                                 this->get_A(),
                                 (this->is_beta_minus() ?
                                  fermi_function::DT_BETA_MINUS :
                                  fermi_function::DT_BETA_PLUS),
                                 mode_SS,
                                 ke_cut));

    _M_  = _mass_parent_ + _energy_parent_;
    _m1_ = CLHEP::electron_mass_c2;
    _m2_ = 0.0; // zero neutrino mass
    if (_massive_neutrino_) {
      // Not used yet:
      _m2_ = _neutrino_mass_;
    }
    _m3_ = _mass_daughter_ + _energy_daughter_;
    _compute_pr_max();
    _compute_limits();

    DT_LOG_DEBUG(get_logging(), "TEST: Exiting.");
    return;
  }

  void beta_decay::set_type(decay_type type_)
  {
    _type_ = type_;
    return;
  }

  beta_decay::decay_type beta_decay::get_type() const
  {
    return _type_;
  }

  bool beta_decay::is_beta_minus() const
  {
    return _type_ == BETA_DECAY_MINUS;
  }

  bool beta_decay::is_beta_plus() const
  {
    return _type_ == BETA_DECAY_PLUS;
  }

  bool beta_decay::has_q_beta() const
  {
    return datatools::is_valid(_q_beta_);
  }


  void beta_decay::set_q_beta(double q_beta_)
  {
    DT_THROW_IF(!datatools::is_valid(q_beta_) || q_beta_ <= 0.0,
                std::logic_error,
                "Invalid decaying nucleus Q beta (" << q_beta_ / CLHEP::keV << " keV)!");
    _q_beta_ = q_beta_;
    return;
  }

  double beta_decay::get_q_beta() const
  {
    return _q_beta_;
  }

  void beta_decay::set_A(int A_)
  {
    DT_THROW_IF(A_ < 1, std::logic_error, "Invalid decaying nucleus' A (" << A_ << ") !");
    _A_ = A_;
    return;
  }

  int beta_decay::get_A() const
  {
    return _A_;
  }

  void beta_decay::set_Z_parent(int val_)
  {
    DT_THROW_IF(val_ < 1 || val_ > 113,
                std::logic_error, "Invalid parent nucleus' Z !");
    _Z_parent_ = val_;
    return;
  }

  int beta_decay::get_Z_parent() const
  {
    return _Z_parent_;
  }

  void beta_decay::set_Z_daughter(int val_)
  {
    DT_THROW_IF(val_ < 1 || val_ > 113,
                std::logic_error, "Invalid daughter nucleus' Z !");
    _Z_daughter_ = val_;
    return;
  }

  int beta_decay::get_Z_daughter() const
  {
    return _Z_daughter_;
  }

  void beta_decay::set_mass_parent(double val_)
  {
    DT_THROW_IF(val_ <= 0.0,
                std::logic_error, "Invalid parent nucleus' mass !");
    _mass_parent_ = val_;
    return;
  }

  double beta_decay::get_mass_parent() const
  {
    return _mass_parent_;
  }

  void beta_decay::set_mass_daughter(double val_)
  {
    DT_THROW_IF(val_ <= 0.0,
                std::logic_error, "Invalid daughter nucleus' mass !");
    _mass_daughter_ = val_;
    return;
  }

  double beta_decay::get_mass_daughter() const
  {
    return _mass_daughter_;
  }

  void beta_decay::set_energy_parent(double val_)
  {
    DT_THROW_IF(val_ < 0.0,
                std::logic_error, "Invalid parent nucleus' excitation energy !");
    _energy_parent_ = val_;
    DT_LOG_DEBUG(get_logging(), "Energy parent set to : " << _energy_parent_ / CLHEP::keV << " keV");
    return;
  }

  double beta_decay::get_energy_parent() const
  {
    return _energy_parent_;
  }

  void beta_decay::set_energy_daughter(double val_)
  {
    DT_THROW_IF(val_ < 0.0,
                std::logic_error, "Invalid daughter nucleus' excitation energy !");
    _energy_daughter_ = val_;
    DT_LOG_DEBUG(get_logging(), "Energy daughter set to : " << _energy_daughter_ / CLHEP::keV << " keV");
    return;
  }

  double beta_decay::get_energy_daughter() const
  {
    return _energy_daughter_;
  }

  void beta_decay::set_coupling(coupling_type coupling_)
  {
    _coupling_ = coupling_;
    if (_coupling_ == COUPLING_UNKNOWN) {
      datatools::invalidate(_a_);
    } else if (_coupling_ != COUPLING_USER) {
      set_a(a_value(_coupling_));
    }
    return;
  }

  beta_decay::coupling_type beta_decay::get_coupling() const
  {
    return _coupling_;
  }

  bool beta_decay::coupling_is_known() const
  {
    return _coupling_ != COUPLING_UNKNOWN && _coupling_ != COUPLING_INVALID;
  }

  void beta_decay::set_a(double val_)
  {
    DT_THROW_IF(datatools::is_valid(val_) && std::abs(val_) > 1.0,
                std::range_error,
                "Invalid range for the a coefficient (|" << _a_ << "| > 1) !");
    _a_ = val_;
    return;
  }

  double beta_decay::get_a() const
  {
    return _a_;
  }

  void beta_decay::set_ke_cut(double val_)
  {
    DT_THROW_IF(val_ < 0.0, std::range_error, "Invalid cut on beta kinetic energy !");

    _ke_cut_ = val_;
    return;
  }

  double beta_decay::get_ke_cut() const
  {
    return _ke_cut_;
  }

  bool beta_decay::is_electron_shakeoff() const
  {
    return _electron_shakeoff_mode_ != ESO_NO_SHAKEOFF;
  }

  void beta_decay::set_electron_shakeoff(electron_shakeoff_mode_type eso_mode_)
  {
    _electron_shakeoff_mode_ = eso_mode_;
    return;
  }

  bool beta_decay::has_electron_shakeoff_fixed_charge() const
  {
    return _electron_shakeoff_fixed_charge_ != ESO_INVALID_FIXED_CHARGE;
  }

  int beta_decay::get_electron_shakeoff_fixed_charge() const
  {
    return _electron_shakeoff_fixed_charge_;
  }

  void beta_decay::set_electron_shakeoff_fixed_charge(int charge_)
  {
    DT_THROW_IF(_electron_shakeoff_mode_ == ESO_RANDOM_CHARGE,
                std::logic_error,
                "Invalid electron shakeoff mode (random)!");
    if (_electron_shakeoff_mode_ == ESO_NO_SHAKEOFF) {
      _electron_shakeoff_mode_ = ESO_FIXED_CHARGE;
    }
    _electron_shakeoff_fixed_charge_ = charge_;
    return;
  }

  void beta_decay::add_electron_shakeoff_random(int charge_, double probability_)
  {
    DT_THROW_IF(probability_ < 0.0, std::range_error,
                "Invalid electron shakeoff charge state probability!");
    electron_shakeoff_entry esoe;
    esoe.charge = charge_;
    esoe.probability = probability_;
    esoe.cumul_probability = datatools::invalid_real();
    _electron_shakeoff_data_random_.push_back(esoe);
    return;
  }

  const beta_decay::electron_shakeoff_data_type &
  beta_decay::get_electron_shakeoff_data_random() const
  {
    return _electron_shakeoff_data_random_;
  }

  double beta_decay::get_pr_max() const
  {
    return _pr_max_;
  }

  double beta_decay::get_probability_max() const
  {
    return _probability_max_;
  }

  void beta_decay::_compute_pr_max()
  {
    double M2  = gsl_pow_2(_M_);
    double m12 = _m1_;
    _pr_max_ = 0.5 * std::sqrt( ( M2 - gsl_pow_2(m12 + _m3_) )
                                * ( M2 - gsl_pow_2(m12 - _m3_) ) ) / _M_;
    //std::cerr << "DEVEL: beta_decay::_compute_pr_max: pr_max=" << _pr_max_ / CLHEP::keV << " keV" << std::endl;
    //double W = _q_beta_ + _m1_;
    //double ke_max = ((W - _m1_) * (W - _m1_) + 2. * _m3_ * (W - _m1_)) / (2. * (W + _m3_));
    // std::cerr << "DEVEL: beta_decay::_compute_pr_max: ke_max=" << ke_max / CLHEP::keV << " keV" << std::endl;
    //_pr_max_ = std::sqrt((W - ke_max - _m1_) * (W - ke_max - _m1_) + 2. * _m3_ * (W - ke_max - _m1_));
    // std::cerr << "DEVEL: beta_decay::_compute_pr_max: pr_max=" << _pr_max_ / CLHEP::keV << " keV" << std::endl;
    _kr_max_ = std::sqrt(gsl_pow_2(_m3_) + gsl_pow_2(_pr_max_)) - _m3_;
    //std::cerr << "DEVEL: beta_decay::_compute_pr_max: kr_max=" << _kr_max_ / CLHEP::keV << " keV" << std::endl;

    return;
  }


  double beta_decay::pdf_ke_pr( double ke_ , double pr_ ) const
  {
    double density_val = 0.0;
    double q_beta = _q_beta_;
    double ke_min, ke_max;
    beta_decay::compute_ke_limits(q_beta, pr_, ke_min, ke_max);
    if (ke_min < _ke_cut_) {
      ke_min = _ke_cut_;
    }
    double a_eff = _a_;
    if (_coupling_ == COUPLING_UNKNOWN) {
      a_eff = 0.0;
    }
    if ( ke_ >= ke_min && ke_ <= ke_max ) {
      double pnu    = q_beta - ke_;
      kinematics beta_kine;
      double me     = _m1_;
      beta_kine.initialize_from_m_k(me, ke_);
      double pe     = beta_kine.p(); // std::sqrt(ke_ * (ke_ + 2. * me));
      double We     = beta_kine.w(); // ke_ + me;
      density_val =
         _F_->operator()(ke_)
        * We * pr_ * pnu
        * (1.0 + a_eff * (gsl_pow_2(pr_) - gsl_pow_2(pnu) - gsl_pow_2(pe)) / (2 * We * pnu));
    }
    return density_val;
  }

  // static
  void beta_decay::compute_ke_limits(//double mass_parent_,
                                     double q_beta_,
                                     double pr_,
                                     double & ke_min_,
                                     double & ke_max_)
  {
    //double M      = mass_parent_;
    double me  = CLHEP::electron_mass_c2;
    double me2 = gsl_pow_2(me);
    double W   = q_beta_ + me;
    ke_max_ = 0.5 * (gsl_pow_2(W + pr_) + me2) / (W + pr_) - me;
    ke_min_ = 0.5 * (gsl_pow_2(W - pr_) + me2) / (W - pr_) - me;
    return;
  }

  void beta_decay::display_pdf(const std::string & title_) const
  {
    double q_beta = _q_beta_;
    double pr_min = 0.0;
    double pr_max = _pr_max_;
    int npr = 500;
    int nke = 500;
    double dpr = (pr_max - pr_min) / npr;
    double dke_base = q_beta / nke;
    double dke = dke_base;

    datatools::temp_file tmp_file;
    tmp_file.set_remove_at_destroy(true);
    tmp_file.create ("/tmp", "_genbb_beta_decay_pdf_");
    tmp_file.out().precision(15);

    for (double ipr = 0.0;
         ipr < pr_max + 0.1 * dpr;
         ipr += dpr ) {
      double pr = ipr;
      if (std::abs(pr - pr_max) < 0.1 * dpr) {
        pr = pr_max;
      }
      //std::cerr << "beta_decay::_compute_limits: pr=" << pr << std::endl;
      double ke_min;
      double ke_max;
      compute_ke_limits(q_beta, pr, ke_min, ke_max);
      if (ke_min < _ke_cut_) {
        ke_min = _ke_cut_;
      }
      for (double ike = ke_min;
           ike < ke_max + 0.1 * dke;
           ike += dke ) {
        double ke = ike;
        if (std::abs(ke - ke_max) < 0.1 * dke) {
          ke = ke_max;
        }
        //std::cerr << "beta_decay::_compute_limits: ke=" << ke << std::endl;
        double prob = pdf_ke_pr(ke, pr);
        tmp_file.out() << pr / CLHEP::MeV << ' ' << ke / CLHEP::MeV << ' ' << prob << std::endl;

        if (ke == ke_max) {
          break;
        }
      } // ike
      if (_log_file_) {
        *_log_file_ << pr / CLHEP::MeV << ' ' << ke_min / CLHEP::MeV << ' ' << ke_max / CLHEP::MeV << std::endl;
      }
      if (pr == _pr_max_) {
        break;
      }
      if (pr > 0.95 * _pr_max_) {
        dpr = 0.1 * (pr_max - pr_min) / npr;
      }
    } // ipr

    Gnuplot g1("line");
    std::string title = title_;
    if (title.empty()) {
      title = "genbb::beta_decay PDF";
    }
    std::ostringstream tss;
    tss << "set title '" << title << "'";
    g1.cmd(tss.str());
    g1.cmd("set grid");
    g1.cmd("set key out");
    g1.cmd("set xlabel 'pr (MeV)'");
    g1.cmd("set ylabel 'ke (MeV)'");
    g1.cmd("set zlabel 'PDF'");
    g1.cmd("set size ratio -1");
    g1.cmd("set xyplane at 0");

    {
      std::ostringstream plot_cmd;
      plot_cmd << "splot '" << tmp_file.get_filename() << "' index 0 u 1:2:3 title '' with dots ";
      g1.cmd (plot_cmd.str ());
      g1.showonscreen (); // window output
      geomtools::gnuplot_drawer::wait_for_key ();
      usleep (200);
    }

    return;
  }

  void beta_decay::_compute_limits()
  {
    _probability_max_ = -1.;
    double q_beta = _q_beta_;
    double dProb = 0;
    double pr_min = 0.0;
    double pr_max = _pr_max_;
    int npr = 500;
    int nke = 500;
    double dpr = (pr_max - pr_min) / npr;
    double dke_base = q_beta / nke;
    double dke = dke_base;
    if (_log_file_) {
      *_log_file_ << "#@type=" << _type_ << std::endl;
      *_log_file_ << "#@q_beta=" << _q_beta_ << std::endl;
      *_log_file_ << "#@A=" << _A_ << std::endl;
      *_log_file_ << "#@Z_parent=" << _Z_parent_ << std::endl;
      *_log_file_ << "#@Z_daughter=" << _Z_daughter_ << std::endl;
      *_log_file_ << "#@coupling=" << _coupling_ << std::endl;
      *_log_file_ << "#@a=" << _a_ << std::endl;
      *_log_file_ << "#@pr_min=" << pr_min << std::endl;
      *_log_file_ << "#@pr_max=" << pr_max << std::endl;
    }
    double pr_prob_max = -1;
    double ke_prob_max = -1;
    for (double ipr = 0.0;
         ipr < pr_max + 0.1 * dpr;
         ipr += dpr ) {
      double pr = ipr;
      if (std::abs(pr - pr_max) < 0.1 * dpr) {
        pr = pr_max;
      }
      //std::cerr << "beta_decay::_compute_limits: pr=" << pr << std::endl;
      double ke_min;
      double ke_max;
      compute_ke_limits(q_beta, pr, ke_min, ke_max);
      if (ke_min < _ke_cut_) {
        ke_min = _ke_cut_;
      }
      for (double ike = ke_min;
           ike < ke_max + 0.1 * dke;
           ike += dke ) {
        double ke = ike;
        if (std::abs(ke - ke_max) < 0.1 * dke) {
          ke = ke_max;
        }
        //std::cerr << "beta_decay::_compute_limits: ke=" << ke << std::endl;
        double prob = pdf_ke_pr(ke, pr);
        if ( prob > _probability_max_ || _probability_max_ < 0.0) {
          if ( _probability_max_ > 0.0 ) {
            dProb = prob - _probability_max_;
          }
          _probability_max_ = prob;
          pr_prob_max = pr;
          ke_prob_max = ke;
        }
        if (ke == ke_max) {
          break;
        }
      } // ike
      if (_log_file_) {
        *_log_file_ << pr << ' ' << ke_min << ' ' << ke_max << std::endl;
      }
      if (pr == _pr_max_) {
        break;
      }
    } // ipr
    _probability_max_ += dProb;
    if (_log_file_) {
     *_log_file_ << std::endl << std::endl;
     *_log_file_ << pr_prob_max << ' ' << ke_prob_max << ' ' << _probability_max_ << std::endl;
     *_log_file_ << std::endl << std::endl;
    }
    return;
  }

  void beta_decay::tree_dump(std::ostream& out_,
                             const std::string& title_,
                             const std::string& indent_,
                             bool inherit_) const
  {
    this->base_decay_driver::tree_dump(out_, title_, indent_, true);

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Decay type     : '" << label_from_decay_type(_type_) << "'" << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "A              : " << _A_ << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Z(parent)      : " << _Z_parent_ << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Z(daughter)    : " << _Z_daughter_ << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Mass(parent)   : " << _mass_parent_ / CLHEP::MeV << " MeV" << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Mass(daughter) : " << _mass_daughter_ / CLHEP::MeV << " MeV" << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "delta(Mass)    : " << (_mass_parent_ - _mass_daughter_) / CLHEP::keV << " keV" << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Q(beta)        : " << _q_beta_ / CLHEP::keV << " keV" << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Weak coupling  : '" << label_from_coupling(_coupling_) << "'" << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "a              : " << _a_ << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Ke(cut)        : " << _ke_cut_ / CLHEP::keV << " keV" << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Pr(max)        : " << _pr_max_ / CLHEP::keV << " keV" << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Prob(max)      : " << _probability_max_ << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Beta generated     : " << _beta_generated_ << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Neutrino generated : " << _neutrino_generated_ << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
         << "Daughter generated : " << _daughter_generated_ << std::endl;

    return;
  }

  int beta_decay::fire_event_kinematics(mygsl::rng & prng_,
                                        double & ke_,
                                        double & pr_,
                                        double & pnu_,
                                        double & cer_,
                                        double & cenu_) const
  {
    datatools::invalidate(ke_);
    datatools::invalidate(pr_);
    datatools::invalidate(pnu_);
    datatools::invalidate(cer_);
    datatools::invalidate(cenu_);
    // Von Neumann rejection method:
    int count(0);
    bool count_alert = false;
    while (true) {
      double pr = prng_.flat(0.0, _pr_max_);
      double ke = prng_.flat(0.0, _q_beta_);
      double prob_ke_pr = pdf_ke_pr(ke, pr);
      double prob = prng_.flat(0.0, _probability_max_);
      // if (pr < 1.0 * CLHEP::keV) {
      //   std::cerr << "DEVEL: pr = " << pr / CLHEP::keV << " keV" << std::endl;
      //   std::cerr << "DEVEL: ->  ke         = " << ke / CLHEP::keV << " keV" << std::endl;
      //   std::cerr << "DEVEL: ->  prob       = " << prob << std::endl;
      //   std::cerr << "DEVEL: ->  prob_ke_pr = " << prob_ke_pr << std::endl;
      //   std::cerr << "DEVEL: ->  prob_max   = " << _probability_max_ << std::endl;
      // }
      if (prob < prob_ke_pr) {
        ke_ = ke;
        pr_ = pr;
        // if (pr_ < 1.0 * CLHEP::keV) {
        //   std::cerr << "DEVEL: ===> pr_ = " << pr_ / CLHEP::keV << " keV" << std::endl;
        //   std::cerr << "DEVEL: ===> ke_ = " << ke_ / CLHEP::keV << " keV" << std::endl;
        // }
        break;
      }
      count++;
      if (count > 1000) {
        DT_LOG_WARNING(datatools::logger::PRIO_WARNING,
                       "Could not determine the kinematics after " << count << " tries !");
        count_alert = true;
        break;
      }
    }
    if (count_alert) {
      return 1;
    }
    double pr2 = gsl_pow_2(pr_);
    // Energy/momentum of the massless (anti-)neutrino
    // double pnub = _q_beta_ - ke_ - (0.5 * pr2 / _m3_);
    kinematics r_kine;
    r_kine.initialize_from_m_p(_m3_, pr_);
    pnu_ = _q_beta_ - ke_ - r_kine.k();
    std::cerr.precision(15);
    // std::cerr << "DEVEL: ===> pnub = " << pnub / CLHEP::keV << " keV" << std::endl;
    // std::cerr << "DEVEL: ===> pnu_ = " << pnu_ / CLHEP::keV << " keV" << std::endl;
    double pnu2 = gsl_pow_2(pnu_);
    double pe2 = ke_ * (ke_ + 2. * _m1_);
    double pe = ::sqrt(pe2);

    cer_  = 0.5 * (pnu2 - pe2 - pr2 ) / (pe * pr_);
    // if (std::abs(cer_) > 1.0) {
    //   std::cerr << "DEVEL: ===> cer_ = " << cer_ << std::endl;
    //   std::cerr << "DEVEL:   =>   pnu_ = " << pnu_ / CLHEP::keV << " keV" << std::endl;
    //   std::cerr << "DEVEL:   =>   pr_  = " << pr_ / CLHEP::keV << " keV" << std::endl;
    //   std::cerr << "DEVEL:   =>   ke_  = " << ke_ / CLHEP::keV << " keV" << std::endl;
    //   std::cerr << "DEVEL:   =>   pe   = " << pe / CLHEP::keV << " keV" << std::endl;
    // }
    /*
Possible problem:
DEVEL: beta_decay::fill: r=(2615.63196822018,0,-487.278143560805) keV
DEVEL: ===> cer_ = -1.00007352666532
DEVEL:   =>   pnu_ = 1114.02259466283 keV
DEVEL:   =>   pr_  = 3970.19687885276 keV
DEVEL:   =>   ke_  = 2389.79051414769 keV
DEVEL:   =>   pe   = 2855.42630744096 keV
DEVEL: beta_decay::fill: ===> cos_er = -1.00007352666532
DEVEL: beta_decay::fill: r=(-nan,0,-3970.48879418991) keV
DEVEL: test1: pr = -nan keV
DEVEL: test1: ke = 2389.79051414769 keV
    */
    if (cer_ < -1.0) cer_ = -1.0;
    if (cer_ > +1.0) cer_ = +1.0;
    cenu_ = 0.5 * (pr2  - pe2 - pnu2) / (pe * pnu_);
    if (cenu_ < -1.0) cenu_ = -1.0;
    if (cenu_ > +1.0) cenu_ = +1.0;

    return 0;
  }

  int beta_decay::fill(mygsl::rng & prng_, genbb::primary_event & event_)
  {
    // Fire kinematics:
    double ke, pr, pnu, cos_er, cos_enu;
    int err = this->fire_event_kinematics(prng_, ke, pr, pnu, cos_er, cos_enu);
    if (err != 0) {
      return err;
    }
    // if (!datatools::is_valid(pr)) {
    //   std::cerr << "DEVEL: beta_decay::fill: ===> pr = " << pr / CLHEP::keV << " keV" << std::endl;
    //   std::cerr << "DEVEL: beta_decay::fill:   => ke = " << ke / CLHEP::keV << " keV" << std::endl;
    // }
    // if (pr < 1.0 * CLHEP::keV) {
    //   std::cerr << "DEVEL: beta_decay::fill: ===> pr = " << pr / CLHEP::keV << " keV" << std::endl;
    //   std::cerr << "DEVEL: beta_decay::fill:   => ke = " << ke / CLHEP::keV << " keV" << std::endl;
    // }
    // if (std::abs(cos_er) > 1.0) {
    //   std::cerr << "DEVEL: beta_decay::fill: ===> cos_er = " << cos_er << std::endl;
    // }
    double sin_er  =  std::sqrt(1.0 - cos_er  * cos_er);
    double sin_enu = -std::sqrt(1.0 - cos_enu * cos_enu);

    // Randomize the beta direction:
    double cost  =  prng_.flat(-1.0, +1.0);
    double sint  = std::sqrt(1 - cost * cost);
    double theta = std::atan2(sint, cost);
    double phi   = prng_.flat(0.0, 2 * M_PI);
    double delta = prng_.flat(0.0, 2 * M_PI);
    geomtools::rotation_3d rot;
    geomtools::create_zyz(rot, phi, theta, delta);
    rot.invert();

    if (is_beta_generated()) {
      // Beta:
      primary_particle & beta = event_.add_particle();
      if (is_beta_minus()) {
        beta.set_type(primary_particle::ELECTRON);
      } else {
        beta.set_type(primary_particle::POSITRON);
      }
      beta.set_time(0.0);
      //beta.grab_auxiliaries().store("DPId", "genbb::beta_decay@beta");
      kinematics beta_kine;
      beta_kine.initialize_from_m_k(beta.get_mass(), ke);
      geomtools::vector_3d beta_momentum(0.0, 0.0, beta_kine.p());
      beta.grab_momentum() = rot * beta_momentum;
    }

    if (is_neutrino_generated()) {
      // Neutrino:
      primary_particle & neutrino = event_.add_particle();
      neutrino.set_neutrino("electron", is_beta_minus() /* antineutrino */);
      neutrino.set_time(0.0);
      //neutrino.grab_auxiliaries().store("DPId", "genbb::beta_decay@neutrino");
      kinematics neutrino_kine;
      neutrino_kine.initialize_from_m_p(neutrino.get_mass(), pnu);
      geomtools::vector_3d neutrino_momentum(pnu * sin_enu, 0.0, pnu * cos_enu);
      neutrino.grab_momentum() = rot * neutrino_momentum;
    }

    if (is_daughter_generated()) {
      // Recoil ion:
      primary_particle & recoil = event_.add_particle();
      // For neutral parent atom and beta minus decay, expected recoil ion charge is typically +1...
      int ion_charge = 0; // default: neutral atom (not realistic) !!!
      if (is_electron_shakeoff()) {
        // Apply electron shakeoff:
        if (_electron_shakeoff_mode_ == ESO_FIXED_CHARGE) {
          ion_charge = get_electron_shakeoff_fixed_charge();
        } else if (_electron_shakeoff_mode_ == ESO_RANDOM_CHARGE) {
          double prob = prng_.flat(0, +1.0);
          for (int i = 0; i < (int) _electron_shakeoff_data_random_.size(); i++) {
            if (prob < _electron_shakeoff_data_random_[i].cumul_probability) {
              ion_charge = _electron_shakeoff_data_random_[i].charge;
              break;
            }
          }
        }
      }
      recoil.set_ion(_Z_daughter_,
                     _A_,
                     _energy_daughter_,
                     ion_charge);
      recoil.set_time(0.0);
      double recoil_mass = _m3_ + (_Z_daughter_ - ion_charge) * CLHEP::electron_mass_c2;
      recoil.set_mass(recoil_mass);
      //recoil.grab_auxiliaries().store("DPId", "genbb::beta_decay@recoil");
      kinematics recoil_kine;
      recoil_kine.initialize_from_m_p(recoil_mass, pr);
      geomtools::vector_3d recoil_momentum(pr * sin_er, 0.0, pr * cos_er);
      // std::cerr << "DEVEL: beta_decay::fill: r="
      //           << recoil_momentum / CLHEP::keV << " keV"
      //           << std::endl;
      recoil.grab_momentum() = rot * recoil_momentum;
    }

    return 0;
  }

} // end of namespace genbb
