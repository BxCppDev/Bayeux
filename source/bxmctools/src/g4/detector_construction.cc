// -*- mode: c++ ; -*-
/* detector_construction.cc
 */

#include <boost/algorithm/string/predicate.hpp>

#include <boost/filesystem.hpp>

#include <datatools/utils.h>
#include <datatools/ioutils.h>
#include <datatools/units.h>
#include <datatools/logger.h>
#include <datatools/exception.h>

#include <mygsl/random_utils.h>

#include <geomtools/gdml_export.h>
#include <geomtools/sensitive.h>
#include <geomtools/color.h>
#include <geomtools/visibility.h>
#include <geomtools/manager.h>
#include <geomtools/materials_plugin.h>
#include <geomtools/materials_utils.h>

#include <emfield/base_electromagnetic_field.h>
#include <emfield/electromagnetic_field_manager.h>
#include <emfield/emfield_geom_plugin.h>
#include <emfield/geom_map.h>

#include <mctools/g4/detector_construction.h>
#include <mctools/g4/sensitive_detector.h>
#include <mctools/g4/magnetic_field.h>
#include <mctools/g4/manager.h>

// G4 stuff
#include <globals.hh>
#include <G4FieldManager.hh>
#include <G4ChordFinder.hh>
#include <G4TransportationManager.hh>
#include <G4GDMLParser.hh>
#include <G4SDManager.hh>
#include <G4LogicalVolumeStore.hh>
#include <G4Color.hh>
#include <G4VisAttributes.hh>
#include <G4UserLimits.hh>

namespace mctools {

  namespace g4 {

    using namespace std;

    const double detector_construction::DEFAULT_MISS_DISTANCE = 0.5 * CLHEP::mm;

    const detector_construction::sensitive_detector_dict_type &
    detector_construction::get_sensitive_detectors() const
    {
      return _sensitive_detectors_;
    }

    detector_construction::SHPF_type &
    detector_construction::grab_step_hit_processor_factory()
    {
      return _SHPF_;
    }

    const manager &
    detector_construction::get_manager() const
    {
      DT_THROW_IF (_g4_manager_ == 0, logic_error,"Operation prohibited !");
      return *_g4_manager_;
    }

    manager &
    detector_construction::grab_manager()
    {
      DT_THROW_IF (_g4_manager_ == 0, logic_error,"Operation prohibited !");
      return *_g4_manager_;
    }

    const geomtools::manager &
    detector_construction::get_geometry_manager() const
    {
      return *_geom_manager_;
    }

    //____________________________________  ctor:

    detector_construction::detector_construction(manager & g4_mgr_)
    {
      _initialized_ = false;
      _abort_on_error_ = true;

      _g4_manager_ = &g4_mgr_;
      _geom_manager_ = 0;
      _generate_gdml_file_ = true;
      _materials_geom_plugin_name_ = "";
      _gdml_filename_ = "";
      _gdml_file_dir_ = "";
      _gdml_schema_ = geomtools::gdml_writer::DEFAULT_REMOTE_GDML_SCHEMA;
      _gdml_validation_ = true;

      _using_vis_attributes_ = true;
      _using_user_limits_ = true;
      _using_regions_ = true;
      _using_sensitive_detectors_ = true;
      _using_mag_field_ = true;
      _mag_field_manager_  = 0;
      _miss_distance_unit_ = CLHEP::mm;
      _general_miss_distance_ = DEFAULT_MISS_DISTANCE;
      return;
    }

    //____________________________________  dtor:
    detector_construction::~detector_construction()
    {

      // Clear visualization attributes:
      _vis_attributes_.clear();
      for (map<string, G4VisAttributes *>::iterator i = _vis_attributes_.begin();
           i != _vis_attributes_.end();
           ++i) {
        if (i->second != 0) {
          delete i->second;
          i->second = 0;
        }
      }

      // Clear user limits:
      for (list<G4UserLimits *>::iterator i = _user_limits_col_.begin();
           i != _user_limits_col_.end();
           ++i) {
        if (*i != 0) {
          delete *i;
        }
      }
      _user_limits_col_.clear();

      return;
    }

    //____________________________________
    bool detector_construction::is_initialized() const
    {
      return _initialized_;
    }


    //____________________________________
    bool detector_construction::has_geometry_manager() const
    {
      return _geom_manager_ != 0;
    }


    //____________________________________
    void detector_construction::set_geometry_manager(const geomtools::manager & gm_)
    {
      DT_THROW_IF (is_initialized(), std::logic_error, "Operation prohibited !");
      _geom_manager_ = &gm_;
      return;
    }

    //____________________________________
    void detector_construction::initialize(const datatools::properties & config_)
    {
      DT_THROW_IF (is_initialized(), std::logic_error, "Already initialized !");

      /*** parsing properties from 'config_'  ***/
      loggable_support::_initialize_logging_support(config_);

      if (config_.has_key("gdml.tmp_dir")) {
        std::string gdml_tmp_dir = config_.fetch_string("gdml.tmp_dir") ;
        datatools::fetch_path_with_env(gdml_tmp_dir);
        _gdml_file_dir_ = gdml_tmp_dir;
      }

      if (config_.has_key("gdml.validation")) {
        _gdml_validation_ = config_.fetch_boolean("gdml.validation");
      }

      if (config_.has_key("gdml.schema_location")) {
        std::string gdml_schema_location = config_.fetch_string("gdml.schema_location") ;
        if (gdml_schema_location == "remote") {
          _gdml_schema_ = geomtools::gdml_writer::DEFAULT_REMOTE_GDML_SCHEMA;
        } else if (gdml_schema_location == "local") {
          _gdml_schema_ = "${GEOMTOOLS_DATA_DIR}/resources/gdml_schema/gdml.xsd";
          datatools::fetch_path_with_env(_gdml_schema_);
        } else {
          DT_THROW_IF(true, std::logic_error, "Missing property '" << "gdml.schema_location" << "' !");
        }
      }

      if (config_.has_key("materials.plugin_name")) {
        std::string mpn = config_.fetch_string("materials.plugin_name");
        set_materials_geom_plugin_name(mpn);
      }

      config_.export_and_rename_starting_with(_SHPF_config_, "hit_processor_factory.", "");
      DT_THROW_IF(_SHPF_config_.empty (), std::logic_error,
                  "Missing property '" << "hit_processor_factory" << "' !");

      /*************************/

      if (config_.has_key("using_user_limits")) {
        _using_user_limits_ = config_.fetch_boolean("using_user_limits");
      }

      config_.export_starting_with(_limits_config_, "limits.");

      // end of fetching.

      DT_THROW_IF (_geom_manager_ == 0, std::logic_error,
                   "Missing geometry manager !");

      /*************************
       *                       *
       *  Sensitive detectors  *
       *                       *
       *************************/

      if (config_.has_key("sensitive.detectors")) {
        std::vector<std::string> list_of_sensitive_detectors;
        config_.fetch("sensitive.detectors", list_of_sensitive_detectors);

        // Loop on the list of official sensitive categories/detectors :
        for (unsigned int i = 0; i < list_of_sensitive_detectors.size(); ++i) {
          const std::string & SD_category = list_of_sensitive_detectors[i];
          if (! _SD_params_.has_key(SD_category)) {
            _SD_params_.add(SD_category, "SD");
          }

          // Extract properties starting with the
          // 'sensitive.CATEGORY_SD' prefix and store them with a new
          // prefix starting only with 'sensitive.'
          const std::string key_oss     = "sensitive." + SD_category;
          const std::string new_key_oss = "sensitive";
          config_.export_and_rename_starting_with(_SD_params_.get(SD_category).get_properties(),
                                                  key_oss,
                                                  new_key_oss);
        }

        DT_LOG_NOTICE(_logprio(),"Info for sensitive detectors : ");
        for (std::vector<std::string>::const_iterator i = list_of_sensitive_detectors.begin();
             i != list_of_sensitive_detectors.end();
             ++i) {
          std::vector<std::string>::const_iterator j = i;
          if (++j == list_of_sensitive_detectors.end()) {
            DT_LOG_NOTICE(_logprio(),"`-- " << "Sensitive detectors '" << *i << "'");
          } else {
            DT_LOG_NOTICE(_logprio(),"|-- " << "Sensitive detectors '" << *i << "'");
          }
          if (is_debug()) {
            DT_LOG_DEBUG(_logprio(),"    ==> " << "Sensitive parameters for '" << *i << "' : ");
            _SD_params_.get(*i).get_properties().tree_dump(std::clog, "");
          }
        }
      }


      /*************************
       *                       *
       *        Regions        *
       *                       *
       *************************/

      if (config_.has_key("using_regions")) {
        _using_regions_ = config_.fetch_boolean("using_regions");
      }

      if (config_.has_key("regions")) {
        std::vector<std::string> regions;
        config_.fetch("regions", regions);
        for (unsigned int i = 0; i < regions.size(); ++i) {
          const std::string & the_region_label = regions[i];
          {
            std::vector<std::string> dummy;
            _region_infos_[the_region_label] = dummy;
          }
          std::vector<std::string> & ri = _region_infos_[the_region_label];

          {
            // Regions by volumes:
            std::ostringstream key;
            key << "regions." << the_region_label << ".volumes";
            if (config_.has_key(key.str())) {
              std::vector<std::string> the_region_infos;
              config_.fetch(key.str(), the_region_infos);
              for (int i = 0; i < the_region_infos.size(); i++) {
                const std::string & log_name = the_region_infos[i];
                if (std::find(ri.begin(), ri.end(), log_name) == ri.end()) {
                  if (log_name != "world.log") {
                    ri.push_back(log_name);
                  }
                }
              }
            }
          } // Regions by volumes.

          {
            // Regions by materials:
            std::ostringstream key;
            key << "regions." << the_region_label << ".volumes.with_materials";
            if (config_.has_key(key.str())) {
              std::vector<std::string> material_refs;
              config_.fetch(key.str(), material_refs);
              for (geomtools::logical_volume::dict_type::const_iterator ilogical
                     = _geom_manager_->get_factory().get_logicals().begin();
                   ilogical != _geom_manager_->get_factory().get_logicals().end();
                   ++ilogical) {
                // Get a reference to the associated logical volume :
                const geomtools::logical_volume & log = *(ilogical->second);
                // Check if it is tagged as a 'sensitive' detector :
                if (log.get_parameters().has_key("material.ref")) {
                  const std::string & mr = log.get_parameters().fetch_string("material.ref");
                  for (int i = 0; i < material_refs.size(); i++) {
                    const std::string & material_ref = material_refs[i];
                    if (mr == material_ref) {
                      if (std::find(ri.begin(), ri.end(), log.get_name()) == ri.end()) {
                        if (log.get_name() != "world.log") {
                          ri.push_back(log.get_name());
                        }
                      }
                      break;
                    }
                  }
                }
              }
            }
          } // Regions by materials.

          {
            // Excluded volumes:
            std::ostringstream key;
            key << "regions." << the_region_label << ".volumes.excluded";
            if (config_.has_key(key.str())) {
              std::vector<std::string> excluded_volumes;
              config_.fetch(key.str(), excluded_volumes);
              for (int i = 0; i < excluded_volumes.size(); i++) {
                const std::string & log_name = excluded_volumes[i];
                ri.erase( std::remove( ri.begin(), ri.end(), log_name ), ri.end() );
              }
            }
          } // Excluded volumes.

          DT_THROW_IF (ri.size() == 0, std::logic_error,
                       "Region '" << the_region_label << "' has not logical volumes !");
        }

        DT_LOG_NOTICE(_logprio(), "Info for regions : ");
        for (region_infos_dict_type::const_iterator i = _region_infos_.begin();
             i != _region_infos_.end();
             ++i) {
          std::ostringstream message;
          region_infos_dict_type::const_iterator j = i;
          if (++j == _region_infos_.end()) {
            message << "`-- ";
          } else {
            message << "|-- ";
          }
          message << "Region '" << i->first << "'  :  Volumes =(";
          for (unsigned int k = 0; k < i->second.size(); ++k) {
            message << "'" << i->second[k] << "' ";
          }
          message << ')';
          DT_LOG_NOTICE(_logprio(), message.str());
        }
      }


      /*************************
       *                       *
       *    Magnetic field     *
       *                       *
       *************************/

      if (config_.has_key("using_magnetic_field")) {
        _using_mag_field_ = config_.fetch_boolean("using_magnetic_field");
      }

      if (_using_mag_field_) {
        DT_LOG_NOTICE(_logprio(), "Using magnetic field...");

        if (config_.has_key("magnetic_field.plugin_name")) {
          std::string fpn = config_.fetch_string("magnetic_field.plugin_name");
          set_emfield_geom_plugin_name(fpn);
        }

        if (config_.has_key("magnetic_field.miss_distance.unit")) {
          std::string md_unit_str = config_.fetch_string("magnetic_field.miss_distance.unit");
          _miss_distance_unit_ = datatools::units::get_length_unit_from(md_unit_str);
        }

        if (config_.has_key("magnetic_field.miss_distance")) {
          _general_miss_distance_ = config_.fetch_real("magnetic_field.miss_distance");
          if (! config_.has_explicit_unit("magnetic_field.miss_distance")) {
            _general_miss_distance_ *= _miss_distance_unit_;
          }

          const geomtools::manager & geo_mgr = get_geometry_manager();
          std::string emfield_geom_plugin_name = _emfield_geom_plugin_name_;
          if (_emfield_geom_plugin_name_.empty()) {
            // We try to find a emfield plugin associated to the manager :
            typedef geomtools::manager::plugins_dict_type dict_type;
            const dict_type & plugins = geo_mgr.get_plugins();
            for (dict_type::const_iterator ip = plugins.begin();
                 ip != plugins.end();
                 ip++) {
              const std::string & plugin_name = ip->first;
              if (geo_mgr.is_plugin_a<emfield::emfield_geom_plugin>(plugin_name)) {
                emfield_geom_plugin_name = plugin_name;
                break;
              }
            }
          }
          // Access to a given plugin by name and type :
          if (geo_mgr.has_plugin(emfield_geom_plugin_name)
              && geo_mgr.is_plugin_a<emfield::emfield_geom_plugin>(emfield_geom_plugin_name)) {
            DT_LOG_NOTICE(_logprio(), "Found EM field geometry plugin named '"
                          << emfield_geom_plugin_name << "'");
            const emfield::emfield_geom_plugin & fgp
              = geo_mgr.get_plugin<emfield::emfield_geom_plugin>(emfield_geom_plugin_name);
            const emfield::electromagnetic_field_manager & emf_mgr = fgp.get_manager();
            set_mag_field_manager(emf_mgr);
          }

          config_.export_and_rename_starting_with(_mag_field_aux_, "magnetic_field.", "");
        }
      }

      _initialized_ = true;
      return;
    }


    //____________________________________
    G4VPhysicalVolume * detector_construction::Construct()
    {
      DT_LOG_TRACE(_logprio(), "Entering...");

      DT_THROW_IF (! is_initialized(), std::logic_error, "Not initialized !");

      G4VPhysicalVolume * world_physical_volume = 0;

      world_physical_volume = _g4_construct();

      if (world_physical_volume == 0) {
        G4Exception("mctools::g4::detector_construction::Construct",
                    "GeometryError",
                    FatalException,
                    "Missing world physical volume !");
      }

      DT_LOG_TRACE(_logprio(), "Exiting.");
      return world_physical_volume;
    }

    //____________________________________
    G4VPhysicalVolume * detector_construction::_g4_construct()
    {
      DT_LOG_TRACE(_logprio(), "Entering...");

      // Automaticaly construct the geometry tree:
      G4VPhysicalVolume * world_physical_volume = 0;
      G4GDMLParser parser;
      parser.SetStripFlag(false);
      if (_generate_gdml_file_) {
        write_tmp_gdml_file();
        parser.Read(_gdml_filename_, _gdml_validation_);
      } else parser.Read("test_manager.gdml");
      world_physical_volume = parser.GetWorldVolume();

      if (is_debug()) {
        DT_LOG_DEBUG(_logprio(), "G4LogicalVolumeStore: ");
        G4LogicalVolumeStore * g4_LV_store = G4LogicalVolumeStore::GetInstance();
        for (G4LogicalVolumeStore::const_iterator i = g4_LV_store->begin();
             i != g4_LV_store->end();
             ++i) {
          G4LogicalVolume * g4_log = *i;
          DT_LOG_DEBUG(_logprio(), "Geant4 logical: '" << g4_log->GetName() << "'");
        }
      }

      // Automaticaly construct regions:
      if (_using_regions_) {
        DT_LOG_DEBUG(_logprio(), "Construct regions.");
        _construct_regions();
      }

      // Automaticaly construct sensitive detectors:
      if (_using_sensitive_detectors_) {
        DT_LOG_DEBUG(_logprio(), "Construct sensitive detectors.");
        _construct_sensitive_detectors();
      }

      // Automaticaly construct the magnetic field :
      if (_using_mag_field_) {
        DT_LOG_DEBUG(_logprio(), "Construct the magnetic field.");
        _construct_magnetic_field();
      }

      // Automaticaly set the visibility attributes:
      if (_using_vis_attributes_) {
        DT_LOG_DEBUG(_logprio(), "Set the visualization attributes.");
        _set_visualization_attributes();
      }

      // Automaticaly set the user limits:
      if (_using_user_limits_) {
        DT_LOG_DEBUG(_logprio(), "Using the user limits(max step).");
        _set_user_limits();
      }

      DT_LOG_TRACE(_logprio(), "Exiting.");
      return world_physical_volume;
    }


    bool detector_construction::has_mag_field_manager() const
    {
      return _mag_field_manager_ != 0;
    }


    void detector_construction::set_mag_field_manager(const emfield::electromagnetic_field_manager &emf_mgr_)
    {
      DT_THROW_IF (_initialized_, std::logic_error, "Already initialized !");
      _mag_field_manager_ = &emf_mgr_;
      return;
    }

    const emfield::electromagnetic_field_manager &
    detector_construction::get_mag_field_manager() const
    {
      DT_THROW_IF (_mag_field_manager_ == 0, std::logic_error, "No magnetic field manager is defined !");
      return *_mag_field_manager_;
    }

    void detector_construction::_construct_magnetic_field()
    {
      datatools::logger::priority logging = _logprio();
      DT_LOG_TRACE(logging,"Entering...");

      // if (! _mag_field_->is_active())
      //   {
      //     return;
      //     /*
      //     std::clog << datatools::io::debug
      //          << "mctools::g4::detector_construction::_construct_magnetic_field: "
      //          << "Magnetic field is active." << std::endl;
      //     G4FieldManager * detector_field_mgr
      //       = G4TransportationManager::GetTransportationManager()->GetFieldManager();
      //     detector_field_mgr->SetDetectorField(_mag_field);
      //     detector_field_mgr->CreateChordFinder(_mag_field);
      //     detector_field_mgr->GetChordFinder()->SetDeltaChord(_miss_distance);
      //     */
      //   }

      if (has_mag_field_manager() && _mag_field_manager_->has_geom_map()) {
        DT_LOG_NOTICE(logging,"Processing geometry/EM field map...");

        std::vector<std::string> associations;
        if (_mag_field_aux_.has_key("associations")) {
          _mag_field_aux_.fetch("associations", associations);
        }
        G4LogicalVolumeStore * g4_LV_store = G4LogicalVolumeStore::GetInstance();
        const emfield::geom_map & geomap = _mag_field_manager_->get_geom_map();

        typedef emfield::geom_map::association_dict_type gma_type;
        typedef emfield::geom_map::association_entry gma_entry_type;
        const gma_type & mfamap = geomap.get_associations();

        for (int i = 0; i < associations.size(); i++) {
          const std::string & association_name = associations[i];
          gma_type::const_iterator found_association = mfamap.find(association_name);
          if (found_association == mfamap.end()) {
            std::ostringstream message;
            message << "Cannot find association with name '" << association_name << "' "
                    << "in field geometry map !";
            if (_abort_on_error_) {
              DT_THROW_IF(true, std::logic_error, message.str());
            }
            continue;
          }
          const gma_entry_type & gefa = found_association->second;
          if (! gefa.has_logvol()) {
            std::ostringstream message;
            message << "Missing G4 logical volume for geom/field association '" << association_name << "' !";
            if (_abort_on_error_) {
              DT_THROW_IF(true, std::logic_error, message.str());
            }
            continue;
          }
          G4String g4_log_name =  gefa.logvol->get_name().c_str();
          G4LogicalVolume * g4_module_log = g4_LV_store->GetVolume(g4_log_name, false);
          if (g4_module_log == 0) {
            std::ostringstream message;
            message << "Missing G4 logical volume with name '" << g4_log_name << "' !";
            if (_abort_on_error_) {
              DT_THROW_IF(true, std::logic_error, message.str());
            }
            DT_LOG_WARNING(logging, message.str());
            continue;
          } else {
            DT_LOG_TRACE(logging," -> G4 logical volume '" << g4_log_name << "' has a magnetic field.");
          }

          magnetic_field * mag_field = new magnetic_field;
          mag_field->loggable_support::_initialize_logging_support(_mag_field_aux_);
          DT_THROW_IF (gefa.field == 0, std::logic_error,
                       "Missing field for geometry/association '" << association_name << "' !");
          DT_THROW_IF (! gefa.field->is_magnetic_field(), std::logic_error,
                       "Field '" << association_name << "' is not a magnetic field !");

          // Check position/time :
          bool cpt = false;
          std::string cpt_key = "check_pos_time." + association_name;
          if (_mag_field_aux_.has_key(cpt_key)) {
            cpt = _mag_field_aux_.fetch_boolean(cpt_key);
          }
          mag_field->set_name(association_name);
          mag_field->set_mag_field(*gefa.field);
          mag_field->set_mag_field_check_pos_time(cpt);
          mag_field->initialize();

          // Use the default miss distance :
          double miss_distance = _general_miss_distance_;
          std::string md_log_key = "miss_distance." + association_name;
          if (_mag_field_aux_.has_key(md_log_key)) {
            miss_distance = _mag_field_aux_.fetch_real(md_log_key);
            if (! _mag_field_aux_.has_explicit_unit(md_log_key)) {
              miss_distance *= _miss_distance_unit_;
            }
          }

          G4FieldManager * detector_field_mgr = new G4FieldManager(mag_field);
          detector_field_mgr->CreateChordFinder(mag_field);
          detector_field_mgr->GetChordFinder()->SetDeltaChord(miss_distance);
          g4_module_log->SetFieldManager(detector_field_mgr, true);
          DT_LOG_NOTICE(logging,
                        "G4 logical volume '" << g4_log_name
                        << "' has a magnetic field named '" << gefa.get_field_name()
                        << "' !");
          DT_LOG_NOTICE(logging, "Miss distance is " << miss_distance / CLHEP::mm << " mm");
        }
        DT_LOG_NOTICE(logging,"Geometry/EM field map has been processed.");
      }

      DT_LOG_TRACE(logging,"Exiting.");
      return;
    }

    //____________________________________
    void detector_construction::_construct_sensitive_detectors()
    {
      DT_LOG_TRACE(_logprio(),"Entering...");
      G4SDManager * SDman = G4SDManager::GetSDMpointer();
      G4LogicalVolumeStore * g4_LV_store = G4LogicalVolumeStore::GetInstance();

      /**************************************************************************
       * Setup the "offical" sensitive categories from the model factory itself *
       **************************************************************************/

      // Loop on sensitive logical volumes :
      for (geomtools::logical_volume::dict_type::const_iterator ilogical
             = _geom_manager_->get_factory().get_logicals().begin();
           ilogical != _geom_manager_->get_factory().get_logicals().end();
           ++ilogical) {
        // Get a reference to the associated logical volume :
        const geomtools::logical_volume & log = *(ilogical->second);

        // Check if it is tagged as a 'sensitive' detector :
        if (! geomtools::sensitive::is_sensitive(log.get_parameters())) {
          DT_LOG_DEBUG(_logprio(),"Logical volume '" << log.get_name() << "' is not sensitive !");
          continue;
        }

        // Get the 'sensitive category' for this detector :
        string sensitive_category
          = geomtools::sensitive::get_sensitive_category(log.get_parameters());
        if (sensitive_category.empty()) {
          DT_LOG_WARNING(_logprio(),"Empty sensitive category name ! Ignore !");
          continue;
        }
        DT_LOG_NOTICE(_logprio(),"Logical volume '" << log.get_name() << "' is sensitive with category '"
                      << sensitive_category << "' !");

        // Pickup the corresponding G4 logical volume :
        G4LogicalVolume * g4_log = 0;
        G4String log_name = log.get_name().c_str();
        g4_log = g4_LV_store->GetVolume(log_name, false);
        if (g4_log == 0) {
          DT_LOG_NOTICE(_logprio(),"Logical volume '" << log.get_name()
                        << "' is not used in G4LogicalVolumeStore !");
          continue;
        }
        sensitive_detector * SD = 0;
        // Search for the sensitive detector that uses this category:
        sensitive_detector_dict_type::iterator found = _sensitive_detectors_.find(sensitive_category);
        bool already_present = false;
        if (found == _sensitive_detectors_.end()) {
          DT_LOG_NOTICE(_logprio(), "Create a new sensitive detector with category '"
                        << sensitive_category << "'");
          SD = new sensitive_detector(sensitive_category);
          SD->set_manager(*_g4_manager_);
          SDman->AddNewDetector(SD);
          _sensitive_detectors_[sensitive_category] = SD;
        } else {
          already_present = true;
          DT_LOG_NOTICE(_logprio(), "Use the sensitive detector with category '"
                        << sensitive_category << "'");
          SD = found->second;
        }

        {
          DT_LOG_NOTICE(_logprio(), "Make the G4 logical volume '" << log.get_name()
                        << "' a sensitive detector with category '"
                        << sensitive_category << "'");
        }

        // update the list of logical volumes(identified by their names) attached
        // to this sensitive detector:
        _sensitive_detectors_[sensitive_category]->attach_logical_volume(log.get_name());

        if (! already_present) {
          // setup special behaviour of the new sensitive detector:
          if (_SD_params_.has_key(sensitive_category)) {
            const datatools::properties & this_SD_params
              = _SD_params_.get(sensitive_category).get_properties();
            SD->configure(this_SD_params);
          }
        }

        // associate this sensitive detector to the G4 logical volume:
        g4_log->SetSensitiveDetector(_sensitive_detectors_[sensitive_category]);

      } // for (logical_volumes)


      /****************************************
       * Setup the step hit processor factory *
       ****************************************/

      // Pass the address of the geometry manager to step hit processors :
      DT_LOG_NOTICE(_logprio(), "SHPF: Preparing the step hit processor factory (SHPF)...");
      // Logging priority:
      datatools::logger::priority lp = datatools::logger::extract_logging_configuration (_SHPF_config_);
      if (lp != datatools::logger::PRIO_UNDEFINED) {
        _SHPF_.set_logging_priority(lp);
      }
      if (this->has_geometry_manager()) {
        _SHPF_.set_geometry_manager(this->get_geometry_manager());
      }
      DT_THROW_IF(! _SHPF_config_.has_key ("config"), std::logic_error,
                  "Missing configuration file for the step hit processor factory !");
      std::string config_file = _SHPF_config_.fetch_string("config");
      datatools::fetch_path_with_env(config_file);
      datatools::multi_properties mconfig("name", "type");
      DT_LOG_NOTICE(_logprio(), "SHPF: Parsing the SHPF configuration file...");
      mconfig.read(config_file);
      DT_LOG_NOTICE(_logprio(), "SHPF: The SHPF configuration file has been parsed.");
      DT_LOG_NOTICE(_logprio(), "SHPF: Loading the hit processors from the SHPF...");
      _SHPF_.load(mconfig);
      DT_LOG_NOTICE(_logprio(), "SHPF: The step hit processors have been loaded from the SHPF.");

      /*****************************************************************
       * Install some step hit processors into the sensitive detectors *
       *****************************************************************/

      /* Look for "non-offical" sensitive categories from the SHPF factory itself.
       * Possibly extends the list of "offical" sensitive detectors
       * from the list of step hit processors found in the SHPF factory
       * and creates the proper association to some logical volumes which must
       * be specified using their respective names.
       */
      for (SHPF_type::processor_dict_type::const_iterator iSHP
             = _SHPF_.get_processors().begin();
           iSHP != _SHPF_.get_processors().end();
           ++iSHP) {
          const string & hit_category = iSHP->second->get_hit_category();
          if (_g4_manager_->forbids_private_hits()) {
            if (boost::starts_with(hit_category, "_")) {
              DT_LOG_WARNING(_logprio(),
                             "SHPF: Sensitive detector '" << iSHP->second->get_name()
                             << "' initialized with private hit category '"
                             << hit_category << "' is not used !");
              continue;
            }
          }

          const string & from_processor_sensitive_category = iSHP->second->get_sensitive_category();

          // If the sensitive_category already exists (from the geometry model setup):
          if (_sensitive_detectors_.find(from_processor_sensitive_category)
              != _sensitive_detectors_.end()) {
            DT_LOG_WARNING(_logprio(),
                           "SHPF: A sensitive detector '" << iSHP->second->get_name()
                           << "' with category '"
                           << from_processor_sensitive_category
                           << "' already exists ! Ignore this rule !");
            continue;
          } else {
            // At this point, we know that some geometry volumes(s)
            // are attached to the newly created sensitive detector:
            DT_LOG_NOTICE(_logprio(),
                          "SHPF: Create a new sensitive detector with category '"
                          << from_processor_sensitive_category << "'");

            sensitive_detector * SD = new sensitive_detector(from_processor_sensitive_category);
            SD->set_manager(*_g4_manager_);

            // Setup special behaviour of the sensitive detector:
            SD->configure(iSHP->second->get_auxiliaries());

            SDman->AddNewDetector(SD);
            _sensitive_detectors_[from_processor_sensitive_category] = SD;

            // Extract the logical volumes this sensitive detector is attached to:
            std::vector<std::string> logicals;

            bool all_logicals = false;
            if (iSHP->second->get_auxiliaries().has_flag("geometry.volumes.all")) {
              all_logicals = true;
            }
            if (all_logicals) {
              for (geomtools::logical_volume::dict_type::const_iterator ilogical
                     = _geom_manager_->get_factory().get_logicals().begin();
                   ilogical != _geom_manager_->get_factory().get_logicals().end();
                   ++ilogical) {
                const std::string & logical_name = ilogical->first;
                if (std::find(logicals.begin(), logicals.end(), logical_name) == logicals.end()) {
                  logicals.push_back(logical_name);
                }
              }
            }

            // Fetch the list of logical volumes associated to this 'sensitive category' :
            if (! all_logicals) {
              if (iSHP->second->get_auxiliaries().has_key("geometry.volumes")) {
                DT_LOG_NOTICE(_logprio(),
                              "SHPF: Fetch the list of logical volumes associated to a sensitive category '"
                              << from_processor_sensitive_category << "'");
                // Logicals are given by name :
                iSHP->second->get_auxiliaries().fetch("geometry.volumes", logicals);
              }
            }

            DT_LOG_NOTICE(_logprio(), "SHPF: Number of logical volumes associated to sensitive category '"
                          << from_processor_sensitive_category << "' : " << logicals.size());

            // Also fetch the list of logical volumes associated to this 'sensitive category'
            // and *with* a given list of materials :
            if (iSHP->second->get_auxiliaries().has_key("geometry.volumes.with_materials")) {
              // Volumes are given by material(s) :
              std::vector<std::string> materials;
              DT_LOG_NOTICE(_logprio(),
                            "SHPF: Fetch the list of materials for sensitive category '"
                            << from_processor_sensitive_category << "'");
              iSHP->second->get_auxiliaries().fetch("geometry.volumes.with_materials", materials);

              ostringstream message;
              message << "SHPF: Searching for logical volumes with material in (";
              for (int j = 0; j < materials.size(); j++) {
                if (j != 0) message << ',';
                message << ' ' << '"' << materials[j] << '"';
              }
              message << ") to be associated to the new sensitive detector with category '"
                      << from_processor_sensitive_category << "'";
              DT_LOG_NOTICE(_logprio(), message.str());

              // Traverse all logical volumes :
              for (geomtools::logical_volume::dict_type::const_iterator ilogical
                     = _geom_manager_->get_factory().get_logicals().begin();
                   ilogical != _geom_manager_->get_factory().get_logicals().end();
                   ++ilogical) {
                const string & logical_name = ilogical->first;
                const geomtools::logical_volume & log = *(ilogical->second);;
                bool checked_material = false;
                std::string mat_name;
                for (int imat = 0; imat < materials.size(); imat++) {
                  mat_name = materials[imat];
                  if (log.get_material_ref() == mat_name) {
                    checked_material = true;
                    break;
                  }
                }
                if (checked_material) {
                  if (std::find(logicals.begin(), logicals.end(), logical_name) == logicals.end()) {
                    std::ostringstream message;
                    message << "SHPF: Associate the logical volume '" << logical_name << "' with material '"
                            << mat_name << "' to the new sensitive detector with category '"
                            << from_processor_sensitive_category << "'";
                    DT_LOG_NOTICE(_logprio(), message.str());
                    logicals.push_back(logical_name);
                  }
                }
              }
            }
            DT_LOG_NOTICE(_logprio(), "SHPF: Number of logical volumes associated to sensitive category '"
                          << from_processor_sensitive_category << "' : " << logicals.size());

            // Skip the logical volumes that should be excluded :
            if (iSHP->second->get_auxiliaries().has_key("geometry.volumes.excluded")) {
              std::vector<std::string> excluded_logicals;
              iSHP->second->get_auxiliaries().fetch("geometry.volumes.excluded", excluded_logicals);
              ostringstream message;
              message << "SHPF: Excluding logical volumes in (";
              for (int j = 0; j < excluded_logicals.size(); j++) {
                if (j != 0) message << ',';
                message << ' ' << '"' << excluded_logicals[j] << '"';
              }
              message << ") from the list of volumes to be associated to the new sensitive detector from SHPF with category '"
                      << from_processor_sensitive_category << "'";
              DT_LOG_NOTICE(_logprio(), message.str());

              for (int ixm = 0; ixm < excluded_logicals.size(); ixm++) {
                logicals.erase(std::remove(logicals.begin(), logicals.end(), excluded_logicals[ixm]), logicals.end());
              }
            }

            if (logicals.size() == 0) {
              DT_LOG_WARNING(_logprio(),
                             "SHPF: New sensitive category '" << from_processor_sensitive_category
                             << "' has no associated logical volumes !");
              continue;
            }

            // Loop on the specified logical volumes and make them sensitive
            // with the new sensitive detector.
            // Detect conflict(more than one sensitive detector per logical volume).
            for (vector<string>::const_iterator ilogical = logicals.begin();
                 ilogical != logicals.end();
                 ++ilogical) {
              const string & logical_name = *ilogical;
              geomtools::logical_volume::dict_type::const_iterator found
                = _geom_manager_->get_factory().get_logicals().find(logical_name);
              if (found == _geom_manager_->get_factory().get_logicals().end()) {
                DT_LOG_WARNING(_logprio(),
                               "SHPF: No logical volume with name '"
                               << logical_name  << "' exists in the geometry manager ! Ignore !");
                continue;
              }
              const geomtools::logical_volume & log = *(found->second);

              // Check sensitivity of the logical volume):
              if (geomtools::sensitive::is_sensitive(log.get_parameters())) {
                DT_LOG_WARNING(_logprio(),
                               "SHPF: Logical volume '"
                               << logical_name  << "' is already associated to sensitive category '"
                               << geomtools::sensitive::get_sensitive_category(log.get_parameters())
                               << "' !");
                continue;
              }

              // pickup the corresponding G4 logical volume:
              G4LogicalVolume * g4_log = 0;
              G4String log_name = log.get_name().c_str();
              g4_log = g4_LV_store->GetVolume(log_name, false);
              if (g4_log == 0) {
                DT_LOG_WARNING(_logprio(),
                               "SHPF: No logical volume has been found '" << log.get_name()
                               << "' in the G4LogicalVolumeStore !");
                continue;
              }

              // Local scope:
              {
                // Makes the logical sensitive (using a trick because of the const-ness
                // of the logical volume instance after building the model factory from
                // the geometry manager) :
                geomtools::logical_volume * mutable_log
                  = const_cast<geomtools::logical_volume *>(&log);
                geomtools::sensitive::set_sensitive_category(mutable_log->grab_parameters(),
                                                             from_processor_sensitive_category);
              }

              ostringstream message;
              message << "SHPF: Make the G4 logical volume '" << log.get_name()
                      << "' a sensitive detector with category '"
                      << from_processor_sensitive_category << "'";
              DT_LOG_NOTICE(_logprio(), message.str());
              _sensitive_detectors_[from_processor_sensitive_category]->attach_logical_volume(log.get_name());

              g4_log->SetSensitiveDetector(_sensitive_detectors_[from_processor_sensitive_category]);
            }
          }
        } // for (hit processors)

      // Associate a sensitive detector to a particular step hit processor
      // from the factory:
      for (SHPF_type::processor_dict_type::const_iterator iSHP
             = _SHPF_.get_processors().begin();
           iSHP != _SHPF_.get_processors().end();
           ++iSHP) {
        const string & processor_name = iSHP->first;
        const string & hit_category = iSHP->second->get_hit_category();
        if (_g4_manager_->forbids_private_hits()) {
          if (boost::starts_with(hit_category, "_")) {
            DT_LOG_WARNING(_logprio(), "SHPF: Sensitive detector '" << iSHP->second->get_name()
                           << "' initialized from SHPF with private hit category '"
                           << hit_category << "' is not used !");
            continue;
          }
        }
        const string & from_processor_sensitive_category = iSHP->second->get_sensitive_category();
        sensitive_detector_dict_type::iterator iSD
          = _sensitive_detectors_.find(from_processor_sensitive_category);
        if (iSD == _sensitive_detectors_.end()) {
          continue;
        }
        sensitive_detector * SD = iSD->second;
        SD->add_hit_processor(processor_name, *iSHP->second);
      } // SHPF

      // Dump:
      if (is_verbose()) {
        for (detector_construction::sensitive_detector_dict_type::iterator iSD
               = _sensitive_detectors_.begin();
             iSD != _sensitive_detectors_.end();
             ++iSD) {
          const string & sensitive_category = iSD->first;
          sensitive_detector * SD = iSD->second;
          DT_LOG_NOTICE(_logprio(),"Sensitive detector '" << sensitive_category << "' : ");
          SD->tree_dump(clog, "");
        }
      }

      DT_LOG_TRACE(_logprio(),"Exiting.");
      return;
    }

    //____________________________________
    void detector_construction::set_materials_geom_plugin_name(const std::string & mpn_)
    {
      DT_THROW_IF (is_initialized(), std::logic_error, "Operation prohibited !");
      _materials_geom_plugin_name_ = mpn_;
      return;
    }

    //____________________________________
    void detector_construction::set_emfield_geom_plugin_name(const std::string & fpn_)
    {
      DT_THROW_IF (is_initialized(), std::logic_error, "Operation prohibited !");
      _emfield_geom_plugin_name_ = fpn_;
      return;
    }

    //____________________________________
    void detector_construction::write_tmp_gdml_file()
    {

      DT_THROW_IF (! _geom_manager_->is_initialized(), std::logic_error,
                   "Geometry manager is not initialized !");
      ostringstream tmp_file_template_oss;
      if (! _gdml_file_dir_.empty()) {
        if (! boost::filesystem::is_directory(_gdml_file_dir_)) {
          bool do_mkdir = true;
          if (do_mkdir) {
            DT_LOG_WARNING(_logprio(), "Attempt to create the temporary directory '"
                           << _gdml_file_dir_ << "'...");
            ostringstream syscmd;
            syscmd << "mkdir -p " << _gdml_file_dir_;
            int syserr = system(syscmd.str().c_str());
            DT_THROW_IF (syserr == -1, std::runtime_error,
                         "Cannot create temporary directory '" << _gdml_file_dir_ << "' !");
          } else {
            DT_THROW_IF(true, logic_error, "Temporary directory '" << _gdml_file_dir_ << "' does not exist !");
          }
        }
        tmp_file_template_oss << _gdml_file_dir_ << '/';
      }
      tmp_file_template_oss << "mctools_g4_geometry.gdml." << "XXXXXX" << ends;
      string tmp_file_template = tmp_file_template_oss.str();
      char tmp_gdml_filename[1024];
      copy(tmp_file_template.begin(), tmp_file_template.end(), tmp_gdml_filename);

      int err = mkstemp(tmp_gdml_filename);
      DT_THROW_IF (err == -1, runtime_error, "Cannot create GDML temporary file !");
      _gdml_filename_ = tmp_gdml_filename;


      // The GDML export object :
      geomtools::gdml_export GDML;

      /*******************************************************************/
      geomtools::gdml_writer materials_writer; // GDML writer for materials
      const materials::manager * mat_mgr_ref = 0;
      std::string materials_plugin_name = _materials_geom_plugin_name_;
      const geomtools::manager & geo_mgr = get_geometry_manager();
      if (materials_plugin_name.empty()) {
        // We try to find a material plugin associated to the manager :
        typedef geomtools::manager::plugins_dict_type dict_type;
        const dict_type & plugins = geo_mgr.get_plugins();
        for (dict_type::const_iterator ip = plugins.begin();
             ip != plugins.end();
             ip++){
          const std::string & plugin_name = ip->first;
          if (geo_mgr.is_plugin_a<geomtools::materials_plugin>(plugin_name)) {
            materials_plugin_name = plugin_name;
            break;
          }
        }
      }
      // Access to a given plugin by name and type :
      if (geo_mgr.has_plugin(materials_plugin_name)
          && geo_mgr.is_plugin_a<geomtools::materials_plugin>(materials_plugin_name)) {
        DT_LOG_NOTICE(_logprio(), "Found materials plugin named '" << materials_plugin_name << "'");
        const geomtools::materials_plugin & mgp
          = geo_mgr.get_plugin<geomtools::materials_plugin>(materials_plugin_name);
        const materials::manager & mat_mgr = mgp.get_manager();
        mat_mgr_ref = &mat_mgr;
      }
      DT_THROW_IF (mat_mgr_ref == 0,std::logic_error,"Cannot find a material driver plugin in the geometry manager !");
      DT_LOG_NOTICE(_logprio(), "Export GDML materials from the materials driver plugin: ");
      geomtools::export_gdml(*mat_mgr_ref, materials_writer);
      GDML.attach_external_materials(materials_writer.get_stream(geomtools::gdml_writer::MATERIALS_SECTION));

      GDML.add_auxiliary_support(false);
      GDML.add_replica_support(false);
      GDML.parameters().store("xml_version",  geomtools::gdml_writer::DEFAULT_XML_VERSION);
      GDML.parameters().store("xml_encoding", geomtools::gdml_writer::DEFAULT_XML_ENCODING);

      GDML.parameters().store("gdml_schema",  _gdml_schema_);

      GDML.parameters().store("length_unit",  geomtools::gdml_export::DEFAULT_LENGTH_UNIT);
      GDML.parameters().store("angle_unit",   geomtools::gdml_export::DEFAULT_ANGLE_UNIT);
      GDML.export_gdml(_gdml_filename_, _geom_manager_->get_factory(), "world");
      return;
    }

    //____________________________________
    void detector_construction::_set_user_limits()
    {
      DT_LOG_TRACE(_logprio(),  "Entering...");
      G4LogicalVolumeStore * g4_LV_store = G4LogicalVolumeStore::GetInstance();

      std::vector<std::string> log_volumes;
      if (_limits_config_.has_key("limits.list_of_volumes")) {
        _limits_config_.fetch("limits.list_of_volumes", log_volumes);
      }
      if (log_volumes.size() == 0) {
        return;
      }
      // const geomtools::models_col_type & models_dict
      //   = _geom_manager_->get_factory().get_models();
      for (unsigned int i = 0; i < log_volumes.size(); ++i) {
        const std::string & log_name = log_volumes[i];
        geomtools::logical_volume::dict_type::const_iterator found
          = _geom_manager_->get_factory().get_logicals().find(log_name);
        //geomtools::models_col_type::const_iterator found = models_dict.find(model_name);
        if (found == _geom_manager_->get_factory().get_logicals().end()) {
          //DT_LOG_WARNING(_logprio(), "Cannot find logical volume '" << log_name << "' !");
          DT_THROW_IF(true, std::logic_error,
                      "Cannot find logical volume '" << log_name << "' for user limits !");
          continue;
        }

        //string log_name = geomtools::i_model::make_logical_volume_name(model_name);
        G4String g4_log_name = log_name.c_str();
        G4LogicalVolume * g4_module_log = g4_LV_store->GetVolume(g4_log_name, false);
        if (g4_module_log == 0) {
          //DT_LOG_WARNING(_logprio(), "Missing G4 logical volume with name '" << g4_log_name << "' !");
          DT_THROW_IF(true, std::logic_error,
                      "Missing G4 logical volume with name '"
                      << g4_log_name << "' for user limits !");
          continue;
        }

        // search for maximum step length:
        double max_step = DBL_MAX;
        std::ostringstream prop_name;
        prop_name << "limits.max_step." << log_name;
        if (_limits_config_.has_key(prop_name.str())) {
          max_step = _limits_config_.fetch_real(prop_name.str());
          if (! _limits_config_.has_explicit_unit(prop_name.str())) {
            max_step *= CLHEP::mm;
          }
          G4UserLimits * g4_user_limits = new G4UserLimits(max_step);
          _user_limits_col_.push_back(g4_user_limits);
          g4_module_log->SetUserLimits(g4_user_limits);
          DT_LOG_NOTICE(_logprio(), "Installing user limits for G4 logical volume with name '" << g4_log_name << "' with max step = " << max_step / CLHEP::mm << " mm");
        }

      }

      DT_LOG_TRACE(_logprio(),"Exiting.");
      return;
    }

    void detector_construction::_construct_regions()
    {
      G4LogicalVolumeStore * g4_LV_store = G4LogicalVolumeStore::GetInstance();
      for (region_infos_dict_type::const_iterator i = _region_infos_.begin();
           i != _region_infos_.end();
           ++i) {
        const string & the_region_name = i->first;
        const vector<string> & the_region_infos = i->second;

        G4Region * the_region = new G4Region(the_region_name);

        for (unsigned int j = 0; j < the_region_infos.size(); ++j) {
          G4LogicalVolume * a_logical = 0;
          string logical_volume_name = the_region_infos[j];
          G4String g4_logical_volume_name = logical_volume_name.c_str();
          a_logical = g4_LV_store->GetVolume(g4_logical_volume_name, false);
          if (a_logical != 0) {
            the_region->AddRootLogicalVolume(a_logical);
          } else {
            // DT_LOG_WARNING(_logprio(),"No logical volume named '"
            //                << logical_volume_name << "' to be added in the '"
            //                << the_region_name << "' region !");
            DT_THROW_IF(true,std::logic_error, "No logical volume named '"
                           << logical_volume_name << "' to be added in the '"
                           << the_region_name << "' region !");
          }
        }
      }
      return;
    }

    //____________________________________
    void detector_construction::_set_visualization_attributes()
    {
      DT_LOG_TRACE(_logprio(),"Entering...");

      // Look for visibility properties :
      /*
      for (geomtools::models_col_type::const_iterator i
             = _geom_manager_->get_factory().get_models().begin();
           i != _geom_manager_->get_factory().get_models().end();
           ++i) {
        const geomtools::i_model & model = *(i->second);
        const geomtools::logical_volume & log = model.get_logical();
      */
      for (geomtools::logical_volume::dict_type::const_iterator ilogical
             = _geom_manager_->get_factory().get_logicals().begin();
           ilogical != _geom_manager_->get_factory().get_logicals().end();
           ++ilogical) {
        // Get a reference to the associated logical volume :
        const geomtools::logical_volume & log = *(ilogical->second);
        bool   visible = true;
        bool   daughters_visible = true;
        string display_color;
        bool   force_wire_frame = true;
        bool   force_solid = false;
        if (geomtools::visibility::is_hidden(log.get_parameters())) {
          visible = false;
          daughters_visible = false;
        } else {
          if (geomtools::visibility::is_hidden_envelope(log.get_parameters())) {
            visible = false;
          }
          if (geomtools::visibility::is_daughters_hidden(log.get_parameters())) {
            daughters_visible = false;
          }
          if (geomtools::visibility::has_color(log.get_parameters())) {
            display_color = geomtools::visibility::get_color(log.get_parameters());
          }
          /*
            if (geomtools::visibility::has_key(log.get_parameters(), "opacity"))
            {
            // Not supported.
            //display_opacity = geomtools::visibility::get_color(log.get_parameters());
            }
          */
        }
        G4LogicalVolume * g4_log = 0;
        G4LogicalVolumeStore * g4_LV_store = G4LogicalVolumeStore::GetInstance();
        G4String log_name = log.get_name().c_str();
        g4_log = g4_LV_store->GetVolume(log_name, false);
        if (g4_log != 0) {
          G4VisAttributes * va = new G4VisAttributes;
          if (! visible) {
            va->SetVisibility(false);
          } else {
            va->SetVisibility(true);
          }
          if (! daughters_visible) {
            va->SetDaughtersInvisible(true);
          } else {
            va->SetDaughtersInvisible(false);
          }
          va->SetForceWireframe(force_wire_frame);
          va->SetForceSolid(force_solid);
          G4Colour color; // default == white
          if (! display_color.empty()) {
            const geomtools::color::constants & gcc = geomtools::color::constants::instance();
            if (display_color == gcc.white)   color = G4Colour::White();
            if (display_color == gcc.grey)    color = G4Colour::Grey();
            if (display_color == gcc.black)   color = G4Colour::Black();
            if (display_color == gcc.red)     color = G4Colour::Red();
            if (display_color == gcc.green)   color = G4Colour::Green();
            if (display_color == gcc.blue)    color = G4Colour::Blue();
            if (display_color == gcc.cyan)    color = G4Colour::Cyan();
            if (display_color == gcc.magenta) color = G4Colour::Magenta();
            if (display_color == gcc.yellow)  color = G4Colour::Yellow();
            if (display_color == gcc.orange)  color = G4Colour(1.0, 0.66, 0.0);
          }
          if (visible) {
            va->SetColor(color);
          }
          // push the G4VisAttributes in the map for persistency in memory:
          _vis_attributes_[log.get_name()] = va;
          g4_log->SetVisAttributes(va);
        } else {
          DT_LOG_DEBUG(_logprio(),
                       "Cannot find logical volume '" << log.get_name() << "' in G4LogicalVolumeStore !");
        }

      }
      DT_LOG_TRACE(_logprio(),"Exiting.");
      return;
    }

  } // end of namespace g4

} // end of namespace mctools

/** Opening macro for implementation
 *  This macro must be used outside of any namespace.
 */
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(mctools::g4::detector_construction,ocd_)
{
  // The class name :
  ocd_.set_class_name ("mctools::g4::detector_construction");

  // The class terse description :
  ocd_.set_class_description ("The Geant4 simulation mandatory detector construction");

  // The library the class belongs to :
  ocd_.set_class_library ("mctools_g4");

  // The class detailed documentation :
  ocd_.set_class_documentation ("The Geant4 simulation engine/manager needs to be given a *detector construction* \n"
                                "object. This complex object is responsible of some fundamental operations\n"
                                "before to run the simulation process :                                   \n"
                                "                                                                         \n"
                                " * initialization of the virtual geometry tree thanks to                 \n"
                                "   the GDML export feature of the ``geomtools::manager`` class           \n"
                                "   used here as the main geometry modelling tools.                       \n"
                                " * initialization of detector *regions* where one can apply special      \n"
                                "   user cuts for the production of secondary particles.                  \n"
                                " * initialization of special *user limits* in some geometry volumes      \n"
                                "   (use maximum step length).                                            \n"
                                " * initialization of magnetic field associated to some geometry volumes  \n"
                                "   using a ``emfield::electromagnetic_field_manager`` object provided    \n"
                                "   a dedicated plugin in the geometry manager.                           \n"
                                " * initialization of the *step hit processor factory* for post-processing\n"
                                "   of truth/step hits generated by sensitive detector within their       \n"
                                "   associated volumes.                                                   \n"
                                " * initialization of visualization attributes for Geant4 3D rendering.   \n"
                                "                                                                         \n"
                               );

  /********
   * GDML *
   ********/

  {
    // Description of the 'gdml.tmp_dir' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("gdml.tmp_dir")
      .set_terse_description("The directory where the GDML file is generated by the ``geomtools::manager``")
      .set_traits(datatools::TYPE_STRING)
      .set_path(true)
      .set_long_description("Default value: empty (match the current directory)            \n"
                            "                                                              \n"
                            "Example::                                                     \n"
                            "                                                              \n"
                            "  gdml.tmp_dir : string as path = \"/tmp/g4_work\"            \n"
                            "                                                              \n"
                            )
      ;
  }

  {
    // Description of the 'gdml.no_validation' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("gdml.validation")
      .set_terse_description("Flag to proceed to the validation of the GDML file")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_long_description("Default value: 1                                              \n"
                            "                                                              \n"
                            "Example::                                                     \n"
                            "                                                              \n"
                            "  gdml.validation : boolean = 1                               \n"
                            "                                                              \n"
                            )
      ;
  }

  {
    // Description of the 'gdml.schema_location' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("gdml.schema_location")
      .set_terse_description("The label indicating where to find the GDML schema resources")
      .set_traits(datatools::TYPE_STRING)
      .set_long_description("Allowed values: ``\"local\"`` , ``\"remote\"``                \n"
                            "                                                              \n"
                            "Default value: ``\"remote\"`` (needs access to the Internet)  \n"
                            "                                                              \n"
                            "Example::                                                     \n"
                            "                                                              \n"
                            "  gdml.schema_location : string = \"local\"                   \n"
                            "                                                              \n"
                            )
      ;
  }

  {
    // Description of the 'materials.plugin_name' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("materials.plugin_name")
      .set_terse_description("The name of the materials plugin embedded in the geometry manager for materials export in GDML")
      .set_traits(datatools::TYPE_STRING)
      .set_long_description("Default value: empty (plugin is autodetected from the geometry manager)\n"
                            "                                                              \n"
                            "Example::                                                     \n"
                            "                                                              \n"
                            "  materials.plugin_name : string = \"materials_driver\"       \n"
                            "                                                              \n"
                            )
      ;
  }

  /*************************
   *  Sensitive detectors  *
   *************************/

  {
    // Description of the 'sensitive.detectors' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("sensitive.detectors")
      .set_terse_description("The list of sensitive detectors attached to the geometry")
      .set_traits(datatools::TYPE_STRING,
                  configuration_property_description::ARRAY)
      .set_long_description("This is the list of so-called *official* sensitive detectors in the   \n"
                            "geometry setup. Each sensitive detectors is identified by its unique  \n"
                            "name.                                                                 \n"
                            "                                                                      \n"
                            "Default value: empty                                                  \n"
                            "                                                                      \n"
                            "Example::                                                             \n"
                            "                                                                      \n"
                            "  sensitive.detectors : string[2] = \"calo_SD\" \"tracker_SD\"        \n"
                            "                                                                      \n"
                            "The configuration parameters of a given sensitive detector are        \n"
                            "provided through additionnal properties with the following syntax ::  \n"
                            "                                                                      \n"
                            " sensitive.SDNAME.KEY1 : TYPE1 = VALUE1                     \n"
                            " sensitive.SDNAME.KEY2 : TYPE2 = VALUE2                     \n"
                            "                                                                      \n"
                            "where ``SDNAME`` is the name of the sensitive detector                \n"
                            "and ``KEY1``, ``TYPE1`` and  ``VALUE1`` are the property's name, type \n"
                            "respectively. Please refer to the OCD manual of the                   \n"
                            "``mctools::g4::sensitive_detector`` class for further documentation   \n"
                            "about the available setup parameters for *sensitive detector* objects.\n"
                            )
      ;
  }

  {
    // Description of the 'sensitive.XXX.record_track_id' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("sensitive.${sensitive.detectors}.record_track_id")
      .set_terse_description("Record the *track ID* attached to a given sensitive detector")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_long_description("Default value: 0                                                      \n"
                            "                                                                      \n"
                            "Example::                                                             \n"
                            "                                                                      \n"
                            "  sensitive.detectors : string[2] = \"calo_SD\" \"tracker_SD\"        \n"
                            "  sensitive.calo_SD.record_track_id    : boolean = 0                  \n"
                            "  sensitive.tracker_SD.record_track_id : boolean = 1                  \n"
                            "                                                                      \n"
                            "See OCD manual of the ``mctools::g4::sensitive_detector`` class.      \n"
                            )
      ;
  }

  {
    // Description of the 'sensitive.XXX.record_primary_particle' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("sensitive.${sensitive.detectors}.record_primary_particle")
      .set_terse_description("Record *primary particle* flag attached to a given sensitive detector")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_long_description("Default value: 0                                                      \n"
                            "                                                                      \n"
                            "Example::                                                             \n"
                            "                                                                      \n"
                            "  sensitive.detectors : string[2] = \"calo_SD\" \"tracker_SD\"        \n"
                            "  sensitive.calo_SD.record_primary_particle    : boolean = 1          \n"
                            "  sensitive.tracker_SD.record_primary_particle : boolean = 0          \n"
                            "                                                                      \n"
                            "See OCD manual of the ``mctools::g4::sensitive_detector`` class.      \n"
                            )
      ;
  }

  {
    // Description of the 'sensitive.XXX.record_alpha_quenching' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("sensitive.${sensitive.detectors}.record_alpha_quenching")
      .set_terse_description("Record the *alpha quenching* flag attached to a given sensitive detector")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_long_description("Default value: 0                                                      \n"
                            "                                                                      \n"
                            "Example::                                                             \n"
                            "                                                                      \n"
                            "  sensitive.detectors : string[2] = \"calo_SD\" \"tracker_SD\"        \n"
                            "  sensitive.calo_SD.record_alpha_quenching    : boolean = 1           \n"
                            "  sensitive.tracker_SD.record_alpha_quenching : boolean = 0           \n"
                            "                                                                      \n"
                            "See OCD manual of the ``mctools::g4::sensitive_detector`` class.      \n"
                           )
      ;
  }

  {
    // Description of the 'sensitive.XXX.record_major_track' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("sensitive.${sensitive.detectors}.record_major_track")
      .set_terse_description("Record the *major track* flag attached to a given sensitive detector")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_long_description("Default value: 0                                                      \n"
                            "                                                                      \n"
                            "Example::                                                             \n"
                            "                                                                      \n"
                            "  sensitive.detectors : string[2] = \"calo_SD\" \"tracker_SD\"        \n"
                            "  sensitive.calo_SD.record_major_track    : boolean = 1               \n"
                            "  sensitive.tracker_SD.record_major_track : boolean = 0               \n"
                            "                                                                      \n"
                            "See OCD manual of the ``mctools::g4::sensitive_detector`` class.      \n"
                           )
      ;
  }

  {
    // Description of the 'sensitive.XXX.major_track_minimum_energy' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("sensitive.${sensitive.detectors}.major_track_minimum_energy")
      .set_terse_description("Energy threshold to consider a track as a *major* one")
      .set_traits(datatools::TYPE_REAL)
      .set_explicit_unit(true)
      .set_unit_label("energy")
      .set_long_description("Default value: 0                                                          \n"
                            "                                                                          \n"
                            "Example::                                                                 \n"
                            "                                                                          \n"
                            "  sensitive.detectors : string[2] = \"calo_SD\" \"tracker_SD\"            \n"
                            "  sensitive.calo_SD.record_major_track         : boolean = 1              \n"
                            "  sensitive.calo_SD.major_track_minimum_energy : real as energy = 50 keV  \n"
                            "                                                                          \n"
                            "See OCD manual of the ``mctools::g4::sensitive_detector`` class.      \n"
                            )
      ;
  }

  {
    // Description of the 'sensitive.XXX.record_category' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("sensitive.${sensitive.detectors}.record_category")
      .set_terse_description("Record the *sensitive category* attached to a given sensitive detector")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_long_description("Default value: 0                                                      \n"
                            "                                                                      \n"
                            "Example::                                                             \n"
                            "                                                                      \n"
                            "  sensitive.detectors : string[2] = \"calo_SD\" \"tracker_SD\"        \n"
                            "  sensitive.calo_SD.record_category    : boolean = 1                  \n"
                            "  sensitive.tracker_SD.record_category : boolean = 1                  \n"
                            "                                                                      \n"
                            "See OCD manual of the ``mctools::g4::sensitive_detector`` class.      \n"
                            )
      ;
  }

  {
    // Description of the 'sensitive.XXX.record_momentum' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("sensitive.${sensitive.detectors}.record_momentum")
      .set_terse_description("Record the start and stop *momenta* of the step for a given sensitive detector")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_long_description("Default value: 0                                                      \n"
                            "                                                                      \n"
                            "Example::                                                             \n"
                            "                                                                      \n"
                            "  sensitive.detectors : string[2] = \"calo_SD\" \"tracker_SD\"        \n"
                            "  sensitive.calo_SD.record_momentum    : boolean = 0                  \n"
                            "  sensitive.tracker_SD.record_momentum : boolean = 1                  \n"
                            "                                                                      \n"
                            "See OCD manual of the ``mctools::g4::sensitive_detector`` class.      \n"
                            )
      ;
  }

  {
    // Description of the 'sensitive.XXX.record_kinetic_energy' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("sensitive.${sensitive.detectors}.record_kinetic_energy")
      .set_terse_description("Record the start and stop *kinetic energy* of the step for a given sensitive detector")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_long_description("Default value: 0                                                      \n"
                            "                                                                      \n"
                            "Example::                                                             \n"
                            "                                                                      \n"
                            "  sensitive.detectors : string[2] = \"calo_SD\" \"tracker_SD\"        \n"
                            "  sensitive.calo_SD.record_kinetic_energy    : boolean = 1            \n"
                            "  sensitive.tracker_SD.record_kinetic_energy : boolean = 0            \n"
                            "                                                                      \n"
                            "See OCD manual of the ``mctools::g4::sensitive_detector`` class.      \n"
                           )
      ;
  }

  {
    // Description of the 'sensitive.XXX.hits_buffer_capacity' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("sensitive.${sensitive.detectors}.hits_buffer_capacity")
      .set_terse_description("Pre-allocate the initial capacity of a buffer of step hits associated to a given sensitive detector")
      .set_traits(datatools::TYPE_INTEGER)
      .set_long_description("Default value: 1000                                                   \n"
                            "                                                                      \n"
                            "Example::                                                             \n"
                            "                                                                      \n"
                            "  sensitive.detectors : string[2] = \"calo_SD\" \"tracker_SD\"        \n"
                            "  sensitive.calo_SD.hits_buffer_capacity    : integer = 30            \n"
                            "  sensitive.tracker_SD.hits_buffer_capacity : integer = 100           \n"
                            "                                                                      \n"
                            "See OCD manual of the ``mctools::g4::sensitive_detector`` class.      \n"
                            )
      ;
  }

  {
    // Description of the 'sensitive.XXX.drop_zero_energy_deposit_steps' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("sensitive.${sensitive.detectors}.drop_zero_energy_deposit_steps")
      .set_terse_description("Drop zero energy deposit step hits for a given sensitive detector")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_long_description("Default value: 0                                                      \n"
                            "                                                                      \n"
                            "Example::                                                             \n"
                            "                                                                      \n"
                            "  sensitive.detectors : string[2] = \"calo_SD\" \"tracker_SD\"        \n"
                            "  sensitive.calo_SD.drop_zero_energy_deposit_steps    : boolean = 1   \n"
                            "  sensitive.tracker_SD.drop_zero_energy_deposit_steps : boolean = 0   \n"
                            "                                                                      \n"
                            "See OCD manual of the ``mctools::g4::sensitive_detector`` class.      \n"
                            )
      ;
  }

  {
    // Description of the 'sensitive.XXX.record_g4_volume_infos' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("sensitive.${sensitive.detectors}.record_g4_volume_infos")
      .set_terse_description("Record the Geant4 geometry volume informations for a given sensitive detector")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_long_description("Default value: 0                                                      \n"
                            "                                                                      \n"
                            "Example::                                                             \n"
                            "                                                                      \n"
                            "  sensitive.detectors : string[2] = \"calo_SD\" \"tracker_SD\"        \n"
                            "  sensitive.calo_SD.record_g4_volume_infos    : boolean = 1           \n"
                            "  sensitive.tracker_SD.record_g4_volume_infos : boolean = 1           \n"
                            "                                                                      \n"
                            "See OCD manual of the ``mctools::g4::sensitive_detector`` class.      \n"
                            )
      ;
  }

  /***********
   * Regions *
   ***********/

  {
    // Description of the 'using_regions' volumes' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("using_regions")
      .set_terse_description("Flag to activate regions")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_long_description("Default value: 1 (use regions)                                        \n"
                            "                                                                      \n"
                            "Example::                                                             \n"
                            "                                                                      \n"
                            "  using_regions : boolean = 1                                         \n"
                            "                                                                      \n"
                            )
      ;
  }

  {
    // Description of the 'regions' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("regions")
      .set_terse_description("The list of regions attached to the geometry")
      .set_traits(datatools::TYPE_STRING,
                  configuration_property_description::ARRAY)
      .set_long_description("Default value: empty                                                  \n"
                            "                                                                      \n"
                            "Example::                                                             \n"
                            "                                                                      \n"
                            "  regions : string[2] = \"calo\" \"tracker\"                          \n"
                            "                                                                      \n"
                            )
      ;
  }

  {
    // Description of the 'regions' volumes' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("regions.${regions}.volumes")
      .set_terse_description("The list of volumes attached to a region referenced through its name")
      .set_traits(datatools::TYPE_STRING,
                  configuration_property_description::ARRAY)
      .set_mandatory(false)
      .set_long_description("Default value: empty                                                  \n"
                            "                                                                      \n"
                            "Example::                                                             \n"
                            "                                                                      \n"
                            "  regions : string[2] = \"calo\" \"tracker\"                          \n"
                            "  regions.calo.volumes    : string[2] = \"calo0.log\" \"calo1.log\"   \n"
                            "  regions.tracker.volumes : string[1] = \"cells.log\"                 \n"
                            "                                                                      \n"
                            )
      ;
  }

  {
    // Description of the 'regions' volumes.with_material' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("regions.${regions}.volumes.with_materials")
      .set_terse_description("The list of volumes with some given materials attached to a region referenced through its name")
      .set_traits(datatools::TYPE_STRING,
                  configuration_property_description::ARRAY)
      .set_mandatory(false)
      .set_long_description("Default value: empty                                                     \n"
                            "                                                                         \n"
                            "Example::                                                                \n"
                            "                                                                         \n"
                            "  regions : string[2] = \"calo\" \"tracker\"                             \n"
                            "  regions.calo.volumes.with_materials    : string[2] = \"PVT\" \"NE110\" \n"
                            "  regions.tracker.volumes.with_materials : string = \"tracking_gas\"     \n"
                            "                                                                         \n"
                            )
      ;
  }

  /***************
   * User limits *
   ***************/

  {
    // Description of the 'using_user_limits' volumes' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("using_user_limits")
      .set_terse_description("Flag to activate user limits")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_long_description("Default value: 1 (use user limits)                                    \n"
                            "                                                                      \n"
                            "Example::                                                             \n"
                            "                                                                      \n"
                            "  using_user_limits : boolean = 1                                     \n"
                            "                                                                      \n"
                            )
      ;
  }

  {
    // Description of the 'limits.list_of_volumes' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("limits.list_of_volumes")
      .set_terse_description("The list of volumes where to apply user limits")
      .set_traits(datatools::TYPE_STRING,
                  configuration_property_description::ARRAY)
      .set_long_description("Default value: empty                                                           \n"
                            "                                                                               \n"
                            "Example::                                                                      \n"
                            "                                                                               \n"
                            "  limits.list_of_volumes : string[2] = \"calo_block.log\" \"tracker_cell.log\" \n"
                            "                                                                               \n"
                            )
      ;
  }

  {
    // Description of the 'limits.max_step.XXX' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("limits.max_step.${limits.list_of_volumes}")
      .set_terse_description("The maximum tracking step length associated to a given volume")
      .set_traits(datatools::TYPE_REAL)
      .set_explicit_unit(true)
      .set_unit_label("length")
      .set_long_description("Default value: empty                                                           \n"
                            "                                                                               \n"
                            "Example::                                                                      \n"
                            "                                                                               \n"
                            "  limits.list_of_volumes : string[2] = \"calo_block.log\" \"tracker_cell.log\" \n"
                            "  limits.max_step.calo_block.log   : real as length = 0.5 mm                   \n"
                            "  limits.max_step.tracker_cell.log : real as length = 5 mm                     \n"
                            "                                                                               \n"
                            )
      ;
  }



  /******************
   * Magnetic field *
   ******************/

  {
    // Description of the 'using_magnetic_field' volumes' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("using_magnetic_field")
      .set_terse_description("Flag to activate a magnetic field")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_long_description("Default value: 0 (no magnetic field)                                  \n"
                            "                                                                      \n"
                            "Example::                                                             \n"
                            "                                                                      \n"
                            "  using_magnetic_field : boolean = 0                                  \n"
                            "                                                                      \n"
                            )
      ;
  }

  {
    // Description of the 'magnetic_field.plugin_name' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("magnetic_field.plugin_name")
      .set_terse_description("The name of the geometry magnetic field plugin embedded in the geometry manager")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(true)
      .set_triggered_by_flag("using_magnetic_field")
      .set_long_description("Default value:  (no magnetic field)                                   \n"
                            "                                                                      \n"
                            "Example::                                                             \n"
                            "                                                                      \n"
                            "  magnetic_field.plugin_name : string = \"coil_driver\"               \n"
                            "                                                                      \n"
                            )
      ;
  }

  {
    // Description of the 'magnetic_field.miss_distance.unit' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("magnetic_field.miss_distance.unit")
      .set_terse_description("The default implicit magnetic field miss distance unit")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_triggered_by_flag("using_magnetic_field")
      .set_long_description("Default value: ``\"mm\"``                                             \n"
                            "                                                                      \n"
                            "Example::                                                             \n"
                            "                                                                      \n"
                            "  magnetic_field.miss_distance.unit : string = \"mm\"                 \n"
                            "                                                                      \n"
                            )
      ;
  }

  {
    // Description of the 'magnetic_field.miss_distance.unit' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("magnetic_field.miss_distance")
      .set_terse_description("The global magnetic field miss distance")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(false)
      .set_triggered_by_flag("using_magnetic_field")
      .set_long_description("Default value: ``\"mm\"``                                             \n"
                            "                                                                      \n"
                            "Example::                                                             \n"
                            "                                                                      \n"
                            "  magnetic_field.miss_distance : real as length = 1.2 mm              \n"
                            "                                                                      \n"
                            )
      .set_explicit_unit(true)
      .set_unit_label("length")
      ;
  }

  {
    // Description of the 'logging.priority' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("magnetic_field.logging.priority")
      .set_terse_description("Logging priority threshold of the magnetic field manager")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description("Allowed values are:                                      \n"
                            "                                                         \n"
                            " * ``\"fatal\"``       : print fatal error messages      \n"
                            " * ``\"critical\"``    : print critical error messages   \n"
                            " * ``\"error\"``       : print error messages            \n"
                            " * ``\"warning\"``     : print warnings                  \n"
                            " * ``\"notice\"``      : print notice messages           \n"
                            " * ``\"information\"`` : print informational messages    \n"
                            " * ``\"debug\"``       : print debug messages            \n"
                            " * ``\"trace\"``       : print trace messages            \n"
                            "                                                         \n"
                            "Default value: ``\"warning\"``                           \n"
                            "                                                         \n"
                            "Example::                                                \n"
                            "                                                         \n"
                            "  magnetic_field.logging.priority : string = \"warning\" \n"
                            "                                                         \n"
                            )
      ;
  }

  {
    // Description of the 'magnetic_field.associations' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("magnetic_field.associations")
      .set_terse_description("List of geom./mag. field associations with special criteria")
      .set_traits(datatools::TYPE_STRING,
                  configuration_property_description::ARRAY)
      .set_mandatory(false)
      .set_long_description("Example::                                                           \n"
                            "                                                                    \n"
                            "  magnetic_field.associations : string[2] = \"coil\" \"lab\"        \n"
                            "                                                                    \n"
                            "Note: the geometry volume/magnetic field association must exist in  \n"
                            "the dedicated plugin.                                               \n"
                            )
      ;
  };

  {
    // Description of the 'magnetic_field.check_pos_time.XXX' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("magnetic_field.miss_distance.${magnetic_field.associations}")
      .set_terse_description("Set a specific miss distance for a given geom/mag. field association")
      .set_traits(datatools::TYPE_REAL)
      .set_mandatory(false)
      .set_explicit_unit(true)
      .set_unit_label("length")
      .set_long_description("Apply a special miss distance for a given geom. volume/mag. field association. \n"
                            "                                                                    \n"
                            "Example::                                                           \n"
                            "                                                                    \n"
                            "  magnetic_field.associations       : string[2] = \"coil\" \"lab\"  \n"
                            "  magnetic_field.miss_distance.coil : real as length = 1 mm         \n"
                            "  magnetic_field.miss_distance.lab  : real as length = 1 cm         \n"
                            "                                                                    \n"
                            "Note: the geometry volume/magnetic field association must exist in  \n"
                            "the dedicated plugin.                                               \n"
                            )
      ;
  }

  {
    // Description of the 'magnetic_field.check_pos_time.XXX' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("magnetic_field.check_pos_time.${magnetic_field.associations}")
      .set_terse_description("Flag to force the checking of position/time while computing mag. field for a given geom/mag. field association")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_long_description("Activate the checking of postion/time while computing the           \n"
                            "magnetic field referenced by a given geom. volume/mag. field association. \n"
                            "                                                                    \n"
                            "Example::                                                           \n"
                            "                                                                    \n"
                            "  magnetic_field.associations        : string[2] = \"coil\" \"lab\" \n"
                            "  magnetic_field.check_pos_time.coil : boolean = 1                  \n"
                            "  magnetic_field.check_pos_time.lab  : boolean = 0                  \n"
                            "                                                                    \n"
                            "Note: the geometry volume/magnetic field association must exist in  \n"
                            "the dedicated plugin.                                               \n"
                            )
      ;
  }

  // Additionnal configuration hints :
  ocd_.set_configuration_hints("Typical configuration from the Gean4 simulation manager is::                 \n"
                               "                                                                             \n"
                               "  [name=\"detector_construction\"]                                           \n"
                               "  #@config Main configuration file for the detector construction user object \n"
                               "  logging.priority : string = \"debug\"                                      \n"
                               "  gdml.tmp_dir : string as path = \"/tmp/${USER}/mctools_g4.d\"              \n"
                               "  gdml.schema_location : string = \"local\"                                  \n"
                               "  gdml.no_validation   : boolean = 0                                         \n"
                               "  materials.plugin_name : string = \"materials_driver\"                      \n"
                               "  sensitive.detectors : string[2] = \"calo_SD\"  \"tracker_SD\"              \n"
                               "  sensitive.calo_SD.hits_buffer_capacity : integer = 50                      \n"
                               "  sensitive.calo_SD.record_track_id : boolean = 1                            \n"
                               "  sensitive.tracker_SD.hits_buffer_capacity : integer = 200                  \n"
                               "  sensitive.tracker_SD.record_track_id : boolean = 1                         \n"
                               "                                                                             \n"
                               "Additional properties for *sensitive detector* are detailed in the           \n"
                               "OCD manual for the ``mctools::g4::sensitive_detector`` class.                \n"
                               );

  ocd_.set_validation_support(true);

  // Lock the description:
  ocd_.lock();

  // ... and we are done.
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END() // Closing macro for implementation

// Registration macro for class 'mctools::g4::detector_construction' :
DOCD_CLASS_SYSTEM_REGISTRATION(mctools::g4::detector_construction,"mctools::g4::detector_construction")

// end of detector_construction.cc
