// combined_particle_generator.cc
/*
 * Copyright 2007-2011 F. Mauger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Publi * License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Publi * License for more details.
 *
 * You should have received a copy of the GNU General Publi * License
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

  bool combined_particle_generator::is_initialized() const
  {
    return _initialized_;
  }

  void combined_particle_generator::set_mode(int mode_)
  {
    _mode_ = mode_;
    return;
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
    _mode_ = MODE_PLAIN_PROBABILITY;
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

    if (config_.has_key("mode")) {
      std::string mode_str = config_.fetch_string("mode");
      if (mode_str == "plain_probability") {
        set_mode(MODE_PLAIN_PROBABILITY);
      } else if (mode_str == "activity") {
        set_mode(MODE_ACTIVITY);
      } else if (mode_str == "timing") {
        set_mode(MODE_TIMING);
      } else {
        DT_THROW_IF(true, std::logic_error,"Unknown mode '" << mode_str << "' !");
      }
    }

    double activity_unit = 1. * CLHEP::becquerel;
    if (is_mode_activity()) {
      if (config_.has_key("activity_unit")) {
        std::string activity_unit_str = config_.fetch_string("activity_unit");
        activity_unit = datatools::units::get_activity_unit_from(activity_unit_str);
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
        pge.prob = 0.0;
        pge.cumul_prob = 0.0;
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

        // Extract probability associated to the PG contribution :
        if (is_mode_plain_probability()) {
          // Plain probability mode :
          std::ostringstream prob_prop_key_oss;
          prob_prop_key_oss << "generators." << pg_label << ".probability";
          std::string prob_prop_key = prob_prop_key_oss.str();
          DT_THROW_IF(! config_.has_key(prob_prop_key),
                       std::logic_error,
                       "Missing probability property '" << prob_prop_key << "' for particle generator with label '"
                       << pg_label << "' !");
          double pg_prob =  config_.fetch_real(prob_prop_key);
          DT_THROW_IF(pg_prob < 0.0,
                       std::logic_error,
                       "Invalid probability value for particle generator with label '"
                       << pg_label << "' (" << pg_prob << ") !");
          pge.prob = pg_prob;
        } // if (is_mode_plain_probability()) {
        if (is_mode_activity()) {
          // Activity mode :
          std::ostringstream act_prop_key_oss;
          act_prop_key_oss << "generators." << pg_label << ".activity";
          std::string act_prop_key = act_prop_key_oss.str();
          DT_THROW_IF(! config_.has_key(act_prop_key),
                       std::logic_error,
                       "Missing activity property '" << act_prop_key << "' for particle generator '"
                       << pg_name << "' !");
          double pg_act =  config_.fetch_real(act_prop_key);
          if (! config_.has_explicit_unit(act_prop_key)) pg_act *= activity_unit;
          /*
            std::string pg_act_str = config_.fetch_string(act_prop_key);
            std::string unit_label;
            if (! datatools::units::find_value_with_unit(pg_act_str,
            pg_act,
            unit_label,
            activity_unit))
            {
            std::ostringstream message;
            message << "genbb::combined_particle_generator::initialize: "
            << "Cannot parse activity value with its unit for particle generator '"
            << pg_label << "' (" << pg_act_str << ") !";
            th row std::logic_error(message.str());
            }
            if (! unit_label.empty() && unit_label != "activity")
            {
            std::ostringstream message;
            message << "genbb::combined_particle_generator::initialize: "
            << "Invalid unit ('" << unit_label << "') for the activity of the particle generator '"
            << pg_label << "' (" << pg_act << ") !";
            th row std::logic_error(message.str());
            }
          */
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
                 "Generator is notlocked/initialized !");
    event_.reset();

    double prob = grab_random().flat(0.0, 1.0);
    //std::cerr << "DEVEL: " << "prob=" << prob << '\n';
    int pg_index = -1;
    for (size_t i = 0; i < _generators_info_.size(); i++) {
      if (prob < _generators_info_[i].cumul_prob) {
        pg_index = i;
        //std::cerr << "DEVEL: " << "  pg_index=" << pg_index << '\n';
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

    DT_LOG_TRACE(get_logging_priority(),"Exiting.");
    return;
  }

  void combined_particle_generator::_at_init_()
  {
    if (! has_external_random()) {
      DT_LOG_NOTICE(get_logging_priority(),
                    "Initializing embeded PRNG...");
      _random_.init("taus2", _seed_);
    }

    DT_THROW_IF(_generators_info_.size() == 0, std::logic_error,
                 "No particle generator was defined in the combo !");

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
    out_ << indent_ << "|-- " << "Embeded PRNG  : "
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
      out_ << " prob=" << e.prob << " (";
      int prec = out_.precision();
      out_.precision(4);
      out_ << e.prob*100 << "%" << ") and  cumulative prob=";
      out_.precision(7);
      out_ << e.cumul_prob
           << std::endl;
      out_.precision(prec);
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
                               "from a weighted combination of external particle generators. \n"
                               "It can be typically initialized from an ASCII configuration  \n"
                               "file. The generator can be given an external PRNG provided   \n"
                               "by the user, a manager or use its own embedded PRNG.         \n"
                              );

  ocd_.set_configuration_hints("Here are some typical configurations:                        \n"
                                "                                                            \n"
                                "***Example 1***                                             \n"
                                "                                                            \n"
                                "Combination of 3 generators with plain probability          \n"
                                "weight::                                                    \n"
                                "                                                            \n"
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
                                );

  ocd_.set_validation_support(false);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()

DOCD_CLASS_SYSTEM_REGISTRATION(genbb::combined_particle_generator,
                               "genbb::combined_particle_generator")
