// time_slicer_generator.cc
/*
 * Copyright 2018 F. Mauger
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
#include <genbb_help/time_slicer_generator.h>

// Third party:
// - Bayeux/datatools:
// #include <datatools/utils.h>
#include <datatools/units.h>
#include <datatools/exception.h>
// - Bayeux/geomtools:
#include <geomtools/utils.h>

// This project:
#include <genbb_help/primary_particle.h>
#include <genbb_help/manager.h>

namespace genbb {

  GENBB_PG_REGISTRATION_IMPLEMENT(time_slicer_generator,"genbb::time_slicer_generator")

  bool time_slicer_generator::has_mode() const
  {
    return _mode_ != SM_UNDEF;
  }

  void time_slicer_generator::set_mode(const slice_mode_type mode_)
  {
    _mode_ = mode_;
    return;
  }

  time_slicer_generator::slice_mode_type
  time_slicer_generator::get_mode() const
  {
    return _mode_;
  }

  bool time_slicer_generator::has_generator() const
  {
    return _pg_ != nullptr;
  }

  void time_slicer_generator::set_generator(i_genbb & gen_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Generator is already initialized!");
    DT_THROW_IF(!gen_.is_initialized(), std::logic_error, "Wrapped generator is not initialized!");
    _pg_ = &gen_;
    return;
  }

  i_genbb & time_slicer_generator::grab_generator()
  {
    DT_THROW_IF(_pg_ == nullptr, std::logic_error, "Generator is not set!");
    return *_pg_;
  }

  const i_genbb & time_slicer_generator::get_generator() const
  {
    DT_THROW_IF(_pg_ == nullptr, std::logic_error, "Generator is not set!");
    return *_pg_;
  }

  double time_slicer_generator::get_time_threshold() const
  {
    return _time_threshold_;
  }

  void time_slicer_generator::set_time_threshold(const double t_)
  {
    _time_threshold_ = t_;
    return;
  }

  bool time_slicer_generator::has_time_threshold() const
  {
    return datatools::is_valid(_time_threshold_);
  }

  double time_slicer_generator::get_time_cut() const
  {
    return _time_cut_;
  }

  void time_slicer_generator::set_time_cut(const double t_)
  {
    _time_cut_ = t_;
    return;
  }

  bool time_slicer_generator::has_time_cut() const
  {
    return datatools::is_valid(_time_cut_);
  }

  void time_slicer_generator::set_record_original_event_id(bool flag_)
  {
    _record_original_event_id_ = flag_;
    return;
  }

  bool time_slicer_generator::is_record_original_event_id() const
  {
    return _record_original_event_id_;
  }

  bool time_slicer_generator::can_external_random() const
  {
    return true;
  }

  time_slicer_generator::time_slicer_generator()
  {
    _initialized_ = false;
    _pg_ = nullptr;
    _time_threshold_ = datatools::invalid_real();
    _time_cut_ = datatools::invalid_real();
    _mode_ = SM_UNDEF;
    return;
  }

  time_slicer_generator::~time_slicer_generator()
  {
    if (_initialized_) {
      reset();
    }
    return;
  }

  void time_slicer_generator::_at_reset_()
  {
    _original_event_counter_ = 0;
    _buffer_event_.reset();
    _time_threshold_ = datatools::invalid_real();
    _time_cut_ = datatools::invalid_real();
    _mode_ = SM_UNDEF;
    _pg_ = nullptr;
    _record_original_event_id_ = true;
    return;
  }

  void time_slicer_generator::_at_init_()
  {
    DT_THROW_IF(_mode_ == SM_UNDEF, std::logic_error, "Mode is not set!");
    DT_THROW_IF(!has_generator(), std::logic_error, "Generator is not set!");
    // DT_THROW_IF(!_pg_->is_initialized(), std::logic_error, "Wrapped generator is not initilized!");
    DT_THROW_IF(!has_time_threshold(), std::logic_error, "Missing time threshold!");
    DT_THROW_IF(has_time_cut() && _time_cut_ <= _time_threshold_,
                std::logic_error, "Invalid time cut!");

    return;
  }

  bool time_slicer_generator::is_initialized() const
  {
    return _initialized_;
  }

  void time_slicer_generator::initialize(const datatools::properties & config_,
                                         datatools::service_manager & service_manager_,
                                         detail::pg_dict_type & dictionary_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Generator is already initialized!");
    i_genbb::_initialize_base(config_);
    DT_LOG_TRACE_ENTERING(get_logging_priority());

    if (!has_generator()) {
      if (config_.has_key("generator")) {
        std::string pg_name = config_.fetch_string("generator");
        detail::pg_dict_type::iterator found = dictionary_.find(pg_name);
        DT_THROW_IF(found == dictionary_.end(), std::logic_error,
                    "No particle generator named '" << pg_name << "' is defined!");
        detail::pg_entry_type & pet = found->second;
        // Initialize the entry :
        if (! pet.is_initialized()) {
          DT_LOG_DEBUG(get_logging_priority(), "Wrapped generator '" << pg_name << "' is not initialized...");
          datatools::factory_register<i_genbb> * facreg = nullptr;
          // Check if a manager can provide a factory object :
          if (pet.has_manager()) {
            facreg = &pet.grab_manager().grab_factory_register();
          }
          DT_LOG_DEBUG(get_logging_priority(),
                       "External random : " << std::boolalpha << has_external_random());
         detail::initialize(pet,
                             &service_manager_,
                             &dictionary_,
                             facreg,
                             (has_external_random()? &grab_external_random() : nullptr));
        }
        set_generator(pet.grab());
      }
    }

    if (!has_time_threshold()) {
      if (config_.has_key("time_threshold")) {
        double time_threshold = config_.fetch_real_with_explicit_dimension("time_threshold", "time");
        set_time_threshold(time_threshold);
      }
    }

    if (!has_time_cut()) {
      if (config_.has_key("time_cut")) {
        double time_cut = config_.fetch_real_with_explicit_dimension("time_cut", "time");
        set_time_cut(time_cut);
      }
    }

    if (!has_mode()) {
      if (config_.has_key("mode")) {
        std::string mode = config_.fetch_string("mode");
        if (mode == "prompt_event_only") {
          set_mode(SM_PROMPT_EVENT_ONLY);
        } else if (mode == "delayed_event_only") {
          set_mode(SM_DELAYED_EVENT_ONLY);
        } else if (mode == "both_events") {
          set_mode(SM_BOTH_EVENTS);
        } else {
          DT_THROW(std::logic_error,
                   "Unknown event slice mode '" << mode << "'!");
        }
      }
    }

    if (config_.has_key("record_original_event_id")) {
      bool flag = config_.fetch_boolean("record_original_event_id");
      set_record_original_event_id(flag);
    }

    _at_init_();
    _initialized_  = true;
    DT_LOG_TRACE_EXITING(get_logging_priority());
    return;
  }

  void time_slicer_generator::reset()
  {
    DT_THROW_IF(! is_initialized(), std::logic_error, "Generator is not initialized!");
    _initialized_ = false;
    _at_reset_();
    return;
  }

  bool time_slicer_generator::has_next()
  {
    DT_THROW_IF(! is_initialized(), std::logic_error, "Generator is not initialized!");
    if (_buffer_event_.is_valid()) {
      return true;
    }
    return _pg_->has_next();
  }

  void time_slicer_generator::_load_next(primary_event & event_,
                                         bool compute_classification_)
  {
    DT_LOG_TRACE_ENTERING(get_logging_priority());
    event_.reset();

    if (_buffer_event_.is_valid()) {
      // We check is a buffered event is stored and use it if available:
      event_ = _buffer_event_;
      _buffer_event_.reset();
    } else {
      // Else, we shoot a new event from the reference generator:
      primary_event working_event;
      bool need_new_event = true;
      while (need_new_event) {
        _pg_->load_next(working_event, false);
        double first_delayed_time = datatools::invalid_real();
        primary_event prompt_event;
        primary_event delayed_event;
        if (_record_original_event_id_) {
          int original_event_id = (int) _original_event_counter_;
          prompt_event.grab_auxiliaries().store(primary_event::original_event_id_key(),
						original_event_id);
          delayed_event.grab_auxiliaries().store(primary_event::original_event_id_key(),
						 original_event_id);
        }
        _original_event_counter_++;
        for (std::size_t ipart = 0; ipart < working_event.get_number_of_particles(); ipart++) {
          primary_particle & parti = working_event.grab_particle(ipart);
          double part_time = parti.get_time();
          if (part_time >= _time_cut_) {
            continue;
          }
          bool prompt = true;
          if (part_time >= _time_threshold_) {
            prompt = false;
            if (!datatools::is_valid(first_delayed_time) || part_time < first_delayed_time) {
              first_delayed_time = part_time;
            }
          }
          // Particles registration:
          if (prompt) {
            if (_mode_ == SM_PROMPT_EVENT_ONLY || _mode_ == SM_BOTH_EVENTS) {
              prompt_event.add_particle(parti);
            }
          } else {
            if (_mode_ == SM_DELAYED_EVENT_ONLY || _mode_ == SM_BOTH_EVENTS) {
              delayed_event.add_particle(parti);
            }
          }
        } // end of particle loop
  
        double ref_time = 0.0;
        // We ignore event time information:
        // if (working_event.has_time()) {
        //   ref_time = working_event.get_time();
        // }
        
        // Specific processing of the extracted prompt event:
        if (prompt_event.get_number_of_particles() > 0) {
          prompt_event.set_time(ref_time); // Force prompt event's time to zero
          if (working_event.has_vertex()) {
            prompt_event.set_vertex(working_event.get_vertex());
          }
          if (working_event.has_label()) {
            prompt_event.set_label(working_event.get_label());
          }
          if (!working_event.is_genbb_weighted()) {
            prompt_event.set_genbb_weight(working_event.get_genbb_weight());
          }
        }

        // Specific processing of the extracted delayed event:
        if (delayed_event.get_number_of_particles() > 0) {
          delayed_event.set_time(ref_time); // Force delayed event's time to zero
          delayed_event.shift_particles_time(-first_delayed_time); // Shift delayed particles 
          if (working_event.has_vertex()) {
            delayed_event.set_vertex(working_event.get_vertex());
          }
          if (working_event.has_label()) {
            delayed_event.set_label(working_event.get_label());
          }
          if (!working_event.is_genbb_weighted()) {
            delayed_event.set_genbb_weight(working_event.get_genbb_weight());
          }
        }
        
        if (prompt_event.get_number_of_particles() > 0) {
          // Deliver the prompt event:
          event_ = prompt_event;
          if (delayed_event.get_number_of_particles() > 0) {
            // Save the delayed event in the buffered event:
            _buffer_event_ = delayed_event;
          }
          need_new_event = false;
        } else if (delayed_event.get_number_of_particles() > 0) {
          // Deliver the delayed event:
          event_ = delayed_event;
          need_new_event = false;
        }
      } // end of event
    }
    if (compute_classification_) {
      event_.compute_classification();
    }
    DT_LOG_TRACE_EXITING(get_logging_priority());
    return;
  }

} // end of namespace genbb

/***************
 * OCD support *
 ***************/

#include <datatools/ocd_macros.h>

// OCD support for class '::genbb::time_slicer_generator' :
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(::genbb::time_slicer_generator,ocd_)
{
  ocd_.set_class_name("genbb::time_slicer_generator");
  ocd_.set_class_description("Time slicer generator");
  ocd_.set_class_library("genbb_help");
  ocd_.set_class_documentation("The ``genbb::time_slicer_generator`` enables to          \n"
                               "split a primary event in two primary events with respect \n"
                               "to the emission time of primary particles.               \n"
                              );

  ::genbb::i_genbb::base_initialize_ocd(ocd_);

  ocd_.set_configuration_hints("Here are some typical configurations:                        \n"
                               "                                                             \n"
                               "                                                             \n"
                               "***Example 1***                                              \n"
                               "                                                             \n"
                               "A generator that splits prompt and delayed parts of the      \n"
                               "Bi-214 decay events:                                         \n"
                               "::                                                           \n"
                               "  logging.priority : string = \"fatal\"                      \n"
                               "  generator : string = \"Bi214\"                             \n"
                               "  mode : string = \"both_events\"                            \n"
                               "  time_threshold : real as time = 400 ns                     \n"
                               "  time_cut : real as time = 4 second                         \n"
                               "                                                             \n"
                               "                                                             \n"
                               "***Example 2***                                              \n"
                               "                                                             \n"
                               "A generator that splits prompt and delayed parts of the      \n"
                               "Bi-207 decay events:                                         \n"
                               "::                                                           \n"
                               "  logging.priority : string = \"fatal\"                      \n"
                               "  generator : string = \"Bi207\"                             \n"
                               "  mode : string = \"both_events\"                            \n"
                               "  time_threshold : real as time = 10 us                      \n"
                               "  time_cut : real as time = 2 ms                             \n"
                               "                                                             \n"
                               );

  ocd_.set_validation_support(false);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()

DOCD_CLASS_SYSTEM_REGISTRATION(genbb::time_slicer_generator,
                               "genbb::time_slicer_generator")
