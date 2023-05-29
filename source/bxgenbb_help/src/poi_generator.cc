// poi_generator.cc
/*
 * Copyright 2023 F. Mauger
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
#include <genbb_help/poi_generator.h>

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

  GENBB_PG_REGISTRATION_IMPLEMENT(poi_generator,"genbb::poi_generator")

  std::size_t poi_generator::get_poi_size() const
  {
    return _poi_cut_.size();
  }

  poi_generator::poi_info_type poi_generator::get_poi_info(std::size_t poi_index_) const
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Invalid POI index!");   
    return _poi_cut_[poi_index_];
  }

  bool poi_generator::has_generator() const
  {
    return _pg_ != nullptr;
  }

  void poi_generator::set_generator(i_genbb & gen_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Generator is already initialized!");
    DT_THROW_IF(!gen_.is_initialized(), std::logic_error, "Wrapped generator is not initialized!");
    _pg_ = &gen_;
    return;
  }

  i_genbb & poi_generator::grab_generator()
  {
    DT_THROW_IF(_pg_ == nullptr, std::logic_error, "Generator is not set!");
    return *_pg_;
  }

  const i_genbb & poi_generator::get_generator() const
  {
    DT_THROW_IF(_pg_ == nullptr, std::logic_error, "Generator is not set!");
    return *_pg_;
  }

  void poi_generator::set_record_original_event_id(bool flag_)
  {
    _record_original_event_id_ = flag_;
    return;
  }

  bool poi_generator::is_record_original_event_id() const
  {
    return _record_original_event_id_;
  }

  bool poi_generator::can_external_random() const
  {
    return true;
  }

  poi_generator::poi_generator()
  {
    _initialized_ = false;
    _pg_ = nullptr;
    return;
  }

  poi_generator::~poi_generator()
  {
    if (_initialized_) {
      reset();
    }
    return;
  }

  void poi_generator::_at_reset_()
  {
    _original_event_counter_ = 0;
    _event_counter_ = 0;
    _buffer_event_.reset();
    _poi_cut_.clear();
    _pg_ = nullptr;
    _record_original_event_id_ = true;
    return;
  }

  void poi_generator::_at_init_()
  {
    DT_THROW_IF(_poi_cut_.size() == 0, std::logic_error, "No POI cut is set!");
    DT_THROW_IF(!has_generator(), std::logic_error, "Generator is not set!");
    _search_for_next_event_();
    return;
  }

  bool poi_generator::is_initialized() const
  {
    return _initialized_;
  }

  void poi_generator::initialize(const datatools::properties & config_,
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

    std::size_t nbPoiItems = 0; 
    if (_poi_cut_.size() == 0) {
      if (config_.has_key("poi.number_of_particules")) {
	nbPoiItems = config_.fetch_positive_integer("poi.number_of_particules");
      }
    }

    for (std::size_t i = 0; i < nbPoiItems; i++) {
      std::string part_id_key = "poi.particule_" + std::to_string(i) + ".type";
      DT_THROW_IF(not config_.has_key(part_id_key), std::logic_error,
		  "Missing particle #" << i << " type!");
      std::string partLabel = config_.fetch_string(part_id_key);
      int partType = primary_particle::particle_type_from_label(partLabel);
      DT_THROW_IF(partType == primary_particle::PARTICLE_UNDEFINED,
		  std::logic_error,
		  "Invalid particle label " << partLabel << "!");
      std::string part_min_energy_key = "poi.particule_" + std::to_string(i) + ".min_energy";
      std::string part_max_energy_key = "poi.particule_" + std::to_string(i) + ".max_energy";
      double partMinEnergy = 0.0;
      double partMaxEnergy = std::numeric_limits<double>::infinity();
      if (config_.has_key(part_min_energy_key)) {
	partMinEnergy = config_.fetch_real_with_explicit_dimension(part_min_energy_key, "energy");
	DT_THROW_IF(partMinEnergy < 0.0, std::domain_error,
		    "Invalid minimum kinetic energy for POI particle #" << i << "!"); 
      }
      if (config_.has_key(part_max_energy_key)) {
	partMaxEnergy = config_.fetch_real_with_explicit_dimension(part_max_energy_key, "energy");
	DT_THROW_IF(partMaxEnergy <= partMinEnergy, std::domain_error,
		    "Invalid maximum kinetic energy for POI particle #" << i << "!"); 
      }
      poi_info_type poi_info;
      poi_info.type = partType;
      poi_info.min_energy = partMinEnergy;
      poi_info.max_energy = partMaxEnergy;
      _poi_cut_.push_back(poi_info);
    }
 
    if (config_.has_key("record_original_event_id")) {
      bool flag = config_.fetch_boolean("record_original_event_id");
      set_record_original_event_id(flag);
    }

    _at_init_();
    _initialized_ = true;
    DT_LOG_TRACE_EXITING(get_logging_priority());
    return;
  }

  void poi_generator::reset()
  {
    DT_THROW_IF(! is_initialized(), std::logic_error, "Generator is not initialized!");
    _initialized_ = false;
    _at_reset_();
    return;
  }

  std::size_t poi_generator::get_original_event_counter() const
  {
    return _original_event_counter_;
  }

  std::size_t poi_generator::get_event_counter() const
  {
    return _event_counter_;
  }

  bool poi_generator::_select_(primary_event & event_) const
  {
    std::vector<unsigned int> poiChecked;
    std::set<std::size_t> checkParticles;
    poiChecked.assign(_poi_cut_.size(), 0U);
    for (std::size_t iPoiInfo = 0; iPoiInfo < _poi_cut_.size(); iPoiInfo++) {
      const poi_info_type & poiInfo = _poi_cut_[iPoiInfo];
      for (std::size_t iPart = 0; iPart < event_.get_number_of_particles(); iPart++) {
	// if (checkParticles.count(iPart)) {
	//   // This particle already matches a POI criterion:
	//   continue;
	// }
	const primary_particle & part = event_.get_particle(iPart);
	if (part.get_type() == poiInfo.type) {
	  double partK = part.get_kinetic_energy();
	  if (partK >= poiInfo.min_energy and partK <= poiInfo.max_energy) {
	    poiChecked[iPoiInfo]++;
	    checkParticles.insert(iPart);
	  }
	}
      }
    }
    if (checkParticles.size() < _poi_cut_.size()) {
      return false;
    }
    for (int nbChk : poiChecked) {
      if (nbChk == 0) return false;
    }
    return true;
  }

  bool poi_generator::has_next()
  {
    DT_THROW_IF(! is_initialized(), std::logic_error, "Generator is not initialized!");
    return _buffer_event_.is_valid();
  }

  void poi_generator::_search_for_next_event_()
  {
    _buffer_event_.reset();
    if (not _pg_->has_next()) {
      // DT_THROW(std::logic_error, "No more event from the wrapped generator!");
      return;
    }
    bool need_new_event = true;
    while (need_new_event) {
      _pg_->load_next(_buffer_event_, false);
      _original_event_counter_++;
      if (_select_(_buffer_event_)) {
	break;
      } else {
	_buffer_event_.reset();
      }
      // Break if no more primary event:
      if (not _pg_->has_next()) {
	break;
      }
    }
    return;
  }

  void poi_generator::_load_next(primary_event & event_,
				 bool compute_classification_)
  {
    DT_LOG_TRACE_ENTERING(get_logging_priority());
    event_.reset();
    if (_buffer_event_.is_valid()) {
      // We check is a buffered event is stored and use it if available:
      event_ = _buffer_event_;
      _buffer_event_.reset();
      _event_counter_++;
    }
    if (_record_original_event_id_) {
      int original_event_id = (int) _original_event_counter_;
      event_.grab_auxiliaries().store("original_event_id", original_event_id);
    }  
    if (compute_classification_) {
      event_.compute_classification();
    }
    _search_for_next_event_();
    DT_LOG_TRACE_EXITING(get_logging_priority());
    return;
  }

} // end of namespace genbb

/***************
 * OCD support *
 ***************/

#include <datatools/ocd_macros.h>

// OCD support for class '::genbb::poi_generator' :
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(::genbb::poi_generator,ocd_)
{
  ocd_.set_class_name("genbb::poi_generator");
  ocd_.set_class_description("Particles-Of-Interest generator");
  ocd_.set_class_library("genbb_help");
  ocd_.set_class_documentation("The ``genbb::poi_generator`` enables to select    \n"
                               "the first primary event from a sequence of primary\n"
                               "events with specific particle/energy topology.    \n"
                              );

  ::genbb::i_genbb::base_initialize_ocd(ocd_);

  ocd_.set_configuration_hints("Here are some typical configurations:                        \n"
                               "                                                             \n"
                               "***Example***                                              \n"
                               "                                                             \n"
                               "A generator that selects Bi-207 events with two high         \n"
			       "energy electrons:                                            \n"
                               "::                                                           \n"
                               "  logging.priority : string = \"fatal\"                      \n"
                               "  generator : string = \"Bi207\"                             \n"
                               "  poi.number_of_particules : integer = 2                     \n"
                               "  poi.particule_0.type : string = \"electron\"               \n"
			       "  poi.particule_0.min_energy : real as energy =  500 keV     \n"
			       "  poi.particule_0.max_energy : real as energy = 2000 keV     \n"
                               "  poi.particule_1.type : string = \"electron\"               \n"
			       "  poi.particule_1.min_energy : real as energy =  500 keV     \n"
                               "                                                             \n"
                               "                                                             \n"
                               );

  ocd_.set_validation_support(false);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()

DOCD_CLASS_SYSTEM_REGISTRATION(genbb::poi_generator,
                               "genbb::poi_generator")
