// nuclear_decay_generator.cc
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
#include <genbb_help/nuclear_decay_generator.h>

// Standard library:
#include <limits>
#include <cmath>

// Third party:
// - Bayeux/mygsl:
#include <mygsl/rng.h>

// This project:
#include <genbb_help/primary_particle.h>
#include <genbb_help/primary_event.h>
#include <genbb_help/kinematics.h>
#include <genbb_help/nuclear_level.h>
#include <genbb_help/nuclear_decay.h>
#include <genbb_help/nuclear_decay_manager.h>

namespace genbb {

  GENBB_PG_REGISTRATION_IMPLEMENT(nuclear_decay_generator, "genbb::nuclear_decay_generator")

  nuclear_decay_generator::nuclear_decay_generator() : i_genbb()
  {
    _initialized_ = false;
    _decaying_level_ = 0;
    return;
  }

  nuclear_decay_generator::~nuclear_decay_generator()
  {
    if (_initialized_) {
      reset();
    }
    return;
  }

  void nuclear_decay_generator::initialize(const datatools::properties & setup_,
                                           datatools::service_manager & /* service_manager_ */,
                                           detail::pg_dict_type & /* dictionary_ */)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Object is already initialized !");

    _initialize_base(setup_);

    if (! _ndm_) {
      datatools::properties ndm_config;

      if (setup_.has_key("manager.configuration")) {
        std::string mc = setup_.fetch_string("manager.configuration");
        datatools::fetch_path_with_env(mc);
        datatools::properties::read_config(mc, ndm_config);
      } else {
        setup_.export_and_rename_starting_with(ndm_config, "manager.", "");
      }
      if (get_logging_priority() >= datatools::logger::PRIO_TRACE) {
        ndm_config.tree_dump(std::cerr, "Nuclear decay manager configuration: ", "TRACE: ");
      }
      _ndm_.reset(new nuclear_decay_manager);
      nuclear_decay_manager & ndm = *_ndm_;
      ndm.initialize(ndm_config);
      if (get_logging_priority() >= datatools::logger::PRIO_TRACE) {
        ndm.tree_dump(std::cerr, "Nuclear decay manager: ", "TRACE: ");
      }
    }

    if (! _decaying_level_) {
      if (setup_.has_key("decaying.level")) {
        std::string dl = setup_.fetch_string("decaying.level");
        const nuclear_decay_manager::level_dict_type & levels = _ndm_->get_levels();
        DT_THROW_IF(levels.find(dl) == levels.end(),
                    std::logic_error,
                    "Cannot find level '" << dl << "' in the dictionary of levels!");
        _decaying_level_ = &levels.find(dl)->second.get();
        if (get_logging_priority() >= datatools::logger::PRIO_TRACE) {
          _decaying_level_->tree_dump(std::cerr, "Decaying level: ", "TRACE: ");
        }
      }
    }

    if (! has_external_random ()) {
      DT_THROW_IF (! setup_.has_key ("seed"), std::logic_error,
                   "Missing 'seed' property for particle generator '" << get_name() << "' !");
      int seed = setup_.fetch_integer("seed");
      DT_THROW_IF (seed < 0, std::logic_error,
                   "Invalid seed value (>=0) for particle generator '" << get_name() << "' !");
      _seed_ = seed;
    }

    if (! has_external_random()) {
      _random_.reset(new mygsl::rng);
      _random_->init("taus2", _seed_);
    }

    _initialized_ = true;
    return;
  }

  void nuclear_decay_generator::reset()
  {
    DT_THROW_IF(! is_initialized(), std::logic_error, "Object is not initialized !");
    _initialized_ = false;
    if (_random_) {
      if (_random_->is_initialized()) {
        _random_->reset();
      }
      _random_.reset();
    }
    if (_decaying_level_) {
      _decaying_level_ = 0;
    }
    if (_ndm_) {
      _ndm_.reset();
    }
    return;
  }

  bool nuclear_decay_generator::has_next()
  {
    return true;
  }

  bool nuclear_decay_generator::is_initialized() const
  {
    return _initialized_;
  }

  unsigned long nuclear_decay_generator::get_seed() const
  {
    return _seed_;
  }

  bool nuclear_decay_generator::can_external_random() const
  {
    return true;
  }

  const mygsl::rng & nuclear_decay_generator::get_random() const
  {
    if (has_external_random()) {
      return get_external_random();
    }
    return *_random_;
  }

  mygsl::rng & nuclear_decay_generator::grab_random()
  {
    if (has_external_random()) {
      return grab_external_random();
    }
    return *_random_;
  }

  void nuclear_decay_generator::_load_next(primary_event & event_,
                                           bool compute_classification_)
  {
    DT_LOG_TRACE(get_logging_priority(), "Entering...");
    DT_THROW_IF (! _initialized_, std::logic_error, "Generator is not locked/initialized !");
    event_.reset();
    event_.set_time(0.0);

    const nuclear_level * current_level = _decaying_level_;
    while (current_level) {
      DT_LOG_TRACE(get_logging_priority(),
                   "Decaying the level '" << current_level->to_string() << "'");

      double prob_level = grab_random().flat(0.0, 1.0);
      const nuclear_decay * current_decay = 0;
      for (size_t ichannel = 0;
           ichannel < current_level->get_number_of_decay_channels();
           ichannel++) {
        const nuclear_level::decay_channel & channel
          = current_level->get_decay_channel(ichannel);
        if (prob_level <= channel.get_cumul_prob()) {
          current_decay = &channel.get_decay();
          break;
        }
      }

      if (current_decay) {
        DT_LOG_TRACE(get_logging_priority(),
                     "Processing the decay '" << current_decay->to_string() << "'");
        base_decay_driver & driver = const_cast<nuclear_decay*>(current_decay)->grab_decay_driver();
        if (get_logging_priority() >= datatools::logger::PRIO_TRACE) {
          driver.tree_dump(std::cerr, "Decay driver: ", "TRACE: ");
        }
        int err = driver.fill(grab_random(), event_);
        DT_THROW_IF (err != 0, std::logic_error,"Decay failed!");

        current_level = &current_decay->get_level_final();
        if (current_level->is_stable()) {
          DT_LOG_TRACE(get_logging_priority(),
                       "Final level '" << current_level->to_string() << "' is stable.");
          current_level = 0;
        }
      } else {
        current_level = 0;
      }

    } // while

    event_.set_label(i_genbb::get_name());
    if (compute_classification_) {
      event_.compute_classification();
    }

    DT_LOG_TRACE(get_logging_priority(), "Exiting.");
    return;
  }

} // end of namespace genbb
