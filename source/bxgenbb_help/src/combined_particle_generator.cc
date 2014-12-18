// combined_particle_generator.cc
/*
 * Copyright 2007-2014 F. Mauger, X. Garrido
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
#include <genbb_help/combined_particle_generator.h>

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
// - Bayeux/geomtools:
#include <geomtools/utils.h>

// This project:
#include <genbb_help/primary_event.h>
#include <genbb_help/manager.h>

namespace genbb {

  GENBB_PG_REGISTRATION_IMPLEMENT(combined_particle_generator,"genbb::combined_particle_generator");

  combined_particle_generator::entry_type::entry_type()
  {
    time_mode = TIME_DELAY_UNDEFINED;
    datatools::invalidate(time);
    datatools::invalidate(time_width);
    datatools::invalidate(prob);
    datatools::invalidate(cumul_prob);
    pg = 0;
    return;
  }

  bool combined_particle_generator::entry_type::is_valid() const
  {
    if (!pg) return false;
    if (time_mode != TIME_DELAY_UNDEFINED) {
      // Time mode
      if (time_mode == TIME_DELAY_FIXED) {
        if (!datatools::is_valid(time)) {
          return false;
        }
      } else if (time_mode == TIME_DELAY_DECAY) {
        if (!datatools::is_valid(time)) {
          return false;
        }
      } else if (time_mode == TIME_DELAY_RANGE) {
        if (!datatools::is_valid(time)) {
          return false;
        }
        if (!datatools::is_valid(time_width)) {
          return false;
        }
      } else if (time_mode == TIME_DELAY_GAUSSIAN) {
        if (!datatools::is_valid(time)) {
          return false;
        }
        if (!datatools::is_valid(time_width)) {
          return false;
        }
      }
    } else {
      // Plain prob/activity mode
      if (!datatools::is_valid(prob)) {
        return false;
      }
      if (!datatools::is_valid(cumul_prob)) {
        return false;
      }
    }
    return true;
  }

  bool combined_particle_generator::is_initialized() const
  {
    return _initialized_;
  }

  void combined_particle_generator::set_mode(int mode_)
  {
    _mode_ = mode_;
    return;
  }

  bool combined_particle_generator::is_mode_time() const
  {
    return _mode_ == MODE_TIME;
  }

  bool combined_particle_generator::is_mode_plain_probability() const
  {
    return _mode_ == MODE_PLAIN_PROBABILITY;
  }

  bool combined_particle_generator::is_mode_activity() const
  {
    return _mode_ == MODE_ACTIVITY;
  }

  bool combined_particle_generator::can_external_random() const
  {
    return true;
  }

  combined_particle_generator::combined_particle_generator() : i_genbb()
  {
    _initialized_ = false;
    _mode_ = MODE_UNDEFINED;
    _seed_ = 0;
    return;
  }

  combined_particle_generator::~combined_particle_generator()
  {
    if (_initialized_) {
      reset();
    }
    return;
  }

  const mygsl::rng & combined_particle_generator::get_random() const
  {
    if (has_external_random()) {
      return get_external_random();
    }
    return _random_;
  }

  mygsl::rng & combined_particle_generator::grab_random()
  {
    if (has_external_random()) {
      return grab_external_random();
    }
    return _random_;
  }

  void combined_particle_generator::_at_reset_()
  {
    if (_random_.is_initialized()) {
      _random_.reset();
    }
    _seed_ = 0;
    _mode_ = MODE_PLAIN_PROBABILITY;
    return;
  }

  void combined_particle_generator::reset()
  {
    if (! _initialized_) return;
    _at_reset_();
    _initialized_ = false;
    return;
  }

  void combined_particle_generator::initialize(const datatools::properties & config_,
                                                datatools::service_manager & service_manager_,
                                                detail::pg_dict_type & dictionary_)
  {
    i_genbb::_initialize_base(config_);

    if (! has_external_random()) {
      if (config_.has_key("seed")) {
        long seed = config_.fetch_integer("seed");
        DT_THROW_IF(seed < 0, std::logic_error, "Invalid seed value (" << seed << ") !");
        _seed_ = seed;
      } else {
        DT_THROW_IF(true, std::logic_error, "Missing 'seed' property !");
      }
    }

    DT_THROW_IF(! config_.has_key("mode"), std::logic_error, "Missing generation mode !");
    const std::string mode_str = config_.fetch_string("mode");
    if (mode_str == "plain_probability") {
      set_mode(MODE_PLAIN_PROBABILITY);
    } else if (mode_str == "activity") {
      set_mode(MODE_ACTIVITY);
    } else if (mode_str == "time") {
      set_mode(MODE_TIME);
    } else {
      DT_THROW_IF(true, std::logic_error, "Unknown mode '" << mode_str << "' !");
    }

    double activity_unit = 1. * CLHEP::becquerel;
    if (is_mode_activity()) {
      if (config_.has_key("activity_unit")) {
        std::string activity_unit_str = config_.fetch_string("activity_unit");
        activity_unit = datatools::units::get_activity_unit_from(activity_unit_str);
      }
    }

    double time_unit = 1. * CLHEP::ns;
    if (is_mode_time()) {
      if (config_.has_key("time_unit")) {
        std::string time_unit_str = config_.fetch_string("time_unit");
        time_unit = datatools::units::get_time_unit_from(time_unit_str);
      }
    }

    if (config_.has_key("generators.labels")) {
      std::vector<std::string> pg_labels;
      config_.fetch("generators.labels", pg_labels);
      for (size_t i = 0; i < pg_labels.size(); i++) {
        const std::string & pg_label = pg_labels[i];
        // Extract the name of the PG contribution with given label :
        std::ostringstream pg_name_key_oss;
        pg_name_key_oss << "generators." << pg_label << ".name";
        std::string pg_name_key = pg_name_key_oss.str();
        DT_THROW_IF(! config_.has_key(pg_name_key), std::logic_error,
                     "Missing particle generator name property '" << pg_name_key << "' for contribution with label '"
                     << pg_label << "' !");
        std::string pg_name = config_.fetch_string(pg_name_key);
        detail::pg_dict_type::iterator found = dictionary_.find(pg_name);
        DT_THROW_IF(found == dictionary_.end(), std::logic_error,
                     "No particle generator named '" << pg_name << "' for contribution with label '"
                     << pg_label << "' !");
        // Add an entry :
        {
          entry_type dummy;
          _generators_info_.push_back(dummy);
        }
        entry_type & pge = _generators_info_.back();
        pge.label = pg_label;
        pge.name = pg_name;
        if (is_mode_plain_probability() || is_mode_activity()) {
          pge.prob = 0.0;
          pge.cumul_prob = 0.0;
        }
        detail::pg_entry_type & pet = found->second;
        // Intialize the entry :
        if (! pet.is_initialized()) {
          datatools::factory_register<i_genbb> * facreg = 0;
          // Check if a manager can provide a factory object :
          if (pet.has_manager()) {
            facreg = &pet.grab_manager().grab_factory_register();
          }
          detail::initialize(pet,
                             &service_manager_,
                             &dictionary_,
                             facreg,
                             (has_external_random()? &grab_external_random() : 0));
        } // if (! pet.is_initialized()) {
          //pet.tree_dump(std::cerr, "genbb::combined_particle_generator::initialize: PET: ", "DEVEL: ");
        pge.pg = &(pet.grab());

        // Define base key for property
        const std::string base_key = "generators." + pg_label;

        // Extract time associated to the PG contribution :
        if (is_mode_time()) {
          time_delay_type time_mode = TIME_DELAY_UNDEFINED;
          {
            // Fetch time delay mode :
            const std::string time_mode_prop_key = base_key + ".time_mode";
            DT_THROW_IF(! config_.has_key(time_mode_prop_key),
                        std::logic_error,
                        "Missing time mode property '" << time_mode_prop_key << "' for particle generator with label '"
                        << pg_label << "' !");
            const std::string time_mode_prop = config_.fetch_string(time_mode_prop_key);
            if (time_mode_prop == "fixed") {
              time_mode = TIME_DELAY_FIXED;
            } else if (time_mode_prop == "decay") {
              time_mode = TIME_DELAY_DECAY;
            } else if (time_mode_prop == "range") {
              time_mode = TIME_DELAY_RANGE;
            } else if (time_mode_prop == "gaussian") {
              time_mode = TIME_DELAY_GAUSSIAN;
            }
          }
          if (time_mode == TIME_DELAY_UNDEFINED) {
            time_mode = TIME_DELAY_DEFAULT;
          }
          pge.time_mode = time_mode;

          double time; datatools::invalidate(time);
          if (pge.time_mode == TIME_DELAY_FIXED) {
            // Fetch fixed time
            const std::string time_prop_key = base_key + ".fixed_time";
            DT_THROW_IF(! config_.has_key(time_prop_key),
                        std::logic_error,
                        "Missing time property '" << time_prop_key << "' for particle generator with label '"
                        << pg_label << "' !");
            time = config_.fetch_real(time_prop_key);
            DT_THROW_IF(time < 0.0,
                        std::domain_error,
                        "Invalid fixed time delay for particle generator with label '"
                        << pg_label << "' !");
            if (! config_.has_explicit_unit(time_prop_key)) time *= time_unit;
          }

          if (pge.time_mode == TIME_DELAY_DECAY) {
            std::string time_prop_key;
            if (config_.has_key(time_prop_key = base_key + ".mean_lifetime")) {
              time = config_.fetch_real(time_prop_key);
              if (! config_.has_explicit_unit(time_prop_key)) time *= time_unit;
            } else if (config_.has_key(time_prop_key = base_key + ".half_life")) {
              time = config_.fetch_real(time_prop_key);
              if (! config_.has_explicit_unit(time_prop_key)) time *= time_unit;
              time /= std::log(2);
            }
            DT_THROW_IF (!datatools::is_valid(time), std::logic_error,
                         "Missing decay mean lifetime/half-life property for particle generator with label '"
                         << pg_label << "' !" );
            DT_THROW_IF (time <= 0.0,
                         std::domain_error,
                         "Invalid decay mean lifetime for particle generator with label '"
                         << pg_label << "' !");
          }
          pge.time = time;
        }

        if (pge.time_mode == TIME_DELAY_RANGE) {
          std::string time_prop_key;
          // Fetch min delay time
          double min_time = 0.0;
          if (config_.has_key(time_prop_key = base_key + ".min_time")) {
            min_time = config_.fetch_real(time_prop_key);
            DT_THROW_IF(min_time < 0.0,
                        std::domain_error,
                        "Invalid min delay time property for particle generator with label '"
                        << pg_label << "' !");
            if (! config_.has_explicit_unit(time_prop_key)) min_time *= time_unit;
          }
          // Fetch max delay time
          DT_THROW_IF(!config_.has_key(time_prop_key = base_key + ".max_time"),
                      std::logic_error,
                      "Missing max delay time property for particle generator with label '"
                      << pg_label << "' !" );
          double max_time = config_.fetch_real(time_prop_key);
          if (! config_.has_explicit_unit(time_prop_key)) max_time *= time_unit;
          DT_THROW_IF(max_time <= min_time,
                      std::domain_error,
                      "Invalid max delay time for particle generator with label '"
                      << pg_label << "' !");
          pge.time = 0.5 * (min_time + max_time);
          pge.time_width = max_time - min_time;
        }

        if (pge.time_mode == TIME_DELAY_GAUSSIAN) {
          std::string time_prop_key;
          // Fetch mean time
          DT_THROW_IF(!config_.has_key(time_prop_key = base_key + ".mean_time"),
                      std::logic_error,
                      "Missing mean delay time property for particle generator with label '"
                      << pg_label << "' !" );
          double mean_time = config_.fetch_real(time_prop_key);
          if (! config_.has_explicit_unit(time_prop_key)) mean_time *= time_unit;
          DT_THROW_IF(mean_time < 0.0,
                      std::domain_error,
                      "Invalid mean delay time for particle generator with label '"
                       << pg_label << "' !");
          DT_THROW_IF(!config_.has_key(time_prop_key = base_key + ".sigma_time"),
                      std::logic_error,
                      "Missing sigma delay time property for particle generator with label '"
                      << pg_label << "' !" );
          double sigma_time = config_.fetch_real(time_prop_key);
          if (! config_.has_explicit_unit(time_prop_key)) sigma_time *= time_unit;
          DT_THROW_IF(sigma_time <= 0.0,
                      std::domain_error,
                      "Invalid sigma delay time for particle generator with label '"
                      << pg_label << "' !");
          pge.time = mean_time;
          pge.time_width = sigma_time;
        } // if (is_mode_time())

        // Extract probability associated to the PG contribution :
        if (is_mode_plain_probability()) {
          // Plain probability mode :
          const std::string prob_prop_key = base_key + ".probability";
          DT_THROW_IF(! config_.has_key(prob_prop_key),
                      std::logic_error,
                      "Missing probability property '" << prob_prop_key << "' for particle generator with label '"
                      << pg_label << "' !");
          double pg_prob = config_.fetch_real(prob_prop_key);
          DT_THROW_IF(pg_prob < 0.0,
                      std::logic_error,
                      "Invalid probability value for particle generator with label '"
                      << pg_label << "' (" << pg_prob << ") !");
          pge.prob = pg_prob;
        } // if (is_mode_plain_probability())

        if (is_mode_activity()) {
          // Activity mode :
          const std::string act_prop_key = base_key + ".activity";
          DT_THROW_IF(! config_.has_key(act_prop_key),
                      std::logic_error,
                      "Missing activity property '" << act_prop_key << "' for particle generator '"
                      << pg_name << "' !");
          double pg_act = config_.fetch_real(act_prop_key);
          if (! config_.has_explicit_unit(act_prop_key)) pg_act *= activity_unit;
          DT_THROW_IF(pg_act < 0.0, std::logic_error,
                      "Invalid activity value for particle generator '"
                      << pg_label << "' (" << pg_act << ") !");
          pge.prob = pg_act;
        } // if (is_mode_activity())
      }
    }
    _at_init_();
    _initialized_  = true;
    return;
  }

  bool combined_particle_generator::has_next()
  {
    return true;
  }

  void combined_particle_generator::_load_next(primary_event & event_,
                                               bool compute_classification_)
  {
    DT_LOG_TRACE(get_logging_priority(),"Entering...");
    DT_THROW_IF(! _initialized_, std::logic_error,
                 "Generator is not initialized !");
    event_.reset();

    if (is_mode_time()) {
      // Randomize the cascading generators:
      double total_delay_time = 0.0;
      for (size_t ig = 0; ig < _generators_info_.size(); ++ig) {
        // Compute a random delay time for the new event in the cascade:
        double time_delay;
        datatools::invalidate(time_delay);
        if (_generators_info_[ig].time_mode == TIME_DELAY_FIXED) {
          time_delay = _generators_info_[ig].time;
        } else if (_generators_info_[ig].time_mode == TIME_DELAY_DECAY) {
          time_delay = grab_random().exponential(1.0/_generators_info_[ig].time);
        } else if (_generators_info_[ig].time_mode == TIME_DELAY_RANGE) {
          const double tmin = _generators_info_[ig].time - 0.5 * _generators_info_[ig].time_width;
          const double tmax = tmin + _generators_info_[ig].time_width;
          time_delay = grab_random().flat(tmin, tmax);
        } else if (_generators_info_[ig].time_mode == TIME_DELAY_GAUSSIAN) {
          time_delay = -1.0;
          int count = 0;
          while (time_delay < 0.0) {
            time_delay = grab_random().gaussian(_generators_info_[ig].time, _generators_info_[ig].time_width);
            count++;
            DT_THROW_IF(count > 1000, std::logic_error,
                        "Von-Neumann loop overflow for generator '" << _generators_info_[ig].name << "'");
          }
        }
        DT_THROW_IF(! datatools::is_valid(time_delay), std::logic_error, "Time delay is still invalid !");
        total_delay_time += time_delay;
        DT_LOG_TRACE(get_logging_priority(), "Generating '" << _generators_info_[ig].name << "'");
        primary_event an_event;
        _generators_info_[ig].pg->load_next(an_event, false);
        for (size_t ip = 0; ip < an_event.get_number_of_particles(); ++ip) {
          primary_particle & pp = an_event.grab_particle(ip);
          pp.shift_time(total_delay_time);
          event_.add_particle(pp);
        }
      }
      event_.set_label(i_genbb::get_name());
    }

    if (is_mode_plain_probability() || is_mode_activity()) {
      const double prob = grab_random().flat(0.0, 1.0);
      int pg_index = -1;
      for (size_t i = 0; i < _generators_info_.size(); i++) {
        if (prob < _generators_info_[i].cumul_prob) {
          pg_index = i;
          break;
        }
      }
      if (pg_index < 0) pg_index = _generators_info_.size() - 1;
      DT_THROW_IF(!_generators_info_[pg_index].pg->has_next(),
                  std::logic_error,
                  "Particle generator '"
                  << _generators_info_[pg_index].name << "' has no more event !");
      _generators_info_[pg_index].pg->load_next(event_,compute_classification_);
      std::ostringstream label_oss;
      label_oss << _generators_info_[pg_index].name;
      event_.set_label(label_oss.str());
    }

    if (compute_classification_) {
      event_.compute_classification();
    }
    DT_LOG_TRACE(get_logging_priority(),"Exiting.");
    return;
  }

  void combined_particle_generator::_at_init_()
  {
    if (! has_external_random()) {
      DT_LOG_NOTICE(get_logging_priority(),
                    "Initializing embedded PRNG...");
      _random_.init("taus2", _seed_);
    }

    DT_THROW_IF(_generators_info_.size() == 0, std::logic_error,
                 "No particle generator was defined in the combo !");

    if (is_mode_plain_probability() || is_mode_activity()) {
      // Compute the sum of probabilities :
      double cumul = 0.0;
      for (size_t i = 0; i < _generators_info_.size(); i++) {
        entry_type & e = _generators_info_[i];
        cumul += e.prob;
        e.cumul_prob = cumul;
      }
      // Normalize probabilities :
      for (size_t i = 0; i < _generators_info_.size(); i++) {
        entry_type & e = _generators_info_[i];
        e.prob /= cumul;
        e.cumul_prob /= cumul;
      }
    }

    //dump(std::clog, "genbb::combined_particle_generator::_at_init_: ", "NOTICE: ");
    return;
  }

  void combined_particle_generator::dump(std::ostream & out_,
                                         const std::string & title_,
                                         const std::string & indent_) const
  {
    if (!title_.empty()) {
      out_ << indent_ << title_ << std::endl;
    }
    bool inherited = false;
    std::string tag = "|-- ";
    std::string ctag = "`-- ";
    if (inherited) {
      tag = "    ";
      ctag = "`-- ";
    }
    out_ << indent_ << "|-- " << "Mode          : "  << _mode_ << std::endl;
    out_ << indent_ << "|-- " << "External PRNG : "  << (has_external_random() ? "Yes": "No") << std::endl;
    out_ << indent_ << "|-- " << "Embedded PRNG  : "
         << (!has_external_random() && _random_.is_initialized() ? "Yes": "No") << std::endl;
    out_ << indent_ << "`-- " << "Generators    : "
         << "[" << _generators_info_.size() << "]"
         << std::endl;
    for (size_t i = 0; i < _generators_info_.size(); i++) {
      const entry_type & e = _generators_info_[i];
      out_ << indent_ << "    ";
      if (i+1 != _generators_info_.size()) {
        out_ << "|-- ";
      } else {
        out_ << "`-- ";
      }
      out_ << "'" << e.label << "' as name='" << e.name << "' with : ";
      if (e.time_mode == TIME_DELAY_UNDEFINED) {
        out_ << " prob=" << e.prob << " (";
        int prec = out_.precision();
        out_.precision(4);
        out_ << e.prob*100 << "%" << ") and  cumulative prob=";
        out_.precision(7);
        out_ << e.cumul_prob
             << std::endl;
        out_.precision(prec);
      } else {
        out_ << " time_mode=" << e.time_mode << " with time=" << e.time / CLHEP::second << " s ";
        if (datatools::is_valid(e.time_width)) {
          out_ << " and time_width=" << e.time_width / CLHEP::second << " s ";
        }
        out_ << std::endl;
      }
    }
    return;
  }

} // end of namespace genbb

/***************
 * OCD support *
 ***************/

#include <datatools/ocd_macros.h>

// OCD support for class '::genbb::combined_particle_generator' :
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(::genbb::combined_particle_generator,ocd_)
{
  ocd_.set_class_name("genbb::combined_particle_generator");
  ocd_.set_class_description("A combined particle generator");
  ocd_.set_class_library("genbb_help");
  ocd_.set_class_documentation("The ``genbb::combined_particle_generator`` enables to        \n"
                               "randomize the initial kinematics of a primary event          \n"
                               "from a combination of external particle generators.          \n"
                               "It can be typically initialized from an ASCII configuration  \n"
                               "file. The generator can be given an external PRNG provided   \n"
                               "by the user, a manager or use its own embedded PRNG.         \n"
                               "                                                             \n"
                               "There are 3 combining modes:                                 \n"
                               "                                                             \n"
                               " * ``plain_probability`` : Each listed generator is given an \n"
                               "   absolute probability used as its branching ratio. The     \n"
                               "   listed generators are thus  \n"
                               "   concurrent and only one is picked-up to fire the event.   \n"
                               " * ``activity`` : Each listed generator is given an          \n"
                               "   absolute activity that is used as a branching ratio.      \n"
                               "   The listed generators are thus concurrent                 \n"
                               "   and only one is picked-up to fire the event.              \n"
                               " * ``time`` : Each listed generator is inserted in a time    \n"
                               "   *decay* cascade given a time delay between each bunch of  \n"
                               "   particles generated by each generator. The time delay can \n"
                               "   be choosen individually for each generator:                      \n"
                               "                                                                    \n"
                               "    * ``fixed`` : time delay is fixed                               \n"
                               "    * ``decay`` : time delay is randomized using an exponential     \n"
                               "      time distribution (given the half-life of the mean life time) \n"
                               "    * ``range`` : time delay is uniformly randomized within a time  \n"
                               "      interval (given the min and max time bounds)                  \n"
                               "    * ``gaussian`` : time delay is randomize using gaussian         \n"
                               "      distribution (given the mean and standard deviation).         \n"
                               "                                                                    \n"
                               "   The delay time is accumulated along the sequence of generators   \n"
                               "   in such a way generated particules are given an absolute emission\n"
                               "   time relatively to the some zero time reference.                 \n"
                               "                                                                    \n"
                               "   Full OCD support is not provided yet but configuration hints     \n"
                               "   are available. In doubt, read the source of the                  \n"
                               "   genbb::combined_particle_generator::initialize method.           \n"
                               "                                                                    \n"
                              );

  ::genbb::i_genbb::base_initialize_ocd(ocd_);

  ocd_.set_configuration_hints("Here are some typical configurations:                        \n"
                                "                                                            \n"
                                "***Example 1***                                             \n"
                                "                                                            \n"
                                "Combination of 3 generators with plain probability          \n"
                                "weight::                                                    \n"
                                "                                                            \n"
                                "  logging.priority : string = \"fatal\"                     \n"
                                "  assign_generation_ids : boolean = true                    \n"
                                "  seed : integer = 12345                                    \n"
                                "  mode : string = \"plain_probability\"                     \n"
                                "  generators.labels : string[3] = \\                        \n"
                                "    \"decay0\" \\                                           \n"
                                "    \"decay1\" \\                                           \n"
                                "    \"decay2\"                                              \n"
                                "  generators.decay0.name        : string = \"Bi214\"        \n"
                                "  generators.decay1.name        : string = \"Tl208\"        \n"
                                "  generators.decay2.name        : string = \"Co60\"         \n"
                                "  generators.decay0.probability : real = 10 %               \n"
                                "  generators.decay1.probability : real = 60 %               \n"
                                "  generators.decay2.probability : real = 30 %               \n"
                                "                                                            \n"
                                "                                                            \n"
                                "***Example 2**                                              \n"
                                "                                                            \n"
                                "Combination of 3 generators with weights given by           \n"
                                "activity::                                                  \n"
                                "                                                            \n"
                                "  seed : integer = 12345                                    \n"
                                "  mode : string = \"activity\"                              \n"
                                "  generators.labels : string[3] = \\                        \n"
                                "    \"decay0\" \\                                           \n"
                                "    \"decay1\" \\                                           \n"
                                "    \"decay2\"                                              \n"
                                "  generators.decay0.name     : string = \"Bi214\"           \n"
                                "  generators.decay1.name     : string = \"Tl208\"           \n"
                                "  generators.decay2.name     : string = \"Co60\"            \n"
                                "  generators.decay0.activity : real = 10 mBq                \n"
                                "  generators.decay1.activity : real = 60 mBq                \n"
                                "  generators.decay2.activity : real = 30 mBq                \n"
                                "                                                            \n"
                                "                                                            \n"
                                "***Example 3**                                              \n"
                                "                                                            \n"
                                "Combination of a cascade of 5 generators (from the thorium-232 chain) \n"
                                " with delay time randomized from half-lives (exponential):   \n"
                                "::                                                           \n"
                                "                                                             \n"
                                "  seed : integer = 12345                                     \n"
                                "  mode : string = \"time\"                                   \n"
                                "  generators.labels : string[5] = \\                         \n"
                                "    \"Rn220\" \\                                             \n"
                                "    \"Po216\" \\                                             \n"
                                "    \"Pb212\" \\                                             \n"
                                "    \"Bi212\" \\                                             \n"
                                "    \"Po212\"                                                \n"
                                "  generators.Rn220.name      : string = \"Rn220_alpha\"      \n"
                                "  generators.Po216.name      : string = \"Po216_alpha\"      \n"
                                "  generators.Pb212.name      : string = \"Pb212_beta_minus\" \n"
                                "  generators.Bi212.name      : string = \"Bi212_beta_minus\" \n"
                                "  generators.Po212.name      : string = \"Po212_alpha\"      \n"
                                "  generators.Rn220.time_mode : string = \"decay\"            \n"
                                "  generators.Po216.time_mode : string = \"decay\"            \n"
                                "  generators.Pb212.time_mode : string = \"decay\"            \n"
                                "  generators.Bi212.time_mode : string = \"decay\"            \n"
                                "  generators.Po212.time_mode : string = \"decay\"            \n"
                                "  generators.Rn220.half_life : real as time = 55.6 s         \n"
                                "  generators.Po216.half_life : real as time = 145 ms         \n"
                                "  generators.Pb212.half_life : real as time = 10.64 hour     \n"
                                "  generators.Bi212.half_life : real as time = 60.55 min      \n"
                                "  generators.Pb212.half_life : real as time = 0.3 us         \n"
                                "                                                             \n"
                                "                                                             \n"
                                "***Example 4**                                               \n"
                                "                                                             \n"
                                "Combination of a cascade of 2 generators with a fixed        \n"
                                "delay time between gamma particles emitted from both events: \n"
                                "::                                                           \n"
                                "                                                             \n"
                                "  seed : integer = 12345                                     \n"
                                "  mode : string = \"time\"                                   \n"
                                "  generators.labels : string[2] = \\                         \n"
                                "    \"gamma0\" \\                                            \n"
                                "    \"gamma1\"                                               \n"
                                "  generators.gamma0.name      : string = \"gamma_1MeV\"      \n"
                                "  generators.gamma1.name      : string = \"gamma_1.2MeV\"    \n"
                                "  generators.gamma0.time_mode : string = \"fixed\"           \n"
                                "  generators.gamma1.time_mode : string = \"fixed\"           \n"
                                "  generators.gamma0.fixed_time : real as time = 0.0 s        \n"
                                "  generators.gamma1.fixed_time : real as time = 1.4 ns       \n"
                                "                                                             \n"
                                 );

  ocd_.set_validation_support(false);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()

DOCD_CLASS_SYSTEM_REGISTRATION(genbb::combined_particle_generator,
                               "genbb::combined_particle_generator")
