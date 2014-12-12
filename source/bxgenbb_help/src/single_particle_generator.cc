// single_particle_generator.cc
/*
 * Copyright 2007-2014 F. Mauger
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
#include <genbb_help/single_particle_generator.h>

// Standard library:
#include <cstdlib>
#include <cmath>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <fstream>

// Third party:
// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/units.h>
#include <datatools/exception.h>
// - Bayeux/materials:
#include <materials/isotope.h>
// - Bayeux/datatools:
#include <geomtools/utils.h>

// This project:
#include <genbb_help/primary_event.h>
#include <genbb_help/primary_particle.h>

namespace genbb {

  GENBB_PG_REGISTRATION_IMPLEMENT(single_particle_generator, "genbb::single_particle_generator");

  bool single_particle_generator::can_external_random() const
  {
    return true;
  }

  void single_particle_generator::set_direction_mode(int dm_)
  {
    _direction_mode_ = dm_;
    return;
  }

  void single_particle_generator::set_randomized_direction(bool rd_)
  {
    DT_THROW_IF(_initialized_, std::logic_error, "Operation prohibited ! Object is locked/initialized !");
    if (rd_) {
      _direction_mode_ = DIRECTION_RANDOMIZED;
    } else {
      _direction_mode_ = DIRECTION_DEFAULT;
    }
    return;
  }

  bool single_particle_generator::is_randomized_direction() const
  {
    return _direction_mode_ == DIRECTION_RANDOMIZED;
  }

  void single_particle_generator::set_z_direction(bool zd_)
  {
    DT_THROW_IF(_initialized_, std::logic_error, "Operation prohibited ! Object is locked/initialized !");
    if (zd_) {
      _direction_mode_ = DIRECTION_Z_AXIS;
    } else {
      _direction_mode_ = DIRECTION_DEFAULT;
    }
    return;
  }

  bool single_particle_generator::is_z_direction() const
  {
    return _direction_mode_ == DIRECTION_Z_AXIS;
  }

  void single_particle_generator::set_cone_direction(bool rd_)
  {
    DT_THROW_IF(_initialized_, std::logic_error, "Operation prohibited ! Object is locked/initialized !");
    if (rd_) {
      _direction_mode_ = DIRECTION_CONE;
    } else {
      _direction_mode_ = DIRECTION_DEFAULT;
    }
    return;
  }

  bool single_particle_generator::is_cone_direction() const
  {
    return _direction_mode_ == DIRECTION_CONE;
  }

  void single_particle_generator::set_cone_max_angle(double ca_)
  {
    _cone_max_angle_ = ca_;
    return;
  }

  double single_particle_generator::get_cone_max_angle() const
  {
    return _cone_max_angle_;
  }

  void single_particle_generator::set_cone_min_angle(double ca_)
  {
    _cone_min_angle_ = ca_;
    return;
  }

  double single_particle_generator::get_cone_min_angle() const
  {
    return _cone_min_angle_;
  }

  void single_particle_generator::set_cone_axis(const geomtools::vector_3d & axis_)
  {
    DT_THROW_IF(axis_.mag() == 0.0, std::logic_error, "Null cone axis !");
    DT_THROW_IF(! geomtools::is_valid(axis_), std::logic_error, "Invalid cone axis !");
    _cone_axis_ = axis_.unit();
    return;
  }

  const geomtools::vector_3d & single_particle_generator::get_cone_axis() const
  {
    return _cone_axis_;
  }

  bool single_particle_generator::is_initialized() const
  {
    return _initialized_;
  }

  const std::string & single_particle_generator::get_particle_name() const
  {
    return _particle_name_;
  }

  void single_particle_generator::set_particle_name(const std::string & new_value_)
  {
    DT_THROW_IF(_initialized_, std::logic_error, "Operation prohibited ! Object is locked/initialized !");
    // DT_THROW_IF(! single_particle_generator::particle_name_is_valid(particle_name),
    //              logic_error,
    //              "Invalid particle name '" << particle_name << "' !");
    _particle_name_ = new_value_;
    return;
  }

  void single_particle_generator::set_particle_mass(double mass_)
  {
    DT_THROW_IF(_initialized_, std::logic_error, "Operation prohibited ! Object is locked/initialized !");
    _particle_mass_ = mass_;
    return;
  }

  double single_particle_generator::get_particle_mass() const
  {
    return _particle_mass_;
  }

  bool single_particle_generator::particle_name_is_valid(const std::string & particle_name_)
  {
    if (particle_name_ == "electron") return true;
    if (particle_name_ == "e-")       return true;
    if (particle_name_ == "positron") return true;
    if (particle_name_ == "e+")       return true;
    if (particle_name_ == "gamma")    return true;
    if (particle_name_ == "alpha")    return true;
    if (particle_name_ == "neutron")  return true;
    if (particle_name_ == "proton")   return true;
    if (particle_name_ == "mu-")      return true;
    if (particle_name_ == "mu+")      return true;
    return false;
  }

  double single_particle_generator::get_particle_mass_from_label(const std::string & particle_name_)
  {
    double mass;
    datatools::invalidate(mass);

    if (particle_name_ == "electron" ||
        particle_name_ == "positron" ||
        particle_name_ == "e+"       ||
        particle_name_ == "e-") {
      mass = CLHEP::electron_mass_c2;
    }

    if (particle_name_ == "gamma") {
      mass = 0.0 * CLHEP::eV;
    }

    if (particle_name_ == "alpha") {
      mass = 3.727417 * CLHEP::GeV;
    }

    if (particle_name_ == "neutron") {
      mass = CLHEP::neutron_mass_c2;
    }

    if (particle_name_ == "proton") {
      mass = CLHEP::proton_mass_c2;
    }

    if (particle_name_ == "mu-" ||
        particle_name_ == "mu+") {
      mass = 105.658369 * CLHEP::MeV;
    }

    return mass;
  }

  int single_particle_generator::get_mode() const
  {
    return _mode_;
  }

  void single_particle_generator::set_mode(int new_value_)
  {
    DT_THROW_IF(_initialized_, std::logic_error, "Operation prohibited ! Object is locked/initialized !");
    _mode_ = new_value_;
    return;
  }

  bool single_particle_generator::is_mode_valid() const
  {
    return _mode_ != MODE_INVALID;
  }

  bool single_particle_generator::is_mode_monokinetic() const
  {
    return _mode_ == MODE_MONOKINETIC;
  }

  bool single_particle_generator::is_mode_gaussian_energy() const
  {
    return _mode_ == MODE_GAUSSIAN_ENERGY;
  }

  bool single_particle_generator::is_mode_energy_range() const
  {
    return _mode_ == MODE_ENERGY_RANGE;
  }

  bool single_particle_generator::is_mode_multi_rays() const
  {
    return _mode_ == MODE_MULTI_RAYS;
  }

  bool single_particle_generator::is_mode_spectrum() const
  {
    return _mode_ == MODE_SPECTRUM;
  }

  void single_particle_generator::set_mean_energy(double mean_, double sigma_)
  {
    DT_THROW_IF(_initialized_,std::logic_error, "Operation prohibited ! Object is locked/initialized !");
    DT_THROW_IF(mean_ <= 0.0, std::logic_error, "Invalid mean energy value !");
    _mean_energy_ = mean_;
    DT_THROW_IF(sigma_ < 0.0, std::logic_error, "Invalid sigma energy value !");
    _sigma_energy_ = sigma_;
    return;
  }

  double single_particle_generator::get_mean_energy() const
  {
    return _mean_energy_;
  }

  double single_particle_generator::get_sigma_energy() const
  {
    return _sigma_energy_;
  }

  void single_particle_generator::set_energy_range(double min_, double max_)
  {
    DT_THROW_IF(_initialized_,std::logic_error, "Operation prohibited ! Object is locked/initialized !");
    DT_THROW_IF(min_ < 0.0, std::logic_error, "Invalid min energy value (0 <= min energy) !");
    DT_THROW_IF(max_ < min_, std::logic_error, "Invalid max energy value (0 <= min energy < max energy) !");
    _min_energy_ = min_;
    _max_energy_ = max_;
    return;
  }

  double single_particle_generator::get_min_energy() const
  {
    return _min_energy_;
  }

  double single_particle_generator::get_max_energy() const
  {
    return _max_energy_;
  }


  void single_particle_generator::add_multi_ray(double energy_, double prob_)
  {
    DT_THROW_IF(_initialized_,std::logic_error, "Operation prohibited ! Object is locked/initialized !");
    if (!is_mode_valid()) {
      set_mode(MODE_MULTI_RAYS);
    }
    DT_THROW_IF(!datatools::is_valid(energy_) || energy_ <= 0.0,
                std::logic_error, "Invalid multi-rays energy !");
    DT_THROW_IF( !datatools::is_valid(prob_) || prob_ <= 0.0,
                 std::logic_error, "Invalid multi-rays probability !");
    for (size_t i = 0; i < _multi_rays_records_.size(); i++) {
      const multi_rays_record_type & rec = _multi_rays_records_[i];
      DT_THROW_IF(rec.energy == energy_, std::logic_error,
                  "Duplicated ray with energy '" << energy_ / CLHEP::keV << "'!");
    }
    multi_rays_record_type record;
    record.energy = energy_;
    record.probability = prob_;
    record.cumul_probability = 0.0;
    _multi_rays_records_.push_back(record);
    return;
  }

  // void single_particle_generator::set_energy_list(const std::vector<double> en_)
  // {
  //   DT_THROW_IF(_initialized_,std::logic_error, "Operation prohibited ! Object is locked/initialized !");
  //   _multi_rays_energies_ = en_;
  //   std::vector<double>::const_iterator it = en_.begin();
  //   double min = *it, max = *it;
  //   while (it < en_.end()) {
  //     if (*it < min) _min_energy_ = *it;
  //     else if (*it > max) _max_energy_ = *it;
  //     it++;
  //   }
  //   return;
  // }

  // void single_particle_generator::set_probability_list(const std::vector<double> prob_)
  // {
  //   DT_THROW_IF(_initialized_,std::logic_error, "Operation prohibited ! Object is locked/initialized !");
  //   _multi_rays_probabilities_ = prob_;
  //   return;
  // }

  void single_particle_generator::set_energy_spectrum_filename(const std::string & filename_)
  {
    DT_THROW_IF(_initialized_,std::logic_error, "Operation prohibited ! Object is locked/initialized !");
    _energy_spectrum_filename_ = filename_;
    return;
  }

  void single_particle_generator::_set_defaults()
  {
    _particle_name_ = "";
    _particle_type_ = primary_particle::PARTICLE_UNDEFINED;
    datatools::invalidate(_particle_mass_);
    _mode_ = MODE_INVALID;
    _mean_energy_ = 0.0;
    datatools::invalidate(_mean_energy_);
    _sigma_energy_ = 0.0;
    _min_energy_ = 0.0;
    datatools::invalidate(_min_energy_);
    _max_energy_ = 0.0;
    datatools::invalidate(_max_energy_);
    _spectrum_interpolation_name_ = mygsl::tabulated_function::linear_interp_name();
    _direction_mode_ = DIRECTION_DEFAULT;
    _cone_max_angle_ = 0.0 * CLHEP::degree;
    _cone_min_angle_ = 0.0 * CLHEP::degree;
    _cone_axis_.set(0.0, 0.0, 1.0);
    _energy_spectrum_filename_.clear();
    _multi_rays_records_.clear();
    _seed_ = 0;
    return;
  }

  single_particle_generator::single_particle_generator() : i_genbb()
  {
    _initialized_ = false;
    _set_defaults();
    return;
  }

  single_particle_generator::~single_particle_generator()
  {
    if (_initialized_) {
      reset();
    }
    return;
  }

  const mygsl::rng & single_particle_generator::get_random() const
  {
    if (has_external_random()) {
      return get_external_random();
    }
    return *_random_;
  }

  mygsl::rng & single_particle_generator::grab_random()
  {
    if (has_external_random()) {
      return grab_external_random();
    }
    return *_random_;
  }

  void single_particle_generator::_at_reset_()
  {
    if (_ion_data_) {
      _ion_data_.reset();
    }
    _vnm_.reset();
    _energy_spectrum_.reset();
    if (_random_) {
      if (_random_->is_initialized()) {
        _random_->reset();
      }
      _random_.reset();
    }
    _set_defaults();
    return;
  }

  void single_particle_generator::reset()
  {
    if (! _initialized_) return;
    _at_reset_();
    _initialized_ = false;
    return;
  }

  void single_particle_generator::initialize(const datatools::properties & config_,
                                               datatools::service_manager & /*service_manager_*/,
                                               detail::pg_dict_type & /*dictionary_*/)
  {
    DT_THROW_IF(_initialized_,std::logic_error, "Operation prohibited ! Object is already initialized !");

    _initialize_base(config_);

    if (! has_external_random()) {
      DT_THROW_IF(! config_.has_key("seed"), std::logic_error,
                  "Missing 'seed' property for particle generator '" << get_name() << "' !");
      long seed = config_.fetch_integer("seed");
      DT_THROW_IF(seed < 0, std::logic_error, "Invalid seed value (>=0) for particle generator '" << get_name() << "' !");
      _seed_ = seed;
    }

    DT_THROW_IF(!config_.has_key("particle_name"), std::logic_error,
                "Missing 'particle_name' property for particle generator '" << get_name() << "' !");
    const std::string particle_name = config_.fetch_string("particle_name");
    set_particle_name(particle_name);

    if (config_.has_key("emission_direction")) {
      std::string emission_direction = config_.fetch_string("emission_direction");
      if (emission_direction == "z_axis") {
        set_z_direction(true);
      } else if (emission_direction == "randomized") {
        set_randomized_direction(true);
      } else if (emission_direction == "cone") {
        set_cone_direction(true);
      } else {
        DT_THROW_IF(true, std::logic_error,
                    "Invalid 'emission_direction' property for particle generator '" << get_name() << "' !");
      }
    } else if (config_.has_flag("z_axis_direction")) {
      set_z_direction(false);
    } else if (config_.has_flag("randomized_direction")) {
      set_randomized_direction(true);
    } else if (config_.has_flag("cone_direction")) {
      set_cone_direction(true);
    }

    double angle_unit = CLHEP::degree;
    if (config_.has_key("angle_unit")) {
      std::string angle_unit_str = config_.fetch_string("angle_unit");
      angle_unit = datatools::units::get_angle_unit_from(angle_unit_str);
    }

    if (is_cone_direction()) {
      if (config_.has_key("cone.max_angle")) {
        double cone_max_angle = config_.fetch_real("cone.max_angle");
        if (! config_.has_explicit_unit("cone.max_angle")) cone_max_angle *= angle_unit;
        set_cone_max_angle(cone_max_angle);
        if (config_.has_key("cone.min_angle")) {
          double cone_min_angle = config_.fetch_real("cone.min_angle");
          if (! config_.has_explicit_unit("cone.min_angle")) cone_min_angle *= angle_unit;
          set_cone_min_angle(cone_min_angle);
        }
      }

      if (config_.has_key("cone.axis")) {
        std::string cone_axis_str = config_.fetch_string("cone.axis");
        geomtools::vector_3d cone_axis;
        if (cone_axis_str == "x" || cone_axis_str == "X" || cone_axis_str == "+x" || cone_axis_str == "+X") {
          cone_axis.set(1.0, 0.0, 0.0);
        } else if (cone_axis_str == "y" || cone_axis_str == "Y" || cone_axis_str == "+y" || cone_axis_str == "+Y") {
          cone_axis.set(0.0, 1.0, 0.0);
        } else if (cone_axis_str == "z" || cone_axis_str == "Z" || cone_axis_str == "+z" || cone_axis_str == "+Z") {
          cone_axis.set(0.0, 0.0, 1.0);
        } else if (cone_axis_str == "-x" || cone_axis_str == "-X") {
          cone_axis.set(-1.0, 0.0, 0.0);
        } else if (cone_axis_str == "-y" || cone_axis_str == "-Y") {
          cone_axis.set(0.0, -1.0, 0.0);
        } else if (cone_axis_str == "-z" || cone_axis_str == "-Z") {
          cone_axis.set(0.0, 0.0, -1.0);
        } else {
          double phi, theta;
          double angle_unit2 = angle_unit;
          std::string angle_unit2_str;
          std::istringstream iss(cone_axis_str);
          iss >> phi >> theta >> std::ws >> angle_unit2_str;
          if (angle_unit2_str.empty()) {
            angle_unit2 = datatools::units::get_angle_unit_from(angle_unit2_str);
          }
          DT_THROW_IF(!iss, std::logic_error,
                      "Invalid format for phi and/or theta angles !");
          phi *= angle_unit2;
          theta *= angle_unit2;
          cone_axis.set(0.0,0.0,1.0);
          cone_axis.setTheta(theta);
          cone_axis.setPhi(phi);
        }
        set_cone_axis(cone_axis);
      }
    }

    DT_THROW_IF(! config_.has_key("mode"), std::logic_error, "Missing 'mode' property !");
    std::string mode_str = config_.fetch_string("mode");
    if (mode_str == "gaussian_energy") {
      set_mode(MODE_GAUSSIAN_ENERGY);
    } else if (mode_str == "energy_range") {
      set_mode(MODE_ENERGY_RANGE);
    } else if (mode_str == "monokinetic") {
      set_mode(MODE_MONOKINETIC);
    } else if (mode_str == "multi_rays") {
      set_mode(MODE_MULTI_RAYS);
    } else if (mode_str == "spectrum") {
      set_mode(MODE_SPECTRUM);
      _spectrum_mode_ = SPECTRUM_MODE_TABFUNC;
      if (config_.has_key("spectrum.mode")) {
        std::string spectrum_mode_str = config_.fetch_string("spectrum.mode");
        if (spectrum_mode_str == "tabulated_function") {
          _spectrum_mode_ = SPECTRUM_MODE_TABFUNC;
        } else if (spectrum_mode_str == "histogram_pdf") {
          _spectrum_mode_ = SPECTRUM_MODE_HISTPDF;
        }
      }
    } else {
      DT_THROW_IF(true, std::logic_error, "Invalid mode '" << mode_str << "' !");
    }

    double energy_unit = CLHEP::keV;
    if (config_.has_key("energy_unit")) {
      std::string unit_str = config_.fetch_string("energy_unit");
      energy_unit = datatools::units::get_energy_unit_from(unit_str);
    }

    if (_mode_ == MODE_MONOKINETIC) {
      DT_THROW_IF(! config_.has_key("energy"), std::logic_error, "Missing 'energy' property !");
      double energy = config_.fetch_real("energy");
      if (! config_.has_explicit_unit("energy")) energy *= energy_unit;
      set_mean_energy(energy, 0.0);
    }

    if (_mode_ == MODE_GAUSSIAN_ENERGY) {
      DT_THROW_IF(! config_.has_key("mean_energy"), std::logic_error, "Missing 'mean_energy' property !");
      double mean_energy = config_.fetch_real("mean_energy");
      if (! config_.has_explicit_unit("mean_energy")) mean_energy *= energy_unit;

      double sigma_energy = 0.0;
      if (config_.has_key("sigma_energy")) {
        sigma_energy = config_.fetch_real("sigma_energy");
        if (! config_.has_explicit_unit("sigma_energy")) sigma_energy *= energy_unit;
      }
      set_mean_energy(mean_energy, sigma_energy);
    }

    if (_mode_ == MODE_ENERGY_RANGE) {
      DT_THROW_IF(! config_.has_key("max_energy"), std::logic_error, "Missing 'max_energy' property !");
      double max_energy = config_.fetch_real("max_energy");
      if (! config_.has_explicit_unit("max_energy")) max_energy *= energy_unit;
      DT_THROW_IF(! config_.has_key("min_energy"), std::logic_error, "Missing 'min_energy' property !");
      double min_energy = config_.fetch_real("min_energy");
      if (! config_.has_explicit_unit("min_energy")) min_energy *= energy_unit;
      set_energy_range(min_energy, max_energy);
    }

    if (_mode_ == MODE_MULTI_RAYS) {
      DT_THROW_IF(! config_.has_key("multi_rays.energies"), std::logic_error, "Missing 'multi_rays.energies' property !");
      DT_THROW_IF(! config_.has_key("multi_rays.probabilities"), std::logic_error, "Missing 'multi_rays.probabilities' property !");

      std::vector<double> en;
      std::vector<double> prob;
      if (config_.has_key("multi_rays.energies")) {
        config_.fetch("multi_rays.energies", en);
        if (!config_.has_explicit_unit("multi_rays.energies")) {
          for (size_t i = 0; i < en.size(); i++) {
            en[i] *= energy_unit;
          }
        }
      }
      if (config_.has_key("multi_rays.probabilities")) {
        config_.fetch("multi_rays.probabilities", prob);
      }

      DT_THROW_IF(en.size() != prob.size(), std::logic_error, "Size of 'multi_rays.energies' and 'multi_rays.probabilities' properties are differents !");

      for (size_t i = 0; i < en.size(); i++) {
        add_multi_ray(en[i], prob[i]);
      }

      // Probability normalization:
      double prob_sum = 0.0;
      for (size_t i = 0; i < _multi_rays_records_.size(); i++) {
        const multi_rays_record_type & rec = _multi_rays_records_[i];
        prob_sum += rec.probability;
      }
      for (size_t i = 0; i < _multi_rays_records_.size(); i++) {
        multi_rays_record_type & rec = _multi_rays_records_[i];
        rec.probability /= prob_sum;
        if (i == 0) {
          rec.cumul_probability = rec.probability;
        } else {
          rec.cumul_probability = _multi_rays_records_[i-1].cumul_probability  + rec.probability;
        }
      }

    }

    if (_mode_ == MODE_SPECTRUM) {
      if (config_.has_key("spectrum.data_file")) {
        std::string spectrum_filename = config_.fetch_string("spectrum.data_file");
        set_energy_spectrum_filename(spectrum_filename);
      }
      double min_energy = _min_energy_;
      double max_energy = _max_energy_;

      if (config_.has_key("spectrum.min_energy")) {
        min_energy = config_.fetch_real("spectrum.min_energy");
        if (! config_.has_explicit_unit("spectrum.min_energy")) min_energy *= energy_unit;
      }

      if (config_.has_key("spectrum.min_energy")) {
        max_energy = config_.fetch_real("spectrum.max_energy");
        if (! config_.has_explicit_unit("spectrum.max_energy")) max_energy *= energy_unit;
      }

      set_energy_range(min_energy, max_energy);
    }

    _at_init_();

    _initialized_  = true;
    return;
  }

  bool single_particle_generator::has_next ()
  {
    return true;
  }

  void single_particle_generator::_load_next (primary_event & event_,
                                              bool compute_classification_)
  {
    DT_LOG_TRACE(_logging_priority, "Entering...");
    DT_THROW_IF(! _initialized_, std::logic_error, "Generator is not locked/initialized !");
    event_.reset();

    double kinetic_energy;
    datatools::invalidate(kinetic_energy);
    const double mass = _particle_mass_;
    if (_mode_ == MODE_MONOKINETIC) {
      kinetic_energy = _mean_energy_;
    }

    if (_mode_ == MODE_GAUSSIAN_ENERGY) {
      while (kinetic_energy <= 0.0) {
        kinetic_energy = grab_random().gaussian(_mean_energy_, _sigma_energy_);
      }
    }

    if (_mode_ == MODE_ENERGY_RANGE) {
      kinetic_energy = grab_random().flat(_min_energy_, _max_energy_);
    }

    if (_mode_ == MODE_MULTI_RAYS) {
      const double prob = grab_random().flat(0., 1.0);
      for (size_t i = 0; i < _multi_rays_records_.size(); i++) {
        const multi_rays_record_type & rec = _multi_rays_records_[i];
        if (prob <= rec.cumul_probability) {
          kinetic_energy = rec.energy;
          break;
        }
      }
      if (!datatools::is_valid(kinetic_energy)) {
        kinetic_energy = _multi_rays_records_.back().energy;
      }
    }

    if (_mode_ == MODE_SPECTRUM) {
      if (_spectrum_mode_ == SPECTRUM_MODE_TABFUNC) {
        kinetic_energy = _vnm_.shoot (grab_random());
      } else if (_spectrum_mode_ == SPECTRUM_MODE_HISTPDF) {
        kinetic_energy = _energy_histo_pdf_.sample(grab_random ());
      }
    }

    event_.set_time(0.0 * CLHEP::second);
    primary_particle pp;
    pp.set_type(_particle_type_);
    if (pp.needs_particle_label()) {
      pp.set_particle_label(_particle_name_);
    }
    pp.set_mass(_particle_mass_);
    pp.set_time(0.0 * CLHEP::second);
    const double momentum = std::sqrt(kinetic_energy * (kinetic_energy + 2 * mass));
    geomtools::vector_3d p(0.0, 0.0, momentum);
    pp.set_momentum(p);
    event_.add_particle(pp);

    if (is_randomized_direction()) {
      double phi = grab_random().flat(0.0, 360.0 * CLHEP::degree);
      double cos_theta = grab_random().flat(-1.0, +1.0);
      double theta = std::acos(cos_theta);
      double psi = grab_random().flat(0.0, 360.0 * CLHEP::degree);
      event_.rotate(phi, theta, psi);
    } else if (is_cone_direction()) {
      double phi = grab_random().flat(0.0, 360.0 * CLHEP::degree);
      double cos_theta = grab_random().flat(std::cos(_cone_max_angle_), std::cos(_cone_min_angle_));
      double theta = std::acos(cos_theta);
      double psi = grab_random().flat(0.0, 360.0 * CLHEP::degree);
      event_.rotate(phi, theta, psi);
      double theta_dir = _cone_axis_.getTheta();
      double phi_dir = _cone_axis_.getPhi();
      event_.rotate(phi_dir, theta_dir, 0.0);
    }

    event_.set_label(i_genbb::get_name());
    if (compute_classification_) {
      event_.compute_classification();
    }

    DT_LOG_TRACE(_logging_priority, "Exiting.");
    return;
  }

  void single_particle_generator::_init_energy_histo_pdf()
  {
    std::string filename = _energy_spectrum_filename_;
    datatools::fetch_path_with_env(filename);
    std::ifstream ifile(filename.c_str());
    DT_THROW_IF(! ifile, std::logic_error, "Cannot open data file '" << _energy_spectrum_filename_ << "' !");

    double energy_unit = CLHEP::MeV;
    size_t nbins = 0;
    double min = 0.0, max = 0.0;
    // load histo
    while (! ifile.eof()) {
      std::string line;
      getline(ifile, line);
      if (line.empty())  {
        continue;
      }
      {
        std::istringstream lineiss(line);
        std::string word;
        lineiss >> word;
        if ((word.length() > 0) && (word[0] == '#')) {
          if (word == "#@limits") {
            lineiss >> nbins >> min >> max;
            // initialize histo
            _energy_histo_.init(nbins, min * energy_unit, max * energy_unit);
          } else if (word == "#@energy_unit") {
            std::string energy_unit_str;
            lineiss >> energy_unit_str;
            energy_unit = datatools::units::get_energy_unit_from(energy_unit_str);
          }
          continue;
        }
      }

      DT_THROW_IF(!nbins, std::logic_error, "Limits to histogram are not given in data file '"
                  << _energy_spectrum_filename_ << "' !");

      std::istringstream lineiss(line);
      double xmin, xmax;
      double weight;
      lineiss >> xmin >> xmax >> weight;
      DT_THROW_IF(! lineiss, std::logic_error, "Format error in data file '"
                  << _energy_spectrum_filename_ << "' !");

      xmin *= energy_unit;
      xmax *= energy_unit;

      DT_THROW_IF(xmin < 0.0 || xmax < 0.0, std::out_of_range,
                  "Invalid energy value < 0) !");
      DT_THROW_IF(weight < 0.0, std::out_of_range, "Invalid spectrum value (" << weight << " < 0) !");

      // filling histogram
      const double mean_energy = (xmin + xmax)/2.0;
      if (mean_energy < _min_energy_ || mean_energy > _max_energy_)
        continue;

      _energy_histo_.fill(mean_energy, weight);
    }

    // Calculate histogram PDF
    _energy_histo_pdf_.init(_energy_histo_);
    return;
  }

  void single_particle_generator::_init_energy_spectrum()
  {
    DT_THROW_IF(_energy_spectrum_.is_table_locked(), std::logic_error,
                "Tabulated energy spectrum is already locked !");
    std::string filename = _energy_spectrum_filename_;
    datatools::fetch_path_with_env(filename);
    std::ifstream ifile;
    ifile.open(filename.c_str());
    DT_THROW_IF(! ifile, std::logic_error, "Cannot open interpolation spectrum data file '"
                << _energy_spectrum_filename_ << "' !");
    std::string interpolator_name;
    double energy_unit = CLHEP::MeV;
    double ymax = -1.0;
    while (! ifile.eof()) {
      std::string line;
      getline(ifile, line);
      if (line.empty()) {
        continue;
      }
      {
        std::istringstream lineiss(line);
        std::string word;
        lineiss >> word;
        if ((word.length() > 0) && (word[0] == '#')) {
          if (word == "#@interpolation_name") {
            lineiss >> interpolator_name;
          } else if (word == "#@energy_unit") {
            std::string energy_unit_str;
            lineiss >> energy_unit_str;
            energy_unit = datatools::units::get_energy_unit_from(energy_unit_str);
          }
          continue;
        }
      }
      std::istringstream lineiss(line);
      double x, y;
      lineiss >> x >> y;
      DT_THROW_IF(! lineiss, std::logic_error, "Format error in interpolation data file '"
                  << _energy_spectrum_filename_ << "' !");
      x *= energy_unit;
      DT_THROW_IF(x < 0.0, std::out_of_range, "Invalid energy value (" << x << " < 0) !");
      DT_THROW_IF(y < 0.0, std::out_of_range, "Invalid spectrum value (" << y << " < 0) !");

      if (x < _min_energy_ || x > _max_energy_)
        continue;

      _energy_spectrum_.add_point(x, y, false);
      if (y > ymax) ymax = y;
    }
    if (interpolator_name.empty()) {
      interpolator_name = _spectrum_interpolation_name_;
    }
    _energy_spectrum_.lock_table(interpolator_name);
    _vnm_.init(_energy_spectrum_.x_min(),
               _energy_spectrum_.x_max(),
               _energy_spectrum_,
               ymax * 1.001);
    return;
  }

  void single_particle_generator::_at_init_()
  {
    DT_LOG_TRACE(get_logging_priority(), "Entering...");
    _particle_type_ = primary_particle::PARTICLE_UNDEFINED;

    // Search for a nucleus (Z,A,E*):
    if (_particle_type_ == primary_particle::PARTICLE_UNDEFINED) {
      DT_LOG_TRACE(get_logging_priority(), "Search for a nucleus...");
      int Z, A;
      double Estar;
      if (primary_particle::label_to_nucleus(_particle_name_, Z, A, Estar)) {
        DT_LOG_TRACE(get_logging_priority(), "Found a nucleus...");
        _ion_data_.reset(new ion_data_type);
        _ion_data_->Z = Z;
        _ion_data_->A = A;
        _ion_data_->Estar = Estar;
        _particle_type_ = primary_particle::NUCLEUS;
        if (!datatools::is_valid(_particle_mass_)) {
          DT_LOG_TRACE(get_logging_priority(), "Searching for the mass of the nucleus...");
          materials::isotope::id nucleus_id(Z, A);
          if (materials::isotope::id_is_tabulated(nucleus_id)) {
            DT_LOG_TRACE(get_logging_priority(), "Nucleus is tabulated...");
            const materials::isotope::record_type & isorec
              = materials::isotope::table_record_from_id(nucleus_id);
            double bea = isorec.get_binding_energy_per_nucleon();
            DT_LOG_TRACE(get_logging_priority(), "Nucleus' binding energy per nucleon is " << bea / CLHEP::keV << " keV");
            DT_THROW_IF(!datatools::is_valid(bea), std::logic_error,
                        "Cannot fetch binding energy per nucleon for isotope " << nucleus_id.to_string() << " !");
            set_particle_mass(materials::isotope::compute_nucleus_mass(Z, A, bea));
            DT_LOG_TRACE(get_logging_priority(), "Nucleus' mass is " << get_particle_mass() / CLHEP::GeV << " GeV");
          } else {
            DT_THROW_IF(true, std::logic_error, "No registered isotope " << nucleus_id.to_string() << " !");
          }
        }
      }
    }

    // Search for an ion (Z,A,E*,Q):
    if (_particle_type_ == primary_particle::PARTICLE_UNDEFINED) {
      DT_LOG_TRACE(get_logging_priority(), "Search for an ion...");
      int Z, A, Q;
      double Estar;
      if (primary_particle::label_to_ion(_particle_name_, Z, A, Estar, Q)) {
        _ion_data_.reset(new ion_data_type);
        _ion_data_->Z = Z;
        _ion_data_->A = A;
        _ion_data_->Estar = Estar;
        _ion_data_->Q = Q;
        _particle_type_ = primary_particle::ION;
        if (!datatools::is_valid(_particle_mass_)) {
          materials::isotope::id nucleus_id(Z, A);
          if (materials::isotope::id_is_tabulated(nucleus_id)) {
            const materials::isotope::record_type & isorec
              = materials::isotope::table_record_from_id(nucleus_id);
            double atomic_mass = isorec.get_atomic_mass();
            DT_THROW_IF(!datatools::is_valid(atomic_mass), std::logic_error, "Cannot fetch atomic mass for isotope " << nucleus_id.to_string() << " !");
            double ion_mass = atomic_mass * CLHEP::amu_c2 - Q * CLHEP::electron_mass_c2;
            set_particle_mass(ion_mass);
          } else {
            DT_THROW_IF(true, std::logic_error, "No registered isotope " << nucleus_id.to_string() << " !");
          }
        }
      }
    }

    // Search for other particles:
    if (_particle_type_ == primary_particle::PARTICLE_UNDEFINED) {
      DT_LOG_TRACE(get_logging_priority(), "Search for a particle...");
      _particle_type_ = primary_particle::particle_type_from_label(_particle_name_);
      if (!datatools::is_valid(_particle_mass_)) {
        _particle_mass_ = get_particle_mass_from_label(_particle_name_);
      }
      DT_THROW_IF(! datatools::is_valid(_particle_mass_), std::logic_error,
                  "Particle mass is not defined !");
      DT_THROW_IF(_cone_min_angle_ > _cone_max_angle_, std::out_of_range,
                  "Invalid  cone angle range (" << _cone_min_angle_ << ">" << _cone_max_angle_ << ") !");
    }

    DT_THROW_IF(_particle_type_ == primary_particle::PARTICLE_UNDEFINED,
                std::logic_error,
                "Unsupported particule!");

    if (_mode_ == MODE_SPECTRUM) {
      if (_spectrum_mode_ == SPECTRUM_MODE_TABFUNC) {
        _init_energy_spectrum();
      } else if (_spectrum_mode_ == SPECTRUM_MODE_HISTPDF) {
        _init_energy_histo_pdf();
      }
    }

    if (! has_external_random()) {
      _random_.reset(new mygsl::rng);
      _random_->init("taus2", _seed_);
    }

    return;
  }

} // end of namespace genbb

/***************
 * OCD support *
 ***************/

#include <datatools/ocd_macros.h>

// OCD support for class '::genbb::single_particle_generator' :
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(::genbb::single_particle_generator,ocd_)
{
  ocd_.set_class_name("genbb::single_particle_generator");
  ocd_.set_class_description("A single particle generator");
  ocd_.set_class_library("genbb_help");
  ocd_.set_class_documentation("The ``genbb::single_particle_generator`` enables to          \n"
                               "randomize the initial kinematics of an unique particle       \n"
                               "as the input of some Monte-Carlo process (example: Geant4).  \n"
                               "It can be typically initialized from an ASCII configuration  \n"
                               "file. The generator can be given an external PRNG provided   \n"
                               "by the user or use its own PRNG.                             \n"
                               );
  {
    configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("logging.priority")
      .set_terse_description("Set the logging priority threshold")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_default_value_string("warning")
      .set_long_description("Allowed values are:                           \n"
                            "                                              \n"
                            " * ``\"trace\"``                              \n"
                            " * ``\"debug\"``                              \n"
                            " * ``\"information\"``                        \n"
                            " * ``\"notice\"``                             \n"
                            " * ``\"warning\"``                            \n"
                            " * ``\"error\"``                              \n"
                            " * ``\"critical\"``                           \n"
                            " * ``\"fatal\"``                              \n"
                            "                                              \n"
                            )
      .add_example("Set the logging threshold::                   \n"
                   "                                              \n"
                   "  logging.priority: string = \"notice\"       \n"
                   "                                              \n"
                   )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("seed")
      .set_terse_description("Embeded PRNG's seed")
      .set_traits(datatools::TYPE_INTEGER)
      .set_mandatory(false)
      .set_complex_triggering_conditions(true)
      .set_long_description("The seed of the embeded PRNG.                 \n"
                            "Not used if some external PRNG is used.       \n"
                            )
      .add_example("Set the PRNG seed with an arbitrary value::   \n"
                   "                                              \n"
                   "  seed: integer = 314159                      \n"
                   "                                              \n"
                   )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("particle_name")
      .set_terse_description("The name of the generated particle")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(true)
      .set_long_description("The name of the generated particle must be taken from the \n"
                            "following list:                               \n"
                            "                                              \n"
                            "  * ``\"electron\"`` or ``\"e-\"``            \n"
                            "  * ``\"positron\"`` or ``\"e+\"``            \n"
                            "  * ``\"gamma\"``                             \n"
                            "  * ``\"alpha\"``                             \n"
                            "  * ``\"neutron\"``                           \n"
                            "  * ``\"proton\"``                            \n"
                            "  * ``\"mu-\"``                               \n"
                            "  * ``\"mu+\" ``                              \n"
                            "                                              \n"
                            )
      .add_example("Generate gamma particles::                    \n"
                   "                                              \n"
                   "  particle_name: string = \"gamma\"           \n"
                   "                                              \n"
                   )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("emission_direction")
      .set_terse_description("The label representing the emission direction mode")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description("The label representing the emission direction mode \n"
                            "must be taken from the following list:             \n"
                            "                                                   \n"
                            "  * \"z_axis\" : the particle is emitted along the positive Z axis.        \n"
                            "  * \"randomized\" : the particle is emitted in a random direction (4xPi). \n"
                            "  * \"cone\" : the particle is emitted within an arbitrary cone of         \n"
                            "    which the characteristics (angles and axis) must be provided.          \n")
      .add_example("Randomization (4 pi) of the direction::            \n"
                   "                                                   \n"
                   "  emission_direction : string = \"randomized\"     \n"
                   "                                                   \n"
                   )
      .add_example("Randomization in a cone of the direction::         \n"
                   "                                                   \n"
                   "  emission_direction : string = \"cone\"           \n"
                   "  cone.min_angle : real as angle =  0 degree       \n"
                   "  cone.max_angle : real as angle = 25 degree       \n"
                   "  cone.axis : string = \"30 45 degree\"            \n"
                   "                                                   \n"
                   )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("angle_unit")
      .set_terse_description("The default unit used for angular parameters")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_default_value_string("degree")
      .set_long_description("The angle unit can be taken from the following list: ``\"rad\"``,  \n"
                            "``\"radian\"``, ``\"mrad\"``, ``\"milliradian\"``, ``\"deg\"``     \n"
                            "and ``\"degree\"``.                                                \n"
                            )
      .add_example("Use degree as the default angle unit::            \n"
                   "                                                  \n"
                   "  angle_unit : string = \"degree\"                \n"
                   )
      ;

  }

  {
    configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("energy_unit")
      .set_terse_description("The default unit used for energy parameters")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_default_value_string("keV")
      .set_long_description("The energy unit can be taken from the following list::       \n"
                            "``\"keV\"``, ``\"MeV\"``...                                  \n"
                            )
      .add_example("Use keV as the default angle unit::             \n"
                   "                                                \n"
                   "  energy_unit : string = \"keV\"                \n"
                   )
      ;

  }

  // cone.axis
  {
    configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("cone.axis")
      .set_terse_description("The axis of the emission cone")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_default_value_string("+Z")
      .set_triggered_by_label("emission_direction", "cone")
      .set_long_description("The axis of the emission cone can be taken from the following list:          \n"
                            "                                                                             \n"
                            "  * ``\"x\"``, ``\"X\"``, ``\"+x\"``, ``\"+X\"`` : positive X-axis direction \n"
                            "  * ``\"y\"``, ``\"Y\"``, ``\"+y\"``, ``\"+Y\"`` : positive Y-axis direction \n"
                            "  * ``\"z\"``, ``\"Z\"``, ``\"+y\"``, ``\"+Y\"`` : positive Z-axis direction \n"
                            "  * ``\"-x\"``, ``\"-X\"`` : negative X-axis direction                       \n"
                            "  * ``\"-y\"``, ``\"-Y\"`` : negative Y-axis direction                       \n"
                            "  * ``\"-z\"``, ``\"-Z\"`` : negative Z-axis direction                       \n"
                            "                                                                             \n"
                            "or explicitely set with phi and theta angles:  ``\"PHI THETA [UNIT]\"``  \n"
                            "                                                                             \n"
                            )
      .add_example("Use the positive Y axis direction::               \n"
                   "                                                  \n"
                   "  cone.axis : string = \"+Y\"                     \n"
                   "                                                  \n"
                   )
      .add_example("Use an arbitrary axis::                           \n"
                   "                                                  \n"
                   "  cone.axis : string = \"30.0 45.0 degree\"       \n"
                   "                                                  \n"
                   )
      ;

  }

  // cone.min_angle
  {
    configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("cone.min_angle")
      .set_terse_description("The minimum angle of the emission cone")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(false)
      .set_default_value_real(0.0)
      .set_triggered_by_label("emission_direction", "cone")
      .set_long_description("The minimum angle of the emission cone.  \n"
                            )
      .add_example("Minimal cone angle is set to 0::                  \n"
                   "                                                  \n"
                   "  cone.min_angle : real as angle = 0 degree       \n"
                   "                                                  \n"
                   )
      ;
  }

  // cone.max_angle
  {
    configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("cone.max_angle")
      .set_terse_description("The maximum angle of the emission cone")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(false)
      .set_default_value_real(0.0)
      .set_triggered_by_label("emission_direction", "cone")
      .set_long_description("The maximum angle of the emission cone.  \n"
                            )
      .add_example("Maximum cone angle is set to 30 degrees::         \n"
                   "                                                  \n"
                   "  cone.max_angle : real as angle = 30 degree      \n"
                   "                                                  \n"
                   )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("mode")
      .set_terse_description("The energy spectrum mode")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(true)
      .set_long_description("The label representing the energy dspectrum mode \n"
                            "must be taken from the following list:             \n"
                            "                                                   \n"
                            "  * ``\"monokinetic\"`` : the particle is emitted with a monokinetic energy        \n"
                            "  * ``\"gaussian_energy\"`` : the particle is emitted with a gaussian random       \n"
                            "    kinetic energy of which the characteristics must be provided.                  \n"
                            "  * ``\"energy_range\"`` : the particle is emitted with a flat distributed         \n"
                            "    kinetic energy in a range of which the characteristics must be provided.       \n"
                            "  * ``\"spectrum\"`` : the particle is emitted with a kinetic energy following     \n"
                            "    a PDF read from a file that must be provided.                                  \n"
                            )
      .add_example("Monokinetic energy mode::                          \n"
                   "                                                   \n"
                   "  mode        : string = \"monokinetic\"           \n"
                   "  energy_unit : string = \"keV\"                   \n"
                   "  energy      : real   = 511.0 # use default unit  \n"
                   "                                                   \n"
                   )
      .add_example("Gaussian random energy mode::                      \n"
                   "                                                   \n"
                   "  mode         : string = \"gaussian_energy\"      \n"
                   "  energy_unit  : string = \"keV\"                  \n"
                   "  mean_energy  : real as energy  = 1.0 MeV         \n"
                   "  sigma_energy : real as energy = 100.0 keV        \n"
                   "                                                    \n"
                   )
      .add_example("Energy range mode::                                \n"
                   "                                                   \n"
                   "  mode        : string = \"energy_range\"          \n"
                   "  energy_unit : string = \"keV\"                   \n"
                   "  min_energy  : real   = 50 # use default unit     \n"
                   "  max_energy  : real as energy = 3.0 MeV           \n"
                   "                                                   \n"
                   )
      ;

  }

  // energy
  {
    configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("energy")
      .set_terse_description("The energy for the monokinetic mode")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_triggered_by_label("mode", "monokinetic")
      .set_long_description("The monokinetic energy of the emitted particles.\n"
                            )
      .add_example("Kinetic energy set to some arbitrary value::      \n"
                   "                                                  \n"
                   "  energy : real as energy = 1.333 MeV             \n"
                   "                                                  \n"
                   )
      ;
  }

  // sigma_energy
  {
    configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("sigma_energy")
      .set_terse_description("The energy standard deviation for the gaussian energy mode")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(false)
      .set_triggered_by_label("mode", "gaussian_energy")
      .set_long_description("The standard deviation energy of the emitted particles.\n"
                            )
      .add_example("Standard deviation of the kinetic energy set to some arbitrary value::      \n"
                   "                                                  \n"
                   "  sigma_energy : real as energy = 100 keV         \n"
                   "                                                  \n"
                   )
      ;
  }

  // mean_energy
  {
    configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("mean_energy")
      .set_terse_description("The energy for the monokinetic mode")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(false)
      .set_triggered_by_label("mode", "gaussian_energy")
      .set_long_description("The mean energy of the emitted particles.\n"
                            )
      .add_example("Kinetic energy set to some arbitrary value::      \n"
                   "                                                  \n"
                   "  mean_energy : real as energy = 850 keV          \n"
                   "                                                  \n"
                   )
      ;
  }

  // min_energy
  {
    configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("min_energy")
      .set_terse_description("The minimum energy for the range mode")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(false)
      .set_triggered_by_label("mode", "energy_range")
      .set_long_description("The minimum energy of the emitted particles.\n"
                            )
      .add_example("Minimum kinetic energy set to some arbitrary value:: \n"
                   "                                                     \n"
                   "  min_energy : real as energy = 50 keV               \n"
                   "                                                     \n"
                   )
      ;
  }

  // max_energy
  {
    configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("max_energy")
      .set_terse_description("The maximum energy for the range mode")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(false)
      .set_triggered_by_label("mode", "energy_range")
      .set_long_description("The maximum energy of the emitted particles.\n"
                            )
      .add_example("Maximum kinetic energy set to some arbitrary value:: \n"
                   "                                                     \n"
                   "  max_energy : real as energy = 1000 keV             \n"
                   "                                                     \n"
                   )
      ;
  }

  // spectrum.data_file
  {
    configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("spectrum.data_file")
      .set_terse_description("The filename that contains the shape of the energy spectrum")
      .set_traits(datatools::TYPE_STRING)
      .set_path(true)
      .set_triggered_by_label("mode", "spectrum")
      .set_long_description("The shape of the energy spectrum can be provided through a file\n"
                            "with two different modes  :                                    \n"
                            "                                                               \n"
                            " * from a tabulated function from which the shape of the       \n"
                            "   spectrum will be interpolated and used by a Von Neumann     \n"
                            "   rejection method,                                           \n"
                            " * from the an histogram of which the PDF will be used to      \n"
                            "   sample the spectrum.                                        \n"
                            "                                                               \n"
                            "The mode is set via the ``spectrum.mode`` property.            \n"
                            "The file uses an ASCII format.                                 \n"
                            "Each mode uses its own format. See the configuration hints     \n"
                            "section for a detailed description.                            \n"
                            )
      .add_example("Load the energy spectrum from a specific file: ::                       \n"
                   "                                                                        \n"
                   "  spectrum.data_file : string as path = \"energy_spectrum.data\"        \n"
                   "                                                                        \n"
                   )
      ;
  }

  // spectrum.mode
  {
    configuration_property_description & cpd = ocd_.add_property_info();
    cpd.set_name_pattern("spectrum.mode")
      .set_terse_description("The mode used to build the energy spectrum")
      .set_traits(datatools::TYPE_STRING)
      .set_triggered_by_label("mode", "spectrum")
      .set_long_description("The supported modes are:                                        \n"
                            "                                                                \n"
                            " * ``\"tabulated_function\"`` : from a tabulated function       \n"
                            " * ``\"histogram_pdf\"`` : from the PDF of an histogram         \n"
                            "                                                                \n"
                            "Each mode uses its own format. See the configuration hints      \n"
                            "section for a detailed description.                             \n"
                            )
      .add_example("Use a tabulated function as the input energy spectrum::                 \n"
                   "                                                                        \n"
                   "  spectrum.mode : string = \"tabulated_function\"                       \n"
                   "                                                                        \n"
                   )
      ;
  }

  ocd_.set_configuration_hints("Here are some typical configurations:                       \n"
                               "                                                            \n"
                               "***Example 1***                                             \n"
                               "                                                            \n"
                               "Monokinetic gamma::                                         \n"
                               "                                                            \n"
                               "  logging.priority   : string = \"notice\"                  \n"
                               "  seed               : integer = 314159                     \n"
                               "  particle_name      : string = \"gamma\"                   \n"
                               "  emission_direction : string = \"randomized\"              \n"
                               "  mode               : string = \"monokinetic\"             \n"
                               "  energy_unit        : string = \"keV\"                     \n"
                               "  energy             : real as energy = 511 keV             \n"
                               "                                                            \n"
                               "***Example 2***                                             \n"
                               "                                                            \n"
                               "Gamma with gaussian energy::                                \n"
                               "                                                            \n"
                               "  logging.priority   : string = \"notice\"                  \n"
                               "  seed               : integer = 314159                     \n"
                               "  particle_name      : string = \"gamma\"                   \n"
                               "  emission_direction : string = \"randomized\"              \n"
                               "  mode               : string = \"gaussian_energy\"         \n"
                               "  energy_unit        : string = \"keV\"                     \n"
                               "  mean_energy        : real as energy = 1.0 MeV             \n"
                               "  sigma_energy       : real as energy = 100.0 keV           \n"
                               "                                                            \n"
                               "***Example 3***                                             \n"
                               "                                                            \n"
                               "Gamma with flat ranged energy::                             \n"
                               "                                                            \n"
                               "  logging.priority   : string = \"notice\"                  \n"
                               "  seed               : integer = 314159                     \n"
                               "  particle_name      : string = \"gamma\"                   \n"
                               "  emission_direction : string = \"randomized\"              \n"
                               "  mode               : string = \"energy_range\"            \n"
                               "  energy_unit        : string = \"keV\"                     \n"
                               "  min_energy         : real as energy = 50 keV              \n"
                               "  max_energy         : real as energy = 3.0 MeV             \n"
                               "                                                            \n"
                               "***Example 4***                                             \n"
                               "                                                            \n"
                               "Gamma with energy PDF from a spectrum::                     \n"
                               "                                                            \n"
                               "  logging.priority    : string = \"notice\"                 \n"
                               "  seed                : integer = 314159                    \n"
                               "  particle_name       : string = \"gamma\"                  \n"
                               "  emission_direction  : string = \"cone\"                   \n"
                               "  angle_unit          : string = \"degree\"                 \n"
                               "  cone.max_angle      : real = 25 degree                    \n"
                               "  cone.min_angle      : real = 0 degree                     \n"
                               "  cone.axis           : string = \"20 45 degree\"           \n"
                               "  mode                : string = \"spectrum\"               \n"
                               "  energy_unit         : string = \"keV\"                    \n"
                               "  spectrum.mode       : string = \"tabulated_function\"     \n"
                               "  spectrum.data_file  : string = \"energy_spectrum.data\"   \n"
                               "  spectrum.min_energy : real as energy = 300 keV            \n"
                               "  spectrum.max_energy : real as energy = 2.0 MeV            \n"
                               "                                                            \n"
                               "where the ASCII file ``\"energy_spectrum.data\"`` contains  \n"
                               "a tabulated function and uses the following format::        \n"
                               "                                                            \n"
                               "  #@interpolation_name cspline ### used by the GSL library  \n"
                               "  #@energy_unit keV            ### for the first column     \n"
                               "  0     1.00                                                \n"
                               "  50    1.05                                                \n"
                               "  100   1.10                                                \n"
                               "  150   1.20                                                \n"
                               "  200   1.25                                                \n"
                               "  ...                                                       \n"
                               "  950   0.12                                                \n"
                               "  1000  0.05                                                \n"
                               "  1200  0.02                                                \n"
                               "  1500  0.01                                                \n"
                               "                                                            \n"
                               "The step of the interpolation samples needs not to be uniform.\n"
                               "                                                            \n"
                               "***Example 5***                                             \n"
                               "                                                            \n"
                               "If an ASCII histogram file is used as the source of the     \n"
                               "spectrum shape, one should use::                            \n"
                               "                                                            \n"
                               "  ...                                                       \n"
                               "  mode               : string = \"spectrum\"                \n"
                               "  spectrum.mode      : string = \"histogram_pdf\"           \n"
                               "  spectrum.data_file : string = \"energy_histo.data\"       \n"
                               "                                                            \n"
                               "where the file ``\"energy_histo.data\"`` uses the           \n"
                               "following format::                                          \n"
                               "                                                            \n"
                               "  #@limits 8 25.0 2000.0  ### number of bins, lower and     \n"
                               "                          ### upper bounds of the histogram \n"
                               "  #@energy_unit keV       ### for the first and second columns\n"
                               "  25     50  1.00                                           \n"
                               "  50    100  1.05                                           \n"
                               "  100   200  2.10                                           \n"
                               "  200   350  3.50                                           \n"
                               "  350   400  5.60                                           \n"
                               "  400   900  12.3                                           \n"
                               "  900  1000  0.35                                           \n"
                               "  1000 2000  0.05                                           \n"
                               "                                                            \n"
                               );
  ocd_.set_validation_support(false);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()

DOCD_CLASS_SYSTEM_REGISTRATION(genbb::single_particle_generator,"genbb::single_particle_generator")
