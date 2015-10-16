// hpge/simulated_data.cc
//
// Copyright (c) 2015 by Arnaud Chapon <achapon@cerap.fr>
//                       and François Mauger <mauger@lpccaen.in2p3.fr>
//
// This file is part of Bayeux/mctools.
//
// Bayeux/mctools is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Bayeux/mctools is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License

// Ourselves:
#include <hpge/calorimetry.h>

// Standard library:
#include <cmath>

// Third party:
// - Bayeux/datatools:
#include <datatools/exception.h>
#include <datatools/clhep_units.h>
#include <datatools/utils.h>
// - Bayeux/mygsl:
#include <mygsl/random_utils.h>

namespace hpge {

  calorimetry::calorimetry()
  {
    _initialized_ = false;
    _set_defaults();
    return;
  }

  calorimetry::calorimetry(double energy_threshold_, double energy_resolution_)
  {
    _initialized_ = false;
    _set_defaults();
    set_energy_threshold(energy_threshold_);
    set_energy_resolution(energy_resolution_);
    return;
  }

  calorimetry::~calorimetry()
  {
    if (is_initialized()) {
      reset();
    }
    return;
  }

  bool calorimetry::is_initialized() const
  {
    return _initialized_;
  }

  void calorimetry::initialize()
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Already initialized!");

    // Checks:
    DT_THROW_IF(!datatools::is_valid(_energy_threshold_), std::logic_error, "Missing energy threshold!");
    DT_THROW_IF(!datatools::is_valid(_energy_resolution_), std::logic_error, "Missing energy resolution!");
    DT_THROW_IF(!datatools::is_valid(_energy_resolution_pedestal_), std::logic_error, "Missing energy resolution pedestal!");
    DT_THROW_IF(_hit_category_.empty(), std::logic_error, "Missing hit category!");
    DT_THROW_IF(!mygsl::random_utils::is_valid_seed(_prng_seed_),
                std::logic_error, "Invalid PRNG seed!");

    _prng_.init("taus", _prng_seed_);

    _initialized_ = true;
    return;
  }

  void calorimetry::reset()
  {
    DT_THROW_IF(!is_initialized(), std::logic_error, "Not initialized yet!");
    _initialized_ = false;
    _prng_.reset();
    _hit_category_.clear();
    _set_defaults();
    return;
  }

  void calorimetry::set_logging(datatools::logger::priority prio_)
  {
    _logging_ = prio_;
    return;
  }

  datatools::logger::priority calorimetry::get_logging() const
  {
    return _logging_;
  }

  void calorimetry::set_energy_threshold(double et_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Already initialized!");
    DT_THROW_IF(et_ < 0.0, std::domain_error,
                "Invalid energy threshold (" << et_ / CLHEP::keV << " keV)!");
    _energy_threshold_ = et_;
    return;
  }

  double calorimetry::get_energy_threshold() const
  {
    return _energy_threshold_;
  }

  void calorimetry::set_energy_resolution(double er_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Already initialized!");
    DT_THROW_IF(er_ <= 0.0, std::domain_error,
                "Invalid energy resolution (" << er_ << ")!");
    _energy_resolution_ = er_;
    return;
  }

  void calorimetry::set_energy_resolution_pedestal(double erp_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Already initialized!");
    DT_THROW_IF(erp_ < 0.0, std::domain_error,
                "Invalid energy resolution pedestal (" << erp_ << ")!");
    _energy_resolution_pedestal_ = erp_;
    return;
  }

  double calorimetry::get_energy_resolution_pedestal() const
  {
    return _energy_resolution_pedestal_;
  }

  void calorimetry::set_hit_category(const std::string & hc_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Already initialized!");
    _hit_category_ = hc_;
    return;
  }

  const std::string & calorimetry::get_hit_category() const
  {
    return _hit_category_;
  }

  void calorimetry::set_only_gamma(bool og_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Already initialized!");
    _only_gamma_ = og_;
    return;
  }

  bool calorimetry::is_only_gamma() const
  {
    return _only_gamma_;
  }

  void calorimetry::set_prng_seed(int s_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Already initialized!");
    DT_THROW_IF(!mygsl::random_utils::is_valid_seed(s_),
                std::logic_error,
                "Invalid PRNG seed value (" << s_ << ")!");
    _prng_seed_ = s_;
    return;
  }

  int calorimetry::get_prng_seed() const
  {
    return _prng_seed_;
  }

  void calorimetry::_set_defaults()
  {
    _logging_ = datatools::logger::PRIO_FATAL;
    datatools::invalidate(_energy_threshold_);
    datatools::invalidate(_energy_resolution_);
    _energy_resolution_pedestal_ = 0.0;
    _prng_seed_ = mygsl::random_utils::SEED_INVALID;
    _only_gamma_ = false;
    return;
  }

  void calorimetry::process_event(const mctools::simulated_data & sd_,
                                  calorimeter_hit & hit_)
  {
    DT_THROW_IF(!is_initialized(), std::logic_error, "Not initialized yet!");

    hit_.reset();
    double energy_deposit_sum = 0.;

    // Check if the source Monte-Carlo event contains some hits of the requestes category:
    if (sd_.has_step_hits(_hit_category_)) {
      int nsteps = sd_.get_number_of_step_hits(_hit_category_);
      // Scan all Monte-Carlo (truth) hits from the requested hit category:
      for (int i = 0; i < nsteps; i++) {
        // Set an arbitrary hit identifier:
        hit_.set_hit_id(0);
        const mctools::base_step_hit & truth_hit = sd_.get_step_hit(_hit_category_, i);
        if (_only_gamma_ && truth_hit.get_particle_name() != "gamma") {
          // If only gamma rays are requested, skip other particles:
          continue;
        }
        if (truth_hit.has_geom_id()) {
          if (! hit_.has_geom_id()) {
            // Record the geometry identifier:
            hit_.set_geom_id(truth_hit.get_geom_id());
          } else {
            if (hit_.get_geom_id() != truth_hit.get_geom_id()) {
              DT_LOG_WARNING(_logging_, "Geometry ID mismatch!");
            }
          }
        }
        // Sum up the total energy deposit in the volume:
        energy_deposit_sum += truth_hit.get_energy_deposit();
      }

      static const double fwhm_sigma_factor = 2.0 * std::sqrt(2.0 * std::log(2.0));
      double mean_energy = energy_deposit_sum;
      /* FWHM = sqrt(a**2 + b*E);
       *  a=0.38;
       *  b=0.00187;
       * with E in keV
       * and  FWHM in keV
       * fwhm_energy = sqrt(a * a + b * energy / CLHEP::keV) * CLHEP::keV;
       * then sigma_energy = FWHM / 2.sqrt(2.ln(2))
       */
      double fwhm_energy = std::sqrt(_energy_resolution_pedestal_ * _energy_resolution_pedestal_
                                     + _energy_resolution_ * mean_energy / CLHEP::keV) * CLHEP::keV;

      double sigma_energy = fwhm_energy / fwhm_sigma_factor;

      // Randomize the energy deposit in the detector:
      double hit_energy_deposit = _prng_.gaussian(mean_energy, sigma_energy);

      if (hit_energy_deposit >= _energy_threshold_) {
        // Record the energy deposit in the target calorimeter hit object:
        hit_.set_energy_deposit(hit_energy_deposit);
      }
    }

    return;
  }

} // namespace hpge
