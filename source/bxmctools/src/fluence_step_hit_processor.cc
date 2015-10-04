/** mctools/fluence_step_hit_processor.cc */

// Ourselves:
#include <mctools/fluence_step_hit_processor.h>

// Standard library:
#include <algorithm>
#include <cstdlib>

// Third party:
// - Boost:
#include <boost/algorithm/string.hpp>
// - Bayeux/datatools:
#include <datatools/ioutils.h>
#include <datatools/exception.h>
// - Bayeux/genbb_help:
#include <genbb_help/single_particle_generator.h>

// This project:
#include <mctools/utils.h>

namespace mctools {

  MCTOOLS_STEP_HIT_PROCESSOR_REGISTRATION_IMPLEMENT(fluence_step_hit_processor,
                                                    "mctools::fluence_step_hit_processor");

  void fluence_step_hit_processor::set_mapping_category(const std::string & sc_)
  {
    _mapping_category_ = sc_;
    return;
  }

  const std::string & fluence_step_hit_processor::get_mapping_category() const
  {
    return _mapping_category_;
  }

  // Constructor:
  fluence_step_hit_processor::fluence_step_hit_processor()
    : base_step_hit_processor()
  {
    _mapping_    = 0;
    _categories_ = 0;
    _fluence_volume_type_ = geomtools::geom_id::INVALID_TYPE;
    return;
  }

  fluence_step_hit_processor::~fluence_step_hit_processor()
  {
    return;
  }

  void fluence_step_hit_processor::initialize(const ::datatools::properties & config_,
                                              ::datatools::service_manager & service_mgr_)
  {
    _init(config_, service_mgr_);
    return;
  }

  void fluence_step_hit_processor::_init(const ::datatools::properties & config_,
                                         ::datatools::service_manager & service_mgr_)
  {
    this->base_step_hit_processor::initialize(config_, service_mgr_);

    // The geometry manager is mandatory for this processor:
    DT_THROW_IF(! has_geom_manager(), std::logic_error,
                "Missing geometry manager for processor '" << get_name() << "' !");

    if (config_.has_key("debug")) {
      bool debug = config_.fetch_boolean("debug");
      set_debug(debug);
    }

    if (config_.has_key("mapping.category")) {
      std::string geom_cat = config_.fetch_string("mapping.category");
      set_mapping_category(geom_cat);
    }

    std::vector<std::string> any_addresses_labels;
    if (config_.has_key("mapping.category.any_addresses")) {
      config_.fetch("mapping.category.any_addresses", any_addresses_labels);
    }

    // A sensitive category is mandatory for this processor,
    // i.e. the one defined for the target fluence volumes:
    DT_THROW_IF(_mapping_category_.empty(), std::logic_error,
                "Missing 'mapping_category' string property for processor '" << get_name() << "' !");

    DT_LOG_DEBUG(get_logging_priority(), "Parsed setup properties for processor '" << get_name() << "' ...");

    // pickup the ID mapping from the geometry manager:
    _mapping_ = &_geom_manager->get_mapping();

    // check if the sensitive category is known:
    _categories_ = &(_geom_manager->get_id_mgr().categories_by_name());
    geomtools::id_mgr::categories_by_name_col_type::const_iterator icat
      = _categories_->find(_mapping_category_);
    DT_THROW_IF(icat == _categories_->end(), std::logic_error,
                "Cannot find geometry ID category '"
                << _mapping_category_ << "' string property for processor '"
                << get_name() << "' !");

    const geomtools::id_mgr::category_info & mapping_cat_info = icat->second;

    // Initialize the smart ID locator for this category of volumes:
    _fluence_volume_type_ = mapping_cat_info.get_type();
    _fluence_volume_locator_.set_gmap(*_mapping_);
    _fluence_volume_locator_.initialize(_fluence_volume_type_);

    for (size_t i = 0; i < any_addresses_labels.size(); i++) {
      std::vector<std::string> strs;
      boost::split(strs, any_addresses_labels[i], boost::is_any_of("@"));
      DT_THROW_IF(strs.size() > 2, std::logic_error,
                  "Invalid format for 'mapping.category.any_addresses' property ' from "
                  << any_addresses_labels[i] << "' !");
      const std::string & label = strs[0];
      int label_count = 1;
      if (strs.size() == 2) {
        const std::string & count_str = strs[1];
        std::istringstream iss(count_str);
        iss >> label_count;
        DT_THROW_IF(! iss, std::logic_error,
                    "Invalid format for 'mapping.category.any_addresses' ! "
                    << "Cannot parse subaddress label count from "
                    << count_str << "' !");
      }
      DT_THROW_IF((! mapping_cat_info.has_subaddress(label, label_count)),
                  std::logic_error,
                  "Category '" << mapping_cat_info.get_category()
                  << "' has no subaddress labelled '" << label << "' !");
      _mapping_category_any_addresses_.push_back(mapping_cat_info.get_subaddress_index(label, label_count));
    }
    if (_mapping_category_any_addresses_.size()) {
      std::sort(_mapping_category_any_addresses_.begin(),
                _mapping_category_any_addresses_.end());
    }

    return;
  }

  const std::vector<int> & fluence_step_hit_processor::get_mapping_category_any_addresses() const
  {
    return _mapping_category_any_addresses_;
  }

  bool fluence_step_hit_processor::locate_fluence_volume(const geomtools::vector_3d & position_,
                                                         geomtools::geom_id & gid_) const
  {
    gid_ = _fluence_volume_locator_.get_geom_id(position_, _fluence_volume_type_);
    return gid_.is_valid();
  }

  void fluence_step_hit_processor::process(const ::mctools::base_step_hit_processor::step_hit_ptr_collection_type & the_base_step_hits,
                                           ::mctools::simulated_data::hit_collection_type & the_plain_fluence_hits)
  {
    _process(the_base_step_hits, (simulated_data::hit_handle_collection_type *) 0, &the_plain_fluence_hits);
    return;
  }

  void fluence_step_hit_processor::process(const ::mctools::base_step_hit_processor::step_hit_ptr_collection_type & the_base_step_hits,
                                           ::mctools::simulated_data::hit_handle_collection_type & the_fluence_hits)
  {
    _process(the_base_step_hits, &the_fluence_hits, (simulated_data::hit_collection_type *) 0);
    return;
  }

  void fluence_step_hit_processor::_process(const base_step_hit_processor::step_hit_ptr_collection_type & shpc_,
                                            simulated_data::hit_handle_collection_type * fluence_hits_,
                                            simulated_data::hit_collection_type        * plain_fluence_hits_)
  {
    // Check the type of output collection (handles or plain hits) :
    bool use_handles = false;
    if (fluence_hits_ != 0) {
      use_handles = true;
    } else if (plain_fluence_hits_ == 0) {
      DT_THROW_IF(true, std::logic_error, "Missing hit collection type (NULL pointer) !");
    }

    base_step_hit * current_fluence_hit = 0;
    uint32_t      fluence_hit_count     = 0;

    // Prereservation :
    if (use_handles) {
      fluence_hits_->reserve(20);
    }
    else {
      plain_fluence_hits_->reserve(20);
      // size_t current_size = plain_fluence_hits_->size();
      // plain_fluence_hits_->reserve(current_size + fluence_hits_->size());
    }

    genbb::single_particle_generator spg;
    for (base_step_hit_processor::step_hit_ptr_collection_type::const_iterator ihit = shpc_.begin();
         ihit != shpc_.end();
         ihit++) {

      base_step_hit & the_step_hit = const_cast<base_step_hit &>(*(*ihit));

      if (the_step_hit.get_auxiliaries().has_flag (mctools::track_utils::ENTERING_VOLUME_FLAG)
          || the_step_hit.get_auxiliaries().has_flag (mctools::track_utils::LEAVING_VOLUME_FLAG)) {
        const geomtools::vector_3d & position_start = the_step_hit.get_position_start();
        const geomtools::vector_3d & position_stop  = the_step_hit.get_position_stop();

        geomtools::vector_3d         position_mean  = 0.5 * (position_start + position_stop);
        const geomtools::vector_3d & momentum_start       = the_step_hit.get_momentum_start();
        DT_THROW_IF(!geomtools::is_valid(momentum_start), std::logic_error,
                    "Source step hit has no valid recorded particle start momentum!");
        const geomtools::vector_3d & momentum_stop       = the_step_hit.get_momentum_stop();
        DT_THROW_IF(!geomtools::is_valid(momentum_stop), std::logic_error,
                    "Source step hit has no valid recorded particle stop momentum!");

        std::string particle_name = the_step_hit.get_particle_name();
        DT_THROW_IF(particle_name.empty(), std::logic_error,
                    "Source step hit has no valid recorded particle name!");
        double      particle_mass = spg.get_particle_mass_from_label (particle_name);

        // locate the hit using the mean position through the smart locator:
        geomtools::geom_id gid;
        locate_fluence_volume (position_mean, gid);
        if (! gid.is_valid()) {
          // we do not process such a hit:
          DT_LOG_ERROR(get_logging_priority(),
                       "We skip this hit for one cannot locate it through the locator attached to the '"
                       << get_mapping_category() << "' ! "
                       << " This may be due to a roundoff error while checking the geometry of the volume "
                       << "or to another mapping category registered in the current '"
                       << get_hit_category() << "' hit category "
                       << "that may generate its own step hits ! Consider to write your own hit processor able "
                       << "to handle several mapping categories (using several suitable locators) !");
          continue;
        }

        DT_LOG_DEBUG(get_logging_priority(), "GID              = " << gid);

        // Set 'any' for some GID subaddresses:
        for (size_t i = 0; i < _mapping_category_any_addresses_.size(); i++) {
          gid.set_any(_mapping_category_any_addresses_[i]);
        }

        if (use_handles) {
          add_new_hit(*fluence_hits_);
          current_fluence_hit = &(fluence_hits_->back().grab());
        }
        else {
          base_step_hit dummy;
          plain_fluence_hits_->push_back(dummy);
          current_fluence_hit = &(plain_fluence_hits_->back());
        }

        // update the attributes of the hit:
        current_fluence_hit->set_hit_id(fluence_hit_count);
        current_fluence_hit->set_geom_id(gid);
        current_fluence_hit->set_particle_name(particle_name);

        // store auxiliaries:

        const bool is_primary_particle
          = the_step_hit.get_auxiliaries().has_flag(mctools::track_utils::PRIMARY_PARTICLE_FLAG);
        DT_LOG_DEBUG(get_logging_priority(), "Primary particle = " << is_primary_particle);
        if (is_primary_particle) {
          current_fluence_hit->grab_auxiliaries().store_flag(mctools::track_utils::PRIMARY_PARTICLE_FLAG);
        }

        const bool has_track_id
          = the_step_hit.get_auxiliaries().has_flag(mctools::track_utils::TRACK_ID_KEY);
        DT_LOG_DEBUG(get_logging_priority(), "Track ID = " << has_track_id);
        if (has_track_id) {
          current_fluence_hit->grab_auxiliaries().store_flag(mctools::track_utils::TRACK_ID_KEY);
        }

        const bool has_parent_track_id
          = the_step_hit.get_auxiliaries().has_flag(mctools::track_utils::PARENT_TRACK_ID_KEY);
        DT_LOG_DEBUG(get_logging_priority(), "Parent track ID = " << has_parent_track_id);
        if (has_parent_track_id) {
          current_fluence_hit->grab_auxiliaries().store_flag(mctools::track_utils::PARENT_TRACK_ID_KEY);
        }

        if (the_step_hit.get_auxiliaries().has_flag (mctools::track_utils::ENTERING_VOLUME_FLAG)) {
          current_fluence_hit->grab_auxiliaries().store_flag(mctools::track_utils::ENTERING_VOLUME_FLAG);
          current_fluence_hit->set_position_start(position_start);
          current_fluence_hit->set_momentum_start(momentum_start);
          double entering_kinetic_energy = std::sqrt(momentum_start.x()*momentum_start.x()
                                                     +momentum_start.y()*momentum_start.y()
                                                     +momentum_start.z()*momentum_start.z()
                                                     +particle_mass*particle_mass)-particle_mass;
          DT_LOG_DEBUG(get_logging_priority(), "entering_kinetic_energy = " << entering_kinetic_energy / CLHEP::keV<< " keV.");
          current_fluence_hit->grab_auxiliaries().store(track_utils::ENTERING_KINETIC_ENERGY_KEY, entering_kinetic_energy);
       }
        if (the_step_hit.get_auxiliaries().has_flag (mctools::track_utils::LEAVING_VOLUME_FLAG)) {
          current_fluence_hit->grab_auxiliaries().store_flag(mctools::track_utils::LEAVING_VOLUME_FLAG);
          current_fluence_hit->set_position_stop(position_stop);
          current_fluence_hit->set_momentum_stop(momentum_stop);
          double leaving_kinetic_energy = std::sqrt(momentum_stop.x()*momentum_stop.x()
                                                    +momentum_stop.y()*momentum_stop.y()
                                                    +momentum_stop.z()*momentum_stop.z()
                                                    +particle_mass*particle_mass)-particle_mass;
          DT_LOG_DEBUG(get_logging_priority(), "leaving_kinetic_energy = " << leaving_kinetic_energy / CLHEP::keV<< " keV.");
          current_fluence_hit->grab_auxiliaries().store(track_utils::LEAVING_KINETIC_ENERGY_KEY, leaving_kinetic_energy);
        }

        //increment the hit id:
        fluence_hit_count++;
      }
    }
    DT_LOG_TRACE(get_logging_priority(), "Exiting.");
    return;
  }

  void fluence_step_hit_processor::tree_dump(std::ostream & out_,
                                             const std::string & title_,
                                             const std::string & indent_,
                                             bool inherit_) const
  {
    std::string indent;
    if (! indent_.empty()) {
      indent = indent_;
    }
    base_step_hit_processor::tree_dump(out_, title_, indent, true);
    out_ << indent << datatools::i_tree_dumpable::tag
         << "Mapping category : '" <<_mapping_category_ << "'" << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag
         << "Mapping    : " << _mapping_ << std::endl;
    {
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Categories : " << _categories_ << " ";
      if (_categories_ != 0) {
        out_ << '[' << _categories_->size() << ']';
      }
      out_ << std::endl;
    }
    out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_)
         << "Fluence volume type : " << _fluence_volume_type_ << std::endl;

    return;
  }

  // static
  void fluence_step_hit_processor::init_ocd(datatools::object_configuration_description & ocd_)
  {

    // Inherits configuration properties from its base class:
    ::mctools::base_step_hit_processor::init_ocd(ocd_);


    {
      datatools::configuration_property_description & cpd = ocd_.add_configuration_property_info();
      cpd.set_name_pattern("mapping.category")
        .set_terse_description("Specify the geometry category associated to the sensitive volume")
        .set_from("mctools::base_step_hit_processor")
        .set_traits(datatools::TYPE_STRING)
        .set_mandatory(true)
        .add_example("Use a specific geometry category::                \n"
                     "                                                  \n"
                     "  mapping.category : string = \"hfluence.volume\" \n"
                     "                                                  \n"
                     )
        .set_long_description("The processor needs to know, from the geometry Id mapping of the \n"
                              "virtual geometry setup, the geometry category that is associated \n"
                              "to the attached sensitive volume. This allows the computing of   \n"
                              "the geometry Id that must be associated to the final hit.        \n")
        ;
    }


    {
      datatools::configuration_property_description & cpd = ocd_.add_configuration_property_info();
      cpd.set_name_pattern("mapping.category.any_addresses")
        .set_terse_description("Specify the subaddress of the geometry category that should be ignored")
        .set_from("mctools::base_step_hit_processor")
        .set_traits(datatools::TYPE_STRING,
                    datatools::configuration_property_description::ARRAY
                    )
        .set_mandatory(false)
        .add_example("Specify the name of the geometry Id subaddress that is irrelevant\n"
                     "while computing the Gid of the hit::                             \n"
                     "                                                                 \n"
                     "  mapping.category.any_addresses : string[1] = \"part\"          \n"
                     "                                                                 \n"
                     )
        .set_long_description("Not documented yet\n")
        ;
    }


    return;
  }

} // end of namespace mctools


  // OCD support for class '::mctools::fluence_step_hit_processor' :
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(::mctools::fluence_step_hit_processor, ocd_)
{
  ocd_.set_class_name("mctools::fluence_step_hit_processor");
  ocd_.set_class_description("A Monte Carlo step hit post-processor that scores the boundary energy of particles going through the surface of sensitive volumes");
  ocd_.set_class_library("mctools");
  ocd_.set_class_documentation("A Monte-Carlo step hit post-processor that saves hits entering in a \n"
                               "sensitive volume. It contains the following informations/attributes:\n"
                               "                                                                    \n"
                               "  * the geometry identifier (GID) of the geometry volume in which   \n"
                               "    the hit was going through the surface                           \n"
                               "  * the name of the traversing particle                             \n"
                               "  * the energy of the particle at boundary                          \n"
                               "                                                                    \n"
                               "Such hit is supposed to be use to compute , from a backend application\n"
                               "dosimetry quantities such gas H*(10)*.                                \n"
                               );
  ocd_.set_class_library("mctools");

  // Load OCD support for this class:
  ::mctools::fluence_step_hit_processor::init_ocd(ocd_);

  ocd_.set_configuration_hints("The fluence step hit processor factory uses a ``datatools::properties`` \n"
                               "object to initialize its behaviour and contents.                  \n"
                               "                                                                  \n"
                               "Example of the configuration for a fluence sensitive detector::   \n"
                               "                                                                  \n"
                               "  sensitive.category    : string  = \"fluence_SD\"                \n"
                               "  use_private_pool      : boolean = 1                             \n"
                               "  hit.category          : string  = \"fluence\"                   \n"
                               "  mapping.category      : string  = \"fluence.volume\"            \n"
                               "                                                                  \n"
                               "From the main configuration file of a *step hit processor factory*,               \n"
                               "one must use the ``datatools::multi_properties`` format. Example of               \n"
                               "a fluence step hit processor attached to some fluence                             \n"
                               "volumes in an experimental setup::                                                \n"
                               "                                                                                  \n"
                               "  [name=\"fluence.hit_processor\" type=\"mctools::fluence_step_hit_processor\"]   \n"
                               "  sensitive.category    : string  = \"fluence_SD\"                                \n"
                               "  use_private_pool      : boolean = 1                                             \n"
                               "  hit.category          : string  = \"fluence\"                                   \n"
                               "  mapping.category      : string  = \"fluence.volume\"                            \n"
                               )
    ;

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()
DOCD_CLASS_SYSTEM_REGISTRATION(::mctools::fluence_step_hit_processor,
                               "mctools::fluence_step_hit_processor")
