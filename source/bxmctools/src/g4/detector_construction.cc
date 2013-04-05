// -*- mode: c++ ; -*-
/* detector_construction.cc
 */

#include <boost/algorithm/string/predicate.hpp>

#include <boost/filesystem.hpp>

#include <datatools/utils.h>
#include <datatools/ioutils.h>
#include <datatools/units.h>

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
      if (_g4_manager_ == 0) {
        ostringstream message;
        message << "mctools::g4::detector_construction::get_manager: "
                << "Operation prohibited !";
        throw logic_error(message.str());
      }
      return *_g4_manager_;
    }

    manager &
    detector_construction::grab_manager()
    {
      if (_g4_manager_ == 0) {
        ostringstream message;
        message << "mctools::g4::detector_construction::grab_manager: "
                << "Operation prohibited !";
        throw logic_error(message.str());
      }
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
      _debug_ = false;
      _verbose_ = false;
      _abort_on_error_ = true;

      _g4_manager_ = &g4_mgr_;
      _geom_manager_ = 0;

      _mag_field_manager_  = 0;
      _using_mag_field_ = true;
      _miss_distance_unit_ = CLHEP::mm;
      _general_miss_distance_ = DEFAULT_MISS_DISTANCE;

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
      //_sensitive_detectors_store_g4_volume = false;
      //_sensitive_detectors_drop_zero_energy_deposit_steps = true;

      //      _SHPF_random_seed_ = mygsl::random_utils::SEED_INVALID;
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
    void detector_construction::set_debug(bool d_)
    {
      _debug_  = d_;
    }

    //____________________________________
    bool detector_construction::is_debug() const
    {
      return _debug_;
    }
    //____________________________________
    void detector_construction::set_verbose(bool d_)
    {
      _verbose_  = d_;
    }

    //____________________________________
    bool detector_construction::is_verbose() const
    {
      return _verbose_;
    }

    bool detector_construction::has_geometry_manager() const
    {
      return _geom_manager_ != 0;
    }


    //____________________________________
    void detector_construction::set_geometry_manager(const geomtools::manager & gm_)
    {
      if (is_initialized()) {
        std::ostringstream message;
        message << "mctools::g4::detector_construction::set_geometry_manager: "
                << "Operation prohibited !";
        throw std::logic_error(message.str());
      }
      _geom_manager_ = &gm_;
      return;
    }

    //____________________________________
    void detector_construction::initialize(const datatools::properties & config_)
    {
      if (is_initialized()) {
        std::ostringstream message;
        message << "mctools::g4::detector_construction::initialize: "
                << "Already initialized !";
        throw std::logic_error(message.str());
      }

      /*** parsing properties from 'config_'  ***/

      if (config_.has_flag("debug")) {
        set_debug(true);
      }

      if (config_.has_flag("verbose")) {
        set_verbose(true);
      }

      if (config_.has_key("gdml.tmp_dir")) {
        std::string gdml_tmp_dir = config_.fetch_string("gdml.tmp_dir") ;
        datatools::fetch_path_with_env(gdml_tmp_dir);
        _gdml_file_dir_ = gdml_tmp_dir;
      }

      if (config_.has_flag("gdml.no_validation")) {
        _gdml_validation_ = false;
      }

      if (config_.has_key("gdml.schema_location")) {
        std::string gdml_schema_location = config_.fetch_string("gdml.schema_location") ;
        if (gdml_schema_location == "remote") {
          _gdml_schema_ = geomtools::gdml_writer::DEFAULT_REMOTE_GDML_SCHEMA;
        } else if (gdml_schema_location == "local") {
          _gdml_schema_ = "${GEOMTOOLS_DATA_DIR}/resources/gdml_schema/gdml.xsd";
          datatools::fetch_path_with_env(_gdml_schema_);
        } else {
          std::ostringstream message;
          message << "mctools::g4::detector_construction::initialize: "
                  << "Missing property '" << "gdml.schema_location" << "' !";
          throw std::logic_error(message.str());
        }
      }
 
      if (config_.has_key("materials.plugin_name")) {
        std::string mpn = config_.fetch_string("materials.plugin_name");
        set_materials_geom_plugin_name(mpn);
      }

      if (config_.has_key("hit_processor_factory.config")) {
        _SHPF_config_ = config_.fetch_string("hit_processor_factory.config");
      } else {
        std::ostringstream message;
        message << "mctools::g4::detector_construction::initialize: "
                << "Missing property '" << "hit_processor_factory.config" << "' !";
        throw std::logic_error(message.str());
      }

      config_.export_starting_with(_limits_config_, "limits.");

      // end of fetching.

      if (_geom_manager_ == 0) {
        std::ostringstream message;
        message << "mctools::g4::detector_construction::initialize: "
                << "Missing geometry manager !";
        throw std::logic_error(message.str());
      }

      /*************************
       *                       *
       *  Sensitive detectors  *
       *                       *
       *************************/

      if (config_.has_key("sensitive.detectors")) {
        std::vector<std::string> list_of_sensitive_detectors;
        config_.fetch("sensitive.detectors", list_of_sensitive_detectors);

        // Loop on the list of sensitive categories/detectors :
        for (unsigned int i = 0; i < list_of_sensitive_detectors.size(); ++i) {
          const std::string & SD_category = list_of_sensitive_detectors[i];
          if (! _SD_params_.has_key(SD_category)) {
            _SD_params_.add(SD_category, "SD");
          }

          // Extract properties starting with
          // sensitive.'SD_category' and store them with a new
          // prefix starting only with sensitive.
          const std::string key_oss     = "sensitive." + SD_category;
          const std::string new_key_oss = "sensitive";

          config_.export_and_rename_starting_with(_SD_params_.get(SD_category).get_properties(),
                                                  key_oss,
                                                  new_key_oss);
        }

        std::clog << datatools::io::notice
                  << "mctools::g4::detector_construction::initialize: "
                  << "Info for sensitive detectors : " << std::endl;
        for (std::vector<std::string>::const_iterator i = list_of_sensitive_detectors.begin();
             i != list_of_sensitive_detectors.end();
             ++i) {
          std::vector<std::string>::const_iterator j = i;
          if (++j == list_of_sensitive_detectors.end()) {
            std::clog << "`-- ";
          }
          else {
            std::clog << "|-- ";
          }
          std::clog << "Sensitive detectors '" << *i << "'" << std::endl;

          if (is_debug()) {
            std::ostringstream title;
            title << datatools::io::debug
                  << "mctools::g4::detector_construction::initialize: "
                  << "sensitive parameters for '" << *i << "'";
            _SD_params_.get(*i).get_properties().tree_dump(std::clog, title.str(), "DEBUG: ");
          }
        }
      }


      /*************************
       *                       *
       *        Regions        *
       *                       *
       *************************/

      if (config_.has_key("regions")) {
        std::vector<std::string> regions;
        config_.fetch("regions", regions);
        for (unsigned int i = 0; i < regions.size(); ++i) {
          const std::string & the_region_label = regions[i];
          std::ostringstream key;
          key << "regions." << the_region_label;
          if (! config_.has_key(key.str())) {
            std::ostringstream message;
            message << "mctools::g4::detector_construction::initialize: "
                    << "Missing informations for region '"
                    << key.str() << "' !";
            throw std::logic_error(message.str());
          }
          std::vector<std::string> the_region_infos;
          config_.fetch(key.str(), the_region_infos);
          _region_infos_[the_region_label] = the_region_infos;
        }

        std::clog << "NOTICE: mctools::g4::detector_construction::initialize: "
                  << "Info for regions : " << std::endl;
        for (region_infos_dict_type::const_iterator i = _region_infos_.begin();
             i != _region_infos_.end();
             ++i) {
          region_infos_dict_type::const_iterator j = i;
          if (++j == _region_infos_.end()) {
            std::clog << "`-- ";
          } else {
            std::clog << "|-- ";
          }
          std::clog << "Region '" << i->first << "'  :  Volumes =(";
          for (unsigned int k = 0; k < i->second.size(); ++k) {
            std::clog << "'" << i->second[k] << "' ";
          }
          std::clog << ')' << std::endl;
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
            std::clog << "NOTICE: " << "Found EM field geometry plugin named '" 
                      << emfield_geom_plugin_name 
                      << "'" << std::endl;
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
      if (is_debug()) {
        std::clog << datatools::io::debug
                  << "mctools::g4::detector_construction::Construct: "
                  << "Entering..." << std::endl;
      }

      if (! is_initialized()) {
        throw logic_error("mctools::g4::detector_construction::Construct: Not initialized !");
      }

      G4VPhysicalVolume * world_physical_volume = 0;

      world_physical_volume = _g4_construct();

      if (world_physical_volume == 0) {
        G4Exception("mctools::g4::detector_construction::Construct: Missing world physical volume !");
      }

      if (is_debug()) {
        std::clog << datatools::io::debug
                  << "mctools::g4::detector_construction::Construct: "
                  << "Exiting." << std::endl;
      }
      return world_physical_volume;
    }

    //____________________________________
    G4VPhysicalVolume * detector_construction::_g4_construct()
    {
      if (is_debug()) {
        std::clog << datatools::io::debug
                  << "mctools::g4::detector_construction::_g4_construct: "
                  << "Entering..." << std::endl;
      }

      // Automaticaly construct the geometry tree:
      G4VPhysicalVolume * world_physical_volume = 0;
      G4GDMLParser parser;
      if (_generate_gdml_file_) {
        write_tmp_gdml_file();
        parser.Read(_gdml_filename_, _gdml_validation_);
      }
      else parser.Read("test_manager.gdml");
      world_physical_volume = parser.GetWorldVolume();

      if (is_debug()) {
        std::clog << datatools::io::debug << "---------------------------------------------" << std::endl;
        std::clog << datatools::io::debug << "G4LogicalVolumeStore" << std::endl;
        std::clog << datatools::io::debug << "---------------------------------------------" << std::endl;
        G4LogicalVolumeStore * g4_LV_store = G4LogicalVolumeStore::GetInstance();
        for (G4LogicalVolumeStore::const_iterator i = g4_LV_store->begin();
             i != g4_LV_store->end();
             ++i) {
          G4LogicalVolume * g4_log = *i;
          std::clog << datatools::io::debug << "Log: '" << g4_log->GetName() << "'";
          std::clog << std::endl;
        }
        std::clog << datatools::io::debug << "---------------------------------------------" << std::endl;
      }

      // Automaticaly construct regions:
      if (_using_regions_) {
        if (is_debug()) {
          std::clog << datatools::io::debug
                    << "mctools::g4::detector_construction::_g4_construct: "
                    << "Construct regions." << std::endl;
        }
        _construct_regions();
      }

      // Automaticaly construct sensitive detectors:
      if (_using_sensitive_detectors_) {
        if (is_debug()) {
          std::clog << datatools::io::debug
                    << "mctools::g4::detector_construction::_g4_construct: "
                    << "Construct sensitive detectors." << std::endl;
        }
        _construct_sensitive_detectors();
      }

      // Automaticaly construct the magnetic field :
      if (_using_mag_field_) {
        if (is_debug()) {
          std::clog << datatools::io::debug
                    << "mctools::g4::detector_construction::_g4_construct: "
                    << "Construct the magnetic field." << std::endl;
        }
        _construct_magnetic_field();
      }
      
      // Automaticaly set the visibility attributes:
      if (_using_vis_attributes_) {
        if (is_debug()) {
          std::clog << datatools::io::debug
                    << "mctools::g4::detector_construction::_g4_construct: "
                    << "Set the visualization attributes." << std::endl;
        }
        _set_visualization_attributes();
      }

      // Automaticaly set the user limits:
      if (_using_user_limits_) {
        if (is_debug()) {
          std::clog << datatools::io::debug
                    << "mctools::g4::detector_construction::_g4_construct: "
                    << "Using the user limits(max step)." << std::endl;
        }
        _set_user_limits();
      }

      if (is_debug()) {
        std::clog << datatools::io::debug
                  << "mctools::g4::detector_construction::_g4_construct: "
                  << "Exiting." << std::endl;
      }
      return world_physical_volume;
    }


    bool detector_construction::has_mag_field_manager() const
    {
      return _mag_field_manager_ != 0;
    }

   
    void detector_construction::set_mag_field_manager(const emfield::electromagnetic_field_manager &emf_mgr_) 
    {
      if (_initialized_) {
        std::ostringstream message;
        message << "mctools::g4::detector_construction::set_mag_field_manager: "
                << "Already initialized !";
        throw std::logic_error(message.str());
      }
      _mag_field_manager_ = &emf_mgr_;
      return;
    }

    const emfield::electromagnetic_field_manager & 
    detector_construction::get_mag_field_manager() const
    {
      if (_mag_field_manager_ == 0) {
        std::ostringstream message;
        message << "mctools::g4::detector_construction::get_mag_field_manager: "
                << "No magentic field manager is defined !";
        throw std::logic_error(message.str());
      }
      return *_mag_field_manager_;
    }
 
    void detector_construction::_construct_magnetic_field()
    {
      if (is_debug()) {
        std::clog << datatools::io::debug
                  << "mctools::g4::detector_construction::_construct_magnetic_field: "
                  << "Entering..." << std::endl;
      }
      
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
        G4LogicalVolumeStore * g4_LV_store = G4LogicalVolumeStore::GetInstance();
        const emfield::geom_map & geomap = _mag_field_manager_->get_geom_map();

        typedef emfield::geom_map::association_dict_type gma_type;
        typedef emfield::geom_map::association_entry gma_entry_type;
        const gma_type & mfamap = geomap.get_associations();
        for (gma_type::const_iterator i = mfamap.begin();
             i !=  mfamap.end();
             i++) {
          const gma_entry_type & gefa = i->second;
          if (! gefa.has_logvol()) {
            std::ostringstream message;
            message << "mctools::g4::detector_construction::_construct_magnetic_field: "
                    << "Missing G4 logical volume for geom/field association '" << i->first << "'(that looks a bug) !";
            if (_abort_on_error_) {
              throw std::logic_error(message.str());
            }
            continue;
          }
          G4String g4_log_name =  gefa.logvol->get_name().c_str();
          G4LogicalVolume * g4_module_log = g4_LV_store->GetVolume(g4_log_name, false); 
          if (g4_module_log == 0) {
            std::ostringstream message;
            message << "mctools::g4::detector_construction::_construct_magnetic_field: "
                    << "Missing G4 logical volume with name '" << g4_log_name << "' !";
            if (_abort_on_error_) {
              throw std::logic_error(message.str());
            }
            std::clog << datatools::io::warning
                      << message.str() << std::endl;
            continue;
          }
          magnetic_field * mag_field = new magnetic_field;

          if (gefa.field == 0) {
            std::ostringstream message;
            message << "mctools::g4::detector_construction::_construct_magnetic_field: "
                    << "Missing field for geometry/association '" << i->first << "' !";
            throw std::logic_error(message.str());
          }                 
          if (! gefa.field->is_magnetic_field()) {
            std::ostringstream message;
            message << "mctools::g4::detector_construction::_construct_magnetic_field: "
                    << "Field '" << i->first << "' is not a magnetic field !";
            throw std::logic_error(message.str());
          }
 
          // Check position/time :
          bool cpt = false;
          std::string cpt_key = "check_pos_time." + i->first; 
          if (_mag_field_aux_.has_key(cpt_key)) {
            cpt = _mag_field_aux_.fetch_boolean(cpt_key);
          }
          mag_field->set_name(i->first);
          mag_field->set_mag_field(*gefa.field);
          mag_field->set_mag_field_check_pos_time(cpt);
          mag_field->initialize();

          // Use the default miss distance :
          double miss_distance = _general_miss_distance_;
          std::string md_log_key = "miss_distance." + i->first;
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
          std::ostringstream message;
          message << "mctools::g4::detector_construction::_construct_magnetic_field: "
                  << "G4 logical volume '" << g4_log_name << "' has a magnetic field named '" << i->first  << "' !";
          std::clog << datatools::io::notice
                    << message.str() << std::endl;
  

        }
      }

      /*
        const geomtools::mapping & the_mapping = _geom_manager_->get_mapping();
        for (magnetic_field::field_map_type::const_iterator i
        = _mag_field_->get_fields().begin();
        i != _mag_field_->get_fields().end();
        ++i)
        {
        uint32_t module_num = i->first;
        geomtools::geom_id module_id;
        module_id.set_type(_mag_field_->get_module_type());
        module_id.set_address(module_num);
        if (! the_mapping.has_geom_info(module_id))
        {
        ostringstream message;
        message << "mctools::g4::detector_construction::_construct_magnetic_field: "
        << "Mapping has no ID " << module_id;
        std::clog << datatools::io::warning
        << message.str() << std::endl;
        continue;
        }
        const geomtools::geom_info & ginfo = the_mapping.get_geom_info(module_id);
        if (! ginfo.has_logical())
        {
        ostringstream message;
        message << "mctools::g4::detector_construction::_construct_magnetic_field: "
        << "Missing logical volume at ID " << module_id;
        std::clog << datatools::io::warning
        << message.str() << std::endl;
        continue;
        //throw runtime_error(message.str());
        }
        const geomtools::logical_volume & log = ginfo.get_logical();
        const string & log_name = log.get_name();
        G4String g4_log_name = log_name.c_str();
        G4LogicalVolume * g4_module_log = g4_LV_store->GetVolume(g4_log_name, false);
        if (g4_module_log == 0)
        {
        ostringstream message;
        message << "mctools::g4::detector_construction::_construct_magnetic_field: "
        << "Missing G4 logical volume with name '" << g4_log_name << "' !";
        std::clog << datatools::io::warning
        << message.str() << std::endl;
        continue;
        }

        G4FieldManager * detector_field_mgr = new G4FieldManager(_mag_field_);
        detector_field_mgr->CreateChordFinder(_mag_field_);
        detector_field_mgr->GetChordFinder()->SetDeltaChord(_miss_distance_);
        g4_module_log->SetFieldManager(detector_field_mgr, true);
        ostringstream message;
        message << "mctools::g4::detector_construction::_construct_magnetic_field: "
        << "G4 logical volume '" << g4_log_name << "' has a magnetic field !";
        std::clog << datatools::io::notice
        << message.str() << std::endl;
        }
      */

      if (is_debug())
        {
          std::clog << datatools::io::debug
                    << "mctools::g4::detector_construction::_construct_magnetic_field: "
                    << "Exiting." << std::endl;
        }
      return;
    }

    //____________________________________
    void detector_construction::_construct_sensitive_detectors()
    {
      bool debug = is_debug();

      //debug = true;

      if (debug)
        {
          std::clog << datatools::io::debug
                    << "mctools::g4::detector_construction::_construct_sensitive_detectors: "
                    << "Entering..." << std::endl;
        }
      G4SDManager * SDman = G4SDManager::GetSDMpointer();
      G4LogicalVolumeStore * g4_LV_store = G4LogicalVolumeStore::GetInstance();

      /**************************************************************************
       * Setup the "offical" sensitive categories from the model factory itself *
       **************************************************************************/

      for (geomtools::models_col_type::const_iterator imodel
             = _geom_manager_->get_factory().get_models().begin();
           imodel != _geom_manager_->get_factory().get_models().end();
           ++imodel)
        {
          // Get a reference to the geometry model :
          const geomtools::i_model & model = *(imodel->second);

          // Get a reference to the associated logical volume :
          const geomtools::logical_volume & log = model.get_logical();

          // Check if it is tagged as a 'sensitive' detector :
          if (! geomtools::sensitive::is_sensitive(log.parameters()))
            {
              if (debug)
                {
                  ostringstream message;
                  message << "mctools::g4::detector_construction::_construct_sensitive_detectors: "
                          << "Logical volume '" << log.get_name() << "' is not sensitive !";
                  std::clog << datatools::io::debug << message.str() << std::endl;
                }
              continue;
            }

          // Get the 'sensitive category' for this detector :
          string sensitive_category
            = geomtools::sensitive::get_sensitive_category(log.parameters());
          if (sensitive_category.empty())
            {
              ostringstream message;
              message << "mctools::g4::detector_construction::_construct_sensitive_detectors: "
                      << "Empty sensitive category name ! Ignore !";
              std::clog << datatools::io::warning << message.str() << std::endl;
              continue;
            }
          if (is_verbose())
            {
              ostringstream message;
              message << "mctools::g4::detector_construction::_construct_sensitive_detectors: "
                      << "Logical volume '" << log.get_name() << "' is sensitive with category '"
                      << sensitive_category << "' !";
              std::clog << datatools::io::notice << message.str() << std::endl;
            }

          // Pickup the corresponding G4 logical volume :
          G4LogicalVolume * g4_log = 0;
          G4String log_name = log.get_name().c_str();
          g4_log = g4_LV_store->GetVolume(log_name, false);
          if (g4_log == 0)
            {
              ostringstream message;
              message << "mctools::g4::detector_construction::_construct_sensitive_detectors: "
                      << "Logical volume '" << log.get_name()
                      << "' is not used in G4LogicalVolumeStore !";
              std::clog << datatools::io::warning << message.str() << std::endl;
              continue;
            }
          sensitive_detector * SD = 0;
          // Search for the sensitive detector that uses this category:
          sensitive_detector_dict_type::iterator found = _sensitive_detectors_.find(sensitive_category);
          bool already_present = false;
          if (found == _sensitive_detectors_.end())
            {
              ostringstream message;
              message << "mctools::g4::detector_construction::_construct_sensitive_detectors: "
                      << "Create a new sensitive detector with category '"
                      << sensitive_category << "'";
              std::clog << datatools::io::notice << message.str() << std::endl;
              SD = new sensitive_detector(sensitive_category);
              SD->set_manager(*_g4_manager_);
              //SD->set_store_g4_volume_properties    (_sensitive_detectors_store_g4_volume);
              //SD->set_drop_zero_energy_deposit_steps(_sensitive_detectors_drop_zero_energy_deposit_steps);
              SDman->AddNewDetector(SD);
              _sensitive_detectors_[sensitive_category] = SD;
            }
          else
            {
              already_present = true;
              ostringstream message;
              message << "mctools::g4::detector_construction::_construct_sensitive_detectors: "
                      << "Use the sensitive detector with category '"
                      << sensitive_category << "'";
              std::clog << datatools::io::notice << message.str() << std::endl;
              SD = found->second;
            }

          {
            ostringstream message;
            message << "mctools::g4::detector_construction::_construct_sensitive_detectors: "
                    << "Make the G4 logical volume '" << log.get_name()
                    << "' a sensitive detector with category '"
                    << sensitive_category << "'";
            std::clog << datatools::io::notice << message.str() << std::endl;
          }

          // update the list of logical volumes(identified by their names) attached
          // to this sensitive detector:
          _sensitive_detectors_[sensitive_category]->attach_logical_volume(log.get_name());

          if (! already_present)
            {
              // setup special behaviour of the new sensitive detector:
              if (_SD_params_.has_key(sensitive_category))
                {
                  const datatools::properties & this_SD_params
                    = _SD_params_.get(sensitive_category).get_properties();
                  SD->configure(this_SD_params);
                }
            }

          // associate this sensitive detector to the G4 logical volume:
          g4_log->SetSensitiveDetector(_sensitive_detectors_[sensitive_category]);

        } // for (models)


      /****************************************
       * Setup the step hit processor factory *
       ****************************************/

      bool SHPF_debug = false;
      _SHPF_.set_debug(SHPF_debug);
      // Pass the address of the geom manager to step hit processors :
      std::clog << datatools::io::notice
                << "mctools::g4::detector_construction::_construct_sensitive_detectors: "
                << "Preparing the SHPF factory..." << std::endl;
      if (this->has_geometry_manager())
        {
          _SHPF_.set_geometry_manager(this->get_geometry_manager());
        }
      if (_SHPF_config_.empty())
        {
          ostringstream message;
          message << "mctools::g4::detector_construction::_construct_sensitive_detectors: "
                  << "Missing configuration file for the step hit processor factory !";
          throw std::logic_error(message.str());
        }
      datatools::fetch_path_with_env(_SHPF_config_);
      datatools::multi_properties mconfig("name", "type");
      std::clog << datatools::io::notice
                << "mctools::g4::detector_construction::_construct_sensitive_detectors: "
                << "Parsing the SHPF configuration file..."
                << std::endl;
      mconfig.read(_SHPF_config_);
      if (debug)
        {
          std::clog << datatools::io::debug
                    << "mctools::g4::detector_construction::_construct_sensitive_detectors: "
                    << "Done." << std::endl;
        }
      _SHPF_.load(mconfig);

      /*****************************************************************
       * Install some step hit processors into the sensitive detectors *
       *****************************************************************/

      /* Look for "un-offical" sensitive categories from the SHPF factory itself.
       * Possibly extends the list of sensitive detectors
       * from the list of step hit processors found in the SHPF factory
       * and creates the proper association to some logical volumes which must
       * be specified using their respective model names.
       */
      for (SHPF_type::processor_dict_type::const_iterator iSHP
             = _SHPF_.get_processors().begin();
           iSHP != _SHPF_.get_processors().end();
           ++iSHP)
        {
          const string & hit_category = iSHP->second->get_hit_category();
          if (_g4_manager_->forbids_private_hits())
            {
              if (boost::starts_with(hit_category, "_"))
                {
                  ostringstream message;
                  message << "mctools::g4::detector_construction::_construct_sensitive_detectors: "
                          << "Sensitive detector '" << iSHP->second->get_name()
                          << "' initialized from SHPF with private hit category '"
                          << hit_category << "' is not used !";
                  std::clog << datatools::io::warning << message.str() << std::endl;
                  continue;
                }
            }

          const string & from_processor_sensitive_category = iSHP->second->get_sensitive_category();

          // If the sensitive_category already exists(from the geometry model setup):
          if (_sensitive_detectors_.find(from_processor_sensitive_category)
              != _sensitive_detectors_.end())
            {
              ostringstream message;
              message << "mctools::g4::detector_construction::_construct_sensitive_detectors: "
                      << "Sensitive detector '" << iSHP->second->get_name()
                      << "' initialized from SHPF with category '"
                      << from_processor_sensitive_category
                      << "' already exists ! Ignore this rule !";
              std::clog << datatools::io::warning << message.str() << std::endl;
              continue;
            }
          else
            {
              // // XXX YYY ZZZ
              // // Check if a list of models is provided : by name or by materials:
              // if (! iSHP->second->get_aux().has_key("geometry.models") &&
              //     ! iSHP->second->get_aux().has_key("geometry.models.with_materials"))
              //   {
              //     ostringstream message;
              //     message << "mctools::g4::detector_construction::_construct_sensitive_detectors: "
              //             << "New sensitive category '" << from_processor_sensitive_category
              //             << "' has no geometry models associated to it ! Ignore this rule !";
              //     std::clog << datatools::io::warning << message.str() << std::endl;
              //     continue;
              //   }

              // At this point, we know that some geometry model(s)
              // are attached to the newly created sensitive detector:
              ostringstream message;
              message << "mctools::g4::detector_construction::_construct_sensitive_detectors: "
                      << "Create a new sensitive detector from SHPF with category '"
                      << from_processor_sensitive_category << "'";
              std::clog << datatools::io::notice << message.str() << std::endl;

              sensitive_detector * SD = new sensitive_detector(from_processor_sensitive_category);
              SD->set_manager(*_g4_manager_);

              // Setup special behaviour of the sensitive detector:
              SD->configure(iSHP->second->get_auxiliaries());

              SDman->AddNewDetector(SD);
              _sensitive_detectors_[from_processor_sensitive_category] = SD;

              // Extract the models this sensitive detector is attached to:
              std::vector<std::string> models;

              // First fetch the list of geometry models associated to this 'sensitive category' :
              if (iSHP->second->get_auxiliaries().has_key("geometry.models"))
                {
                  // Models are given by name :
                  iSHP->second->get_auxiliaries().fetch("geometry.models", models);
                }

              // Also fetch the list of geometry models associated to this 'sensitive category' 
              // and *with* a given list of materials :
              if (iSHP->second->get_auxiliaries().has_key("geometry.models.with_materials"))
                {
                  // Models are given by material(s) :
                  std::vector<std::string> materials;
                  iSHP->second->get_auxiliaries().fetch("geometry.models.with_materials", materials);

                  ostringstream message;
                  message << "mctools::g4::detector_construction::_construct_sensitive_detectors: "
                          << "Searching for geometry models with material in(";
                  for (int j = 0; j < materials.size(); j++)
                    {
                      if (j != 0) message << ',';
                      message << ' ' << '"' << materials[j] << '"';
                    }
                  message << ") to be associated to the new sensitive detector from SHPF with category '"
                          << from_processor_sensitive_category << "'";
                  std::clog << datatools::io::notice << message.str() << std::endl;
                  
                  // Traverse all geometry models :
                  for (geomtools::models_col_type::const_iterator imodel 
                         = _geom_manager_->get_factory().get_models().begin();
                       imodel != _geom_manager_->get_factory().get_models().end();
                       ++imodel)
                    {
                      const string & model_name = imodel->first;
                      const geomtools::i_model & model = *(imodel->second);
                      const geomtools::logical_volume & log = model.get_logical();
                      bool checked_material = false;
                      string mat_name;
                      for (int imat = 0; imat < materials.size(); imat++)
                        {
                          mat_name = materials[imat];
                          if (log.get_material_ref() == mat_name)
                            {
                              checked_material = true;                          
                            }
                        }
                      if (checked_material)
                        {
                          if (std::find(models.begin(), models.end(), model_name) == models.end())
                            {
                              ostringstream message;
                              message << "mctools::g4::detector_construction::_construct_sensitive_detectors: "
                                      << "Associate model '" << model_name << "' with material '"
                                      << mat_name << "' to the new sensitive detector from SHPF with category '"
                                      << from_processor_sensitive_category << "'";
                              std::clog << datatools::io::notice << message.str() << std::endl;
                              models.push_back(model_name);
                            }
                        }
                    }
                }
 
              // Skip the geometry models that should be excluded :
              if (iSHP->second->get_auxiliaries().has_key("geometry.models.excluded"))
                {
                  vector<string> excluded_models;
                  iSHP->second->get_auxiliaries().fetch("geometry.models.excluded", excluded_models);
                  ostringstream message;
                  message << "mctools::g4::detector_construction::_construct_sensitive_detectors: "
                          << "Excluding geometry models in(";
                  for (int j = 0; j < excluded_models.size(); j++)
                    {
                      if (j != 0) message << ',';
                      message << ' ' << '"' << excluded_models[j] << '"';
                    }
                  message << ") from the list of models to be associated to the new sensitive detector from SHPF with category '"
                          << from_processor_sensitive_category << "'";
                  std::clog << datatools::io::notice << message.str() << std::endl;
                  
                  for (int ixm = 0; ixm < excluded_models.size(); ixm++)
                    {
                      models.erase(std::remove(models.begin(), models.end(), excluded_models[ixm]), models.end());
                    }
                }
                  
              if (models.size() == 0)
                {
                  ostringstream message;
                  message << "mctools::g4::detector_construction::_construct_sensitive_detectors: "
                          << "New sensitive category '" << from_processor_sensitive_category
                          << "' has no geometry models associated to it !";
                  std::clog << datatools::io::warning << message.str() << std::endl;
                  continue;
                }

              // Loop on the specified geometry models and make them sensitive
              // with the new sensitive detector.
              // Detect conflict(more than one sensitive detector per model).
              for (vector<string>::const_iterator imodel = models.begin();
                   imodel != models.end();
                   ++imodel)
                {
                  const string & model_name = *imodel;
                  geomtools::models_col_type::const_iterator found
                    = _geom_manager_->get_factory().get_models().find(model_name);
                  if (found == _geom_manager_->get_factory().get_models().end())
                    {
                      std::clog << datatools::io::warning
                                << "mctools::g4::detector_construction::_construct_sensitive_detectors: "
                                <<  "No geometry model with name '"
                                << model_name  << "' exists in the geometry manager ! Ignore !"
                                << std::endl;
                      continue;
                    }
                  const geomtools::i_model & model = *(found->second);
                  const geomtools::logical_volume & log = model.get_logical();

                  // check sensitivity of the model's log(ical volume):
                  if (geomtools::sensitive::is_sensitive(log.parameters()))
                    {
                      ostringstream message;
                      message << "mctools::g4::detector_construction::_construct_sensitive_detectors: "
                              << "Geometry model '"
                              << model_name  << "' is already associated to sensitive category '"
                              << geomtools::sensitive::get_sensitive_category(log.parameters())
                              << "' !";
                      std::clog << datatools::io::warning << message.str() << std::endl;
                      continue;
                    }

                  // pickup the corresponding G4 logical volume:
                  G4LogicalVolume * g4_log = 0;
                  G4String log_name = log.get_name().c_str();
                  g4_log = g4_LV_store->GetVolume(log_name, false);
                  if (g4_log == 0)
                    {
                      ostringstream message;
                      message << "mctools::g4::detector_construction::_construct_sensitive_detectors: "
                              << "No logical volume has been found '" << log.get_name()
                              << "' in the G4LogicalVolumeStore !";
                      std::clog << datatools::io::warning << message.str() << std::endl;
                      continue;
                    }

                  // Local scope:
                  {
                    // Makes the logical sensitive(using a trick because of the const-ness
                    // of the log(ical) after building the model factory in our geometry model:
                    geomtools::logical_volume * mod_log
                      = const_cast<geomtools::logical_volume *>(&log);
                    geomtools::sensitive::set_sensitive_category(mod_log->parameters(),
                                                                 from_processor_sensitive_category);
                  }

                  ostringstream message;
                  message << "mctools::g4::detector_construction::_construct_sensitive_detectors: "
                          << "Make the G4 logical volume '" << log.get_name()
                          << "' a sensitive detector with category '"
                          << from_processor_sensitive_category << "'";
                  std::clog << datatools::io::notice << message.str() << std::endl;
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
           ++iSHP)
        {
          const string & processor_name = iSHP->first;
          const string & hit_category = iSHP->second->get_hit_category();
          if (_g4_manager_->forbids_private_hits())
            {
              if (boost::starts_with(hit_category, "_"))
                {
                  std::ostringstream message;
                  message << "mctools::g4::detector_construction::_construct_sensitive_detectors: "
                          << "Sensitive detector '" << iSHP->second->get_name()
                          << "' initialized from SHPF with private hit category '"
                          << hit_category << "' is not used !";
                  std::clog << datatools::io::warning << message.str() << std::endl;
                  continue;
                }
            }
          const string & from_processor_sensitive_category = iSHP->second->get_sensitive_category();
          sensitive_detector_dict_type::iterator iSD
            = _sensitive_detectors_.find(from_processor_sensitive_category);
          if (iSD == _sensitive_detectors_.end())
            {
              continue;
            }
          sensitive_detector * SD = iSD->second;
          SD->add_hit_processor(processor_name, *iSHP->second);
        }

      // Dump:
      if (is_verbose())
        {
          for (detector_construction::sensitive_detector_dict_type::iterator iSD
                 = _sensitive_detectors_.begin();
               iSD != _sensitive_detectors_.end();
               ++iSD)
            {
              const string & sensitive_category = iSD->first;
              sensitive_detector * SD = iSD->second;
              std::clog << datatools::io::notice
                        << "mctools::g4::detector_construction::_construct_sensitive_detectors: "
                        << "Sensitive detector '" << sensitive_category << "' " << std::endl;
              SD->tree_dump(clog, "", "NOTICE: ");
            }
        }

      if (debug)
        {
          std::clog << datatools::io::debug
                    << "mctools::g4::detector_construction::_construct_sensitive_detectors: "
                    << "Exiting." << std::endl;
        }
      return;
    }

    //____________________________________
    void detector_construction::set_materials_geom_plugin_name(const std::string & mpn_)
    {
      if (is_initialized()) {
        std::ostringstream message;
        message << "mctools::g4::detector_construction::set_materials_geom_plugin_name: "
                << "Operation prohibited !";
        throw std::logic_error(message.str());
      }
      _materials_geom_plugin_name_ = mpn_;
      return;
    }

    //____________________________________
    void detector_construction::set_emfield_geom_plugin_name(const std::string & fpn_)
    {
      if (is_initialized()) {
        std::ostringstream message;
        message << "mctools::g4::detector_construction::set_emfield_geom_plugin_name: "
                << "Operation prohibited !";
        throw std::logic_error(message.str());
      }
      _emfield_geom_plugin_name_ = fpn_;
      return;
    }

    //____________________________________
    void detector_construction::write_tmp_gdml_file()
    {

      if (! _geom_manager_->is_initialized())
        {
          ostringstream message;
          message << "mctools::g4::detector_construction::_g4_construct: "
                  << "Geometry manager is not initialized !";
          throw logic_error(message.str());
        }

      ostringstream tmp_file_template_oss;
      if (! _gdml_file_dir_.empty()) {
        if (! boost::filesystem::is_directory(_gdml_file_dir_)) {
          bool do_mkdir = true;
          if (do_mkdir) {
            std::clog << "WARNING: "
                      << "mctools::g4::detector_construction::_g4_construct: "
                      << "Attempt to create the temporary directory '"
                      << _gdml_file_dir_ << "'..." << std::endl;
            ostringstream syscmd;
            syscmd << "mkdir -p " << _gdml_file_dir_;
            int syserr = system(syscmd.str().c_str());
            if (syserr == -1) {
              ostringstream message;
              message << "mctools::g4::detector_construction::_g4_construct: "
                      << "Cannot create temporary directory '"
                      << _gdml_file_dir_ << "' !";
              throw runtime_error(message.str());
            }
          }
          else {
            ostringstream message;
            message << "mctools::g4::detector_construction::_g4_construct: Temporary directory '" << _gdml_file_dir_ << "' does not exist !";
            throw logic_error(message.str());
          }
        }
        tmp_file_template_oss << _gdml_file_dir_ << '/';
      }
      tmp_file_template_oss << "mctools_g4_geometry.gdml." << "XXXXXX" << ends;
      string tmp_file_template = tmp_file_template_oss.str();
      char tmp_gdml_filename[1024];
      copy(tmp_file_template.begin(), tmp_file_template.end(), tmp_gdml_filename);

      int err = mkstemp(tmp_gdml_filename);
      if (err == -1) {
        ostringstream message;
        message << "mctools::g4::detector_construction::_g4_construct: Cannot create GDML temporary file !";
        throw runtime_error(message.str());
      }
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
        std::clog << "NOTICE: " << "Found materials plugin named '" << materials_plugin_name 
                  << "'" << std::endl;
        const geomtools::materials_plugin & mgp 
          = geo_mgr.get_plugin<geomtools::materials_plugin>(materials_plugin_name);
        const materials::manager & mat_mgr = mgp.get_manager();
        mat_mgr_ref = &mat_mgr;
      }
      if (mat_mgr_ref == 0) {         
        std::ostringstream message;
        message << "mctools::g4::detector_construction::_g4_construct: "
                << "Cannot find a material driver plugin in the geometry manager !";
        throw std::logic_error(message.str());
      }
      std::clog << "NOTICE: " 
                << "Export GDML materials from the materials driver plugin: "<< std::endl;
      geomtools::export_gdml(*mat_mgr_ref, materials_writer);
      GDML.attach_external_materials(materials_writer.get_stream(geomtools::gdml_writer::MATERIALS_SECTION));

      // XXX
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
      if (is_debug()) {
        std::clog << datatools::io::debug
                  << "mctools::g4::detector_construction::_set_user_limits: "
                  << "Entering..." << std::endl;
      }
      G4LogicalVolumeStore * g4_LV_store = G4LogicalVolumeStore::GetInstance();

      vector<string> models;
      if (_limits_config_.has_key("limits.list_of_models")) {
        _limits_config_.fetch("limits.list_of_models", models);
      }
      if (models.size() == 0) {
        return;
      }
      const geomtools::models_col_type & models_dict
        = _geom_manager_->get_factory().get_models();
      for (unsigned int i = 0; i < models.size(); ++i) {
        const string & model_name = models[i];
        geomtools::models_col_type::const_iterator found = models_dict.find(model_name);
        if (found == models_dict.end()) {
          ostringstream message;
          message << "mctools::g4::detector_construction::_set_user_limits: "
                  << "Cannot find model '" << model_name
                  << "' !";
          std::clog << datatools::io::warning << message.str() << std::endl;
          continue;
        }

        string log_name = geomtools::i_model::make_logical_volume_name(model_name);
        G4String g4_log_name = log_name.c_str();
        G4LogicalVolume * g4_module_log = g4_LV_store->GetVolume(g4_log_name, false);
        if (g4_module_log == 0) {
          ostringstream message;
          message << "mctools::g4::detector_construction::_set_user_limits: "
                  << "Missing G4 logical volume with name '" << g4_log_name << "' !";
          std::clog << datatools::io::warning
                    << message.str() << std::endl;
          continue;
        }

        // search for maximum step length:
        double max_step = DBL_MAX;
        ostringstream prop_name;
        prop_name << "limits.max_step." << model_name;
        if (_limits_config_.has_key(prop_name.str())) {
          max_step = _limits_config_.fetch_real(prop_name.str());
          if (! _limits_config_.has_explicit_unit(prop_name.str())) {
            max_step *= CLHEP::mm;
          }
          G4UserLimits * g4_user_limits = new G4UserLimits(max_step);
          _user_limits_col_.push_back(g4_user_limits);
          g4_module_log->SetUserLimits(g4_user_limits);
          ostringstream message;
          message << "mctools::g4::detector_construction::_set_user_limits: "
                  << "Install user limits for G4 logical volume with name '" << g4_log_name << "' with max step = " << max_step / CLHEP::mm << " mm";
          std::clog << datatools::io::notice
                    << message.str() << std::endl;
        }

      }

      if (is_debug()) {
        std::clog << datatools::io::debug
                  << "mctools::g4::detector_construction::_set_user_limits: "
                  << "Exiting." << std::endl;
      }
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
          string logical_volume_name = the_region_infos[j] + ".log";
          G4String g4_logical_volume_name = logical_volume_name.c_str();
          a_logical = g4_LV_store->GetVolume(g4_logical_volume_name, false);
          if (a_logical != 0) {
            the_region->AddRootLogicalVolume(a_logical);
          }
          else {
            ostringstream message;
            message << "mctools::g4::detector_construction::_construct_regions: "
                    << "No logical volume named '"
                    << logical_volume_name << "' to be added in the '"
                    << the_region_name << "' region !";
            cerr << "WARNING: " << message.str() << std::endl;
          }
        }
      }
      return;
    }

    //____________________________________
    void detector_construction::_set_visualization_attributes()
    {
      if (is_debug()) {
        std::clog << datatools::io::debug
                  << "mctools::g4::detector_construction::_set_visualization_attributes: "
                  << "Entering..." << std::endl;
      }
      // Look for visibility properties :
      for (geomtools::models_col_type::const_iterator i
             = _geom_manager_->get_factory().get_models().begin();
           i != _geom_manager_->get_factory().get_models().end();
           ++i) {
        const geomtools::i_model & model = *(i->second);
        const geomtools::logical_volume & log = model.get_logical();
        bool   visible = true;
        bool   daughters_visible = true;
        string display_color;
        bool   force_wire_frame = true;
        bool   force_solid = false;
        if (geomtools::visibility::is_hidden(log.parameters())) {
          visible = false;
          daughters_visible = false;
        }
        else {
          if (geomtools::visibility::is_hidden_envelop(log.parameters())) {
            visible = false;
          }
          if (geomtools::visibility::is_daughters_hidden(log.parameters())) {
            daughters_visible = false;
          }
          if (geomtools::visibility::has_color(log.parameters())) {
            display_color = geomtools::visibility::get_color(log.parameters());
          }
          /*
            if (geomtools::visibility::has_key(log.parameters(), "opacity"))
            {
            // Not supported.
            //display_opacity = geomtools::visibility::get_color(log.parameters());
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
          }
          else {
            va->SetVisibility(true);
          }
          if (! daughters_visible) {
            va->SetDaughtersInvisible(true);
          }
          else {
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
        }
        else {
          if (is_debug()) {
            ostringstream message;
            message << "mctools::g4::detector_construction::_set_visualization_attributes: "
                    << "Cannot find logical volume '" << log.get_name() << "' in G4LogicalVolumeStore !";
            std::clog << datatools::io::debug << message.str() << std::endl;
          }
        }

      }
      if (is_debug()) {
        std::clog << datatools::io::debug
                  << "mctools::g4::detector_construction::_set_visualization_attributes: "
                  << "Exiting." << std::endl;
      }
      return;
    }

  } // end of namespace g4

} // end of namespace mctools

// end of detector_construction.cc
