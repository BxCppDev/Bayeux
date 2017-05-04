// nuclear_level.cc
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
#include <genbb_help/nuclear_level.h>

// Standard library:
#include <limits>
#include <cmath>

// Third party:
// - CLHEP:
#include<CLHEP/Units/PhysicalConstants.h>
#include<CLHEP/Units/SystemOfUnits.h>
// - Bayeux/datatools:
#include <datatools/logger.h>
#include <datatools/exception.h>
#include <datatools/utils.h>
// - Bayeux/materials:
#include <materials/chemical_symbol.h>

// This project:
#include <genbb_help/nuclear_decay.h>

namespace genbb {

  // static
  std::string nuclear_level::label_from_parity(parity_type parity_)
  {
    switch(parity_) {
    case PARITY_MINUS:
      return "-";
    case PARITY_PLUS:
      return "+";
    case PARITY_UNKNOWN:
      return "?";
    default:
      return "";
    }
  }

  // static
  nuclear_level::parity_type
  nuclear_level::parity_from_label(const std::string & label_)
  {
    if (label_ == "+") {
      return PARITY_PLUS;
    } else if (label_ == "-") {
      return PARITY_MINUS;
    } else if (label_ == "?") {
      return PARITY_UNKNOWN;
    } else return PARITY_INVALID;
  }

  // static
  std::string nuclear_level::label_from_spin(double spin_)
  {
    if (spin_ == 0.0) {
      return "0";
    }
    double s2 = std::floor(2 * spin_ + 0.1);
    double sgn = std::pow(-1.0, s2);
    int i2 = (int) s2;
    if (sgn > 0) {
      std::stringstream oss;
      oss << i2/2;
      return oss.str();
    } else {
      std::stringstream oss;
      oss << i2 << "/2";
      return oss.str();
    }
  }

  // static
  double nuclear_level::spin_from_label(const std::string & label_)
  {
    double J = std::numeric_limits<double>::quiet_NaN();
    std::istringstream iss(label_);
    std::string word;
    iss >> word;
    if (word.empty()) {
      return J;
    }
    int n, d(1);
    std::istringstream iss2(word);
    iss2 >> n;
    if (iss2) {
      if (n < 0) {
        return J;
      }
      char c = 0;
      iss2 >> c;
      if (!iss2) {
        J = n;
        return J;
      } else {
        if (c == '/') {
          iss2 >> d;
          if (iss2) {
            if (d == 2) {
              J = n / 2;
              return J;
            }
          }
        }
      }
    }
    return J;
  }

  nuclear_level::decay_channel::decay_channel()
  {
    _decay_ = 0;
    datatools::invalidate(_branching_ratio_);
    datatools::invalidate(_cumul_prob_);
    return;
  }

  nuclear_level::decay_channel::~decay_channel()
  {
    return;
  }

  bool nuclear_level::decay_channel::is_valid() const
  {
    return _decay_ != 0
      && datatools::is_valid(_branching_ratio_)
      && datatools::is_valid(_cumul_prob_);
  }

  void nuclear_level::decay_channel::set_decay(const nuclear_decay & decay_)
  {
    DT_THROW_IF(!decay_.is_valid(), std::logic_error, "Decay is not valid !");
    _decay_ = &decay_;
    return;
  }

  void nuclear_level::decay_channel::set_branching_ratio(double br_)
  {
    DT_THROW_IF(br_ < 0.0, std::logic_error, "Negative branching ratio is not valid !");
    _branching_ratio_ = br_;
    return;
  }

  void nuclear_level::decay_channel::set_cumul_prob(double cbr_)
  {
    DT_THROW_IF(cbr_ < 0.0, std::logic_error, "Invalid cumulative branching ratio !");
    DT_THROW_IF(cbr_ > 1.0, std::logic_error, "Invalid cumulative branching ratio !");
    _cumul_prob_ = cbr_;
    return;
  }

  void nuclear_level::decay_channel::initialize(const nuclear_decay & decay_,
                                                double br_,
                                                double cbr_)
  {
    set_decay(decay_);
    set_branching_ratio(br_);
    set_cumul_prob(cbr_);
    return;
  }

  void nuclear_level::decay_channel::reset()
  {
    _branching_ratio_ = 0.0;
    _cumul_prob_ = 1.0;
    _decay_ = 0;
    return;
  }

  const nuclear_decay & nuclear_level::decay_channel::get_decay() const
  {
    DT_THROW_IF(!_decay_, std::logic_error, "Missing decay !");
    return *_decay_;
  }

  double nuclear_level::decay_channel::get_branching_ratio() const
  {
    return _branching_ratio_;
  }

  double nuclear_level::decay_channel::get_cumul_prob() const
  {
    return _cumul_prob_;
  }

  nuclear_level::nuclear_level()
  {
    _initialized_ = false;
    _set_defaults();
    return;
  }

  nuclear_level::~nuclear_level()
  {
    if (is_initialized()) {
      reset();
    }
    return;
  }

  void nuclear_level::initialize_simple()
  {
    datatools::properties dummy;
    initialize(dummy);
    return;
  }

  bool nuclear_level::is_initialized() const
  {
    return _initialized_;
  }

  void nuclear_level::initialize(const datatools::properties & config_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error,
                "Nuclear level '" << to_string() << "' is already initialized !");
    if (_Z_ < 0) {
      if (config_.has_key("Z")) {
        int Z = config_.fetch_integer("Z");
        set_Z(Z);
      }
    }
    if (_A_ == 0) {
      if (config_.has_key("A")) {
        int A = config_.fetch_integer("A");
        set_A(A);
      }
    }
    if (! datatools::is_valid(_spin_)) {
      if (config_.has_key("spin")) {
        const std::string & spin_label = config_.fetch_string("spin");
        double spin = spin_from_label(spin_label);
        set_spin(spin);
      }
    }
    if (_parity_ == PARITY_INVALID) {
      if (config_.has_key("parity")) {
        const std::string & parity_label = config_.fetch_string("parity");
        parity_type parity = parity_from_label(parity_label);
        set_parity(parity);
      }
    }

    double default_time_unit = CLHEP::second;
    double default_energy_unit = CLHEP::keV;

    if (! datatools::is_valid(_half_life_)) {
      if (config_.has_flag("stable")) {
        set_stable();
      } else if (config_.has_key("half_life")) {
        double half_life = config_.fetch_real("half_life");
        // std::cerr << "DEVEL: half_life=" << half_life << std::endl;
        if (! config_.has_explicit_unit("half_life")) {
          // std::cerr << "DEVEL: no explicit unit !" << std::endl;
          half_life *= default_time_unit;
        }
        set_half_life(half_life);
      } else if (config_.has_key("width")) {
        double width = config_.fetch_real("width");
        if (! config_.has_explicit_unit("width")) {
          width *= default_energy_unit;
        }
        set_width(width);
      }
    }

    if (! datatools::is_valid(_energy_)) {
      if (config_.has_flag("fundamental")) {
        set_fundamental();
      } else if (config_.has_key("energy")) {
        double energy = config_.fetch_real("energy");
        if (! config_.has_explicit_unit("energy")) {
          energy *= default_energy_unit;
        }
        set_energy(energy);
      }
    }

    // Store properties describing the decay table of the level:
    // config_.export_starting_with(_auxiliaries_, "decay_table.");
    config_.export_and_rename_starting_with(_decay_table_config_, "decay_table.", "");

    // Store other properties:
    config_.export_and_rename_starting_with(_auxiliaries_, "aux.", "");

    DT_THROW_IF (!is_valid(), std::logic_error,"Invalid nuclear level !");

    _initialized_ = true;
    return;
  }

  void nuclear_level::install_decay_table(const datatools::properties & config_,
                                          const std::map<std::string, datatools::handle<nuclear_decay> > & decays_)
  {
    DT_THROW_IF(! is_initialized(), std::logic_error,
                "Nuclear level '" << to_string()
                << "' should be initialized first before to setup its decay table!");
    DT_THROW_IF(is_stable(), std::logic_error,
                "Nuclear level '" << to_string()
                << "' is stable so cannot have a decay table!");
    double default_br_unit = 1.0;
    if (config_.has_key("branching_ratio_unit")) {
      const std::string & brunit_lbl = config_.fetch_string("branching_ratio_unit");
      if (brunit_lbl == "%" || brunit_lbl == "percent") {
        default_br_unit = CLHEP::perCent;
      } else {
        DT_THROW_IF(true, std::logic_error, "Invalid branching ratio unit '" << brunit_lbl << "' !");
      }
    }

    if (config_.has_key("channels")) {
      std::vector<std::string> dc_labels;
      config_.fetch("channels", dc_labels);
      for (int i = 0; i < (int) dc_labels.size(); i++) {
        const std::string & dc_label = dc_labels[i];
        std::ostringstream decay_key;
        decay_key << dc_label << ".decay";
        DT_THROW_IF(! config_.has_key(decay_key.str()),
                    std::logic_error,
                    "Missing decay key '" << decay_key.str() << "' !");
        const std::string & decay_name = config_.fetch_string(decay_key.str());
        std::map<std::string, decay_handle_type>::const_iterator decay_found = decays_.find(decay_name);
        DT_THROW_IF(decay_found == decays_.end(), std::logic_error,
                    "Cannot find decay '" << decay_name << "' from dictionary of decays !");
        std::ostringstream br_key;
        br_key << dc_label << ".branching_ratio";
        DT_THROW_IF(! config_.has_key(br_key.str()),
                    std::logic_error,
                    "Missing branching ratio key '" << br_key.str() << "' !");
        double br = config_.fetch_real(br_key.str());
        if (! config_.has_explicit_unit(br_key.str())) {
          br *= default_br_unit;
        }
        add_decay_channel(decay_found->second.get(), br);
      }
    }

    return;
  }

  void nuclear_level::reset()
  {
    reset_decay_table();
    _initialized_ = false;
    _auxiliaries_.clear();
    _set_defaults();
    return;
  }

  void nuclear_level::reset_decay_table()
  {
    _decay_channels_.clear();
    return;
  }

  void nuclear_level::_set_defaults()
  {
    _A_ = 0;
    _Z_ = -1;
    datatools::invalidate(_spin_);
    _parity_ = PARITY_INVALID;
    datatools::invalidate(_half_life_);
    datatools::invalidate(_energy_);
    return;
  }

  bool nuclear_level::is_valid() const
  {
    if (_A_ <= 0) return false;
    if (_Z_ < 0) return false;
    if (!datatools::is_valid(_spin_)) return false;
    if (_parity_ == PARITY_INVALID) return false;
    if (!datatools::is_valid(_half_life_)) return false;
    if (!datatools::is_valid(_energy_)) return false;
    return true;
  }

  void nuclear_level::set_A(int val_)
  {
    DT_THROW_IF(val_ < 1, std::range_error, "Invalid negative or null A !");
    _A_ = val_;
    return;
  }

  void nuclear_level::set_Z(int val_)
  {
    DT_THROW_IF(val_ < 0, std::range_error, "Invalid negative Z !");
    _Z_ = val_;
    return;
  }

  void nuclear_level::set_spin(double val_)
  {
    DT_THROW_IF(val_ < 0, std::range_error, "Invalid negative spin value !");
    double val2 = 2 * val_ + std::numeric_limits<double>::epsilon();
    double chk = std::floor(val2);
    int i = (int) chk;
    DT_THROW_IF((val2 - i) > std::numeric_limits<double>::epsilon(),
                std::logic_error,
                "Invalid spin value '" << val_ << "' !");
    _spin_ = val_;
    return;
  }

  void nuclear_level::set_parity(parity_type val_)
  {
    _parity_ = val_;
    return;
  }

  void nuclear_level::set_stable()
  {
    set_half_life(std::numeric_limits<double>::infinity());
    return;
  }

  void nuclear_level::set_half_life(double val_)
  {
    DT_THROW_IF(val_ < 0.0, std::range_error, "Invalid negative half-life !");
    _half_life_ = val_;
    return;
  }

  void nuclear_level::set_width(double val_)
  {
    if (val_ == 0.0) {
      set_stable();
    } else {
      double T1_2 = M_LN2 * CLHEP::hbar_Planck / val_;
      set_half_life(T1_2);
    }
    return;
  }

  void nuclear_level::set_fundamental()
  {
    set_energy(0.0);
    return;
  }

  void nuclear_level::set_energy(double val_)
  {
    DT_THROW_IF(val_ < 0.0, std::range_error, "Invalid negative energy !");
    _energy_ = val_;
    return;
  }

  int nuclear_level::get_A() const
  {
    return _A_;
  }

  int nuclear_level::get_Z() const
  {
    return _Z_;
  }

  const std::string & nuclear_level::get_element_symbol() const
  {
    DT_THROW_IF(_Z_ < 0, std::logic_error, "Undefined Z value !");
    return materials::chemical_symbol::symbol_from_z((size_t) _Z_);
  }

  int nuclear_level::get_N() const
  {
    return _A_ - _Z_;
  }

  bool nuclear_level::spin_is_known() const
  {
    return datatools::is_valid(_spin_);
  }

  bool nuclear_level::parity_is_known() const
  {
    return _parity_ != PARITY_INVALID && _parity_ != PARITY_UNKNOWN;
  }

  double nuclear_level::get_spin() const
  {
    return _spin_;
  }

  nuclear_level::parity_type nuclear_level::get_parity() const
  {
    return _parity_;
  }

  double nuclear_level::get_half_life() const
  {
    return _half_life_;
  }

  double nuclear_level::get_mean_life_time() const
  {
    return _half_life_ / M_LN2;
  }

  double nuclear_level::get_width() const
  {
    return CLHEP::hbar_Planck / get_mean_life_time();
  }

  bool nuclear_level::is_stable() const
  {
    return _half_life_ == std::numeric_limits<double>::infinity();
  }

  double nuclear_level::get_energy() const
  {
    return _energy_;
  }

  bool nuclear_level::is_fundamental() const
  {
    return _energy_ == 0.0;
  }

  bool nuclear_level::has_decay_table() const
  {
    return !is_stable() && _decay_channels_.size() > 0;
  }

  void nuclear_level::add_decay_channel(const nuclear_decay & decay_, double branching_ratio_)
  {
    {
      decay_channel dummy;
      _decay_channels_.push_back(dummy);
    }
    decay_channel & new_channel = _decay_channels_.back();
    new_channel.set_decay(decay_);
    new_channel.set_branching_ratio(branching_ratio_);
    _compute_decay_weights();
    return;
  }

  size_t nuclear_level::get_number_of_decay_channels() const
  {
    return _decay_channels_.size();
  }

  const nuclear_level::decay_channel &
  nuclear_level::get_decay_channel(int i_) const
  {
    DT_THROW_IF(i_ < 0 || i_ >= (int) _decay_channels_.size(),
                std::range_error,
                "Invalid decay channel index !");
    return _decay_channels_[i_];
  }

  const nuclear_level::decay_channels_col_type & nuclear_level::get_decay_channels() const
  {
    return _decay_channels_;
  }

  const datatools::properties & nuclear_level::get_auxiliaries() const
  {
    return _auxiliaries_;
  }

  datatools::properties & nuclear_level::grab_auxiliaries()
  {
    return _auxiliaries_;
  }

  const datatools::properties & nuclear_level::get_decay_table_config() const
  {
    return _decay_table_config_;
  }

  datatools::properties & nuclear_level::grab_decay_table_config()
  {
    return _decay_table_config_;
  }

  void nuclear_level::_compute_decay_weights()
  {
    double wsum = 0.0;
    std::vector<double> weights;
    weights.assign(_decay_channels_.size(), 0.0);
    for (int i = 0; i < (int) _decay_channels_.size(); i++) {
      decay_channel & channel = _decay_channels_[i];
      wsum += channel.get_branching_ratio();
      // Record the non-normalized cumulative probability:
      weights[i] = wsum;
    }
    for (int i = 0; i < (int) _decay_channels_.size(); i++) {
      decay_channel & channel = _decay_channels_[i];
      // Now we store the normalized probability:
      channel.set_cumul_prob(weights[i] / wsum);
    }
    return;
  }

  std::string nuclear_level::to_string(unsigned int) const
  {
    std::ostringstream oss;
    if (_Z_ > 0) {
      oss << get_element_symbol() << '-' << _A_;
      oss << '(';
      if (_energy_ < 10 * CLHEP::keV) {
        oss.precision(2);
      } else if (_energy_ < 100 * CLHEP::keV) {
        oss.precision(3);
      } else if (_energy_ < 1000 * CLHEP::keV) {
        oss.precision(4);
      } else if (_energy_ < 10000 * CLHEP::keV) {
        oss.precision(5);
      } else if (_energy_ < 100000 * CLHEP::keV) {
        oss.precision(6);
      }
      oss << _energy_ / CLHEP::keV;
      oss << ',';
      oss << label_from_spin(_spin_);
      oss << label_from_parity(_parity_);
      oss << ')';
    } else {
      oss << "n";
    }
    return oss.str();
  }

  void nuclear_level::tree_dump(std::ostream & out_,
                                const std::string & title_,
                                const std::string & indent_,
                                bool inherit_) const
  {
    if (! title_.empty()) {
      out_ << indent_ << title_ << std::endl;
    }

    out_ << indent_ << i_tree_dumpable::tag
         << "Label     : '" << to_string() << "'" << std::endl;

    out_ << indent_ << i_tree_dumpable::tag
         << "A         : " << _A_ << std::endl;

    out_ << indent_ << i_tree_dumpable::tag
         << "Z         : " << _Z_;
    out_ << ' ' << '(' << get_element_symbol() << ')';
    out_ << std::endl;

    out_ << indent_ << i_tree_dumpable::tag
         << "J         : " << label_from_spin(_spin_) << std::endl;

    out_ << indent_ << i_tree_dumpable::tag
         << "Parity    : " << label_from_parity(_parity_) << std::endl;

    out_ << indent_ << i_tree_dumpable::tag
         << "Half-life : ";
    if (is_stable()) {
      out_ << "<stable>";
    } else {
      out_ << _half_life_ / CLHEP::second << " s";
    }
    out_ << std::endl;

    out_ << indent_ << i_tree_dumpable::tag
         << "Energy    : " << _energy_ / CLHEP::keV << " keV";
    if (is_fundamental()) {
      out_ << " <fundamental>";
    }
    out_ << std::endl;

    out_ << indent_ << i_tree_dumpable::tag
         << "Decay table configuration  : " << std::endl;
    {
      std::ostringstream indent_oss;
      indent_oss << indent_ << i_tree_dumpable::skip_tag;
      _decay_table_config_.tree_dump(out_, "", indent_oss.str());
    }

    out_ << indent_ << i_tree_dumpable::tag
         << "Decay channels : ";
    if (_decay_channels_.size()) {
      out_ << _decay_channels_.size();
    } else {
      out_ << "<none>";
    }
    out_ << std::endl;
    for (int i = 0; i < (int) _decay_channels_.size(); i++) {
      const decay_channel & dm = _decay_channels_[i];
      out_ << indent_ << i_tree_dumpable::skip_tag;
      if (i + 1 < (int) _decay_channels_.size()) {
        out_ << i_tree_dumpable::tag;
      } else {
        out_ << i_tree_dumpable::last_tag;
      }
      out_ << dm.get_decay().to_string();
      /*
        out_ << nuclear_decay::label_from_decay_mode(dm.get_decay().get_decay_mode());
      */
      out_ << " : ";
      out_.width(5);
      out_.precision(4);
      out_ << dm.get_branching_ratio() * 100 << " %";
      out_ << " [" << dm.get_cumul_prob() << "]";
      out_ << std::endl;
    }

    out_ << indent_ << i_tree_dumpable::tag
         << "Auxiliaries  : " << std::endl;
    {
      std::ostringstream indent_oss;
      indent_oss << indent_ << i_tree_dumpable::skip_tag;
      _auxiliaries_.tree_dump(out_, "", indent_oss.str());
    }

    out_ << indent_ << i_tree_dumpable::tag
         << "Valid     : " << is_valid() << std::endl;

    out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_)
         << "Initialized : " << is_initialized() << std::endl;

    return;
  }

} // end of namespace genbb
