// lorentz_boost_wrapper.cc
/*
 * Copyright 2014-2015 F. Mauger
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
#include <genbb_help/lorentz_boost_wrapper.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/logger.h>
#include <datatools/exception.h>
#include <datatools/service_manager.h>
// - Bayeux/dpp:
#include <dpp/input_module.h>

// This project:
#include <genbb_help/manager.h>
#include <genbb_help/primary_event.h>

namespace genbb {

  // Factory stuff :
  DATATOOLS_FACTORY_SYSTEM_REGISTER_IMPLEMENTATION(i_lorentz_boost_generator, "genbb::i_lorentz_boost_generator/__system__")

  i_lorentz_boost_generator::i_lorentz_boost_generator()
  {
    return;
  }

  i_lorentz_boost_generator::~i_lorentz_boost_generator()
  {
    return;
  }

  void i_lorentz_boost_generator::add_metadata(primary_event & /*pe_*/)
  {
    return;
  }

  void i_lorentz_boost_generator::initialize_simple()
  {
    datatools::properties dummy;
    initialize(dummy);
    return;
  }

  // static
  i_lorentz_boost_generator * i_lorentz_boost_generator::create(const std::string & id_,
                                                                const datatools::properties & config_)
  {
    i_lorentz_boost_generator * lbg = 0;
    datatools::factory_register<i_lorentz_boost_generator> & the_factory_register
      = DATATOOLS_FACTORY_GRAB_SYSTEM_REGISTER(i_lorentz_boost_generator);
    DT_THROW_IF (!the_factory_register.has(id_),
                 std::logic_error,
                 "Cannot find Lorentz boost generator factory with ID '" << id_ << "'!");
    const datatools::factory_register<i_lorentz_boost_generator>::factory_type & the_factory
      = the_factory_register.get(id_);
    lbg = the_factory();
    lbg->initialize(config_);
    return lbg;
  }

  void i_lorentz_boost_generator::generate(geomtools::vector_3d & speed_,
                                           geomtools::vector_3d & vtx_)
  {
    double time = datatools::invalid_real();
    _generate(speed_, vtx_, time);
    return;
  }

  void i_lorentz_boost_generator::generate(geomtools::vector_3d & speed_,
                                           geomtools::vector_3d & vtx_,
                                           double & time_)
  {
    _generate(speed_, vtx_, time_);
    return;
  }

  // Factory stuff :
  GENBB_PG_REGISTRATION_IMPLEMENT(lorentz_boost_wrapper, "genbb::lorentz_boost_wrapper")
  lorentz_boost_wrapper::lorentz_boost_wrapper() : i_genbb()
  {
    _initialized_ = false;
    _pg_ = 0;
    _lbg_ = 0;
    _lbg_owned_ = false;
    return;
  }

  lorentz_boost_wrapper::~lorentz_boost_wrapper()
  {
    if (_initialized_) {
      reset();
    }
    return;
  }

  void lorentz_boost_wrapper::set_generator(i_genbb & pg_)
  {
    DT_THROW_IF(_initialized_, std::logic_error, "Operation prohibited ! Object is already initialized !");
    DT_THROW_IF(! pg_.is_initialized(), std::logic_error, "Generator '" << pg_.get_name() << "' is not initialized !");
    _pg_ = &pg_;
    return;
  }

  void lorentz_boost_wrapper::set_lorentz_boost_generator(i_lorentz_boost_generator & lbg_)
  {
    DT_THROW_IF(_initialized_, std::logic_error, "Operation prohibited ! Object is already initialized !");
    _lbg_ = &lbg_;
    _lbg_owned_ = false;
    return;
  }

  void lorentz_boost_wrapper::set_lorentz_boost_generator(i_lorentz_boost_generator * lbg_)
  {
    DT_THROW_IF(_initialized_, std::logic_error, "Operation prohibited ! Object is already initialized !");
    _lbg_ = lbg_;
    _lbg_owned_ = true;
    return;
  }


  bool lorentz_boost_wrapper::is_initialized() const
  {
    return _initialized_;
  }

  void lorentz_boost_wrapper::reset()
  {
    DT_THROW_IF(!_initialized_, std::logic_error, "Operation prohibited ! Object is not initialized !");
    _initialized_ = false;
    if (_lbg_) {
      if (_lbg_owned_) {
        delete _lbg_;
        _lbg_owned_ = false;
      }
      _lbg_ = 0;
    }
    _pg_ = 0;
    return;
  }

  void lorentz_boost_wrapper::initialize(const datatools::properties & config_,
                                         datatools::service_manager & service_manager_,
                                         detail::pg_dict_type & dictionary_)
  {
    DT_THROW_IF(_initialized_, std::logic_error, "Operation prohibited ! Object is already initialized !");

    _initialize_base(config_);

    if (_pg_ == 0) {
      DT_THROW_IF (!config_.has_key("generator"),
                   std::logic_error,
                   "Missing 'generator' key !");

      // Fetch particle generator name:
      const std::string & pg_name = config_.fetch_string("generator");

      // Find it from the dictionary:
      detail::pg_dict_type::iterator found = dictionary_.find(pg_name);
      DT_THROW_IF (found == dictionary_.end(), std::logic_error,
                   "No particle generator named '" << pg_name << "' found !");
      detail::pg_entry_type & pet = found->second;
      if (! pet.is_initialized()) {
        datatools::factory_register<i_genbb> * facreg = 0;
        // Check if a manager can provide a factory object :
        mygsl::rng * xprng = 0;
        if (has_external_random()) {
          xprng = &grab_external_random();
        }
        if (pet.has_manager()) {
          facreg = &pet.grab_manager().grab_factory_register();
          if (xprng == 0) {
            if (pet.get_manager().has_external_prng()) {
              xprng = &pet.grab_manager().grab_external_prng();
            }
          }
        }
        detail::initialize(pet,
                           &service_manager_,
                           &dictionary_,
                           facreg,
                           xprng);
      }
      set_generator(pet.grab());
    }

    if (! _lbg_) {
      if (config_.has_key("lbg.id")) {
        const std::string & lbg_id = config_.fetch_string("lbg.id");
        datatools::properties lbg_config;
        config_.export_and_rename_starting_with(lbg_config, "lbg.config.", "");
        i_lorentz_boost_generator * lbg = i_lorentz_boost_generator::create(lbg_id, lbg_config);
        if (lbg) {
          set_lorentz_boost_generator(lbg);
        }
      }
    }

    if (! _lbg_) {
      DT_THROW(std::logic_error, "Missing Lorentz boost source!");
    }

    _initialized_  = true;
    return;
  }

  bool lorentz_boost_wrapper::has_next()
  {
    if (!_pg_) {
      return false;
    }
    if (!_lbg_) {
      return false;
    }
    return _pg_->has_next() && _lbg_->has_next();
  }

  void lorentz_boost_wrapper::_load_next(primary_event & event_,
                                         bool compute_classification_)
  {
    DT_LOG_TRACE_ENTERING(_logging_priority);
    DT_THROW_IF(_pg_ == 0, std::logic_error, "Null particle generator !");
    _pg_->load_next(event_, compute_classification_);
    if (_lbg_) {
      geomtools::vector_3d vertex;
      geomtools::vector_3d speed;
      double time;
      geomtools::invalidate(speed);
      geomtools::invalidate(vertex);
      datatools::invalidate(time);
      _lbg_->generate(speed, vertex, time);
      if (geomtools::is_valid(vertex)) {
        event_.set_vertex(vertex);
      }
      if (datatools::is_valid(time)) {
        event_.set_time(time);
      }
      for (primary_event::particles_col_type::iterator ipart = event_.grab_particles().begin();
           ipart != event_.grab_particles().end();
           ipart++) {
        primary_particle & part = *ipart;
        if (geomtools::is_valid(speed)) {
          CLHEP::HepLorentzVector q4;
          // Compute the associated 4-vector:
          part.compute_four_momentum(q4);
          CLHEP::HepLorentzVector q4_2 = CLHEP::boostOf(q4, speed / CLHEP::c_light);
          part.set_momentum(q4_2.vect());
        }
      }
      _lbg_->add_metadata(event_);
    }

    DT_LOG_TRACE_EXITING(_logging_priority);
    return;
  }

} // end of namespace genbb
