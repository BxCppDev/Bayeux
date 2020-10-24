// electromagnetic_field_manager.cc

// Ourselves:
#include <emfield/electromagnetic_field_manager.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/service_manager.h>
#include <datatools/utils.h>
// - Bayeux/geomtools:
#include <geomtools/utils.h>
#include <geomtools/geometry_service.h>
#include <geomtools/manager.h>

// Ths project:
#include <emfield/geom_map.h>

namespace emfield {

  void electromagnetic_field_manager::set_logging_priority(datatools::logger::priority priority_)
  {
    _logging_priority_ = priority_;
    return;
  }

  datatools::logger::priority electromagnetic_field_manager::get_logging_priority() const
  {
    return _logging_priority_;
  }

  bool electromagnetic_field_manager::is_initialized() const
  {
    return _initialized_;
  }

  void electromagnetic_field_manager::_set_initialized(bool initialized_)
  {
    _initialized_ = initialized_;
    return;
  }

  bool electromagnetic_field_manager::has_geometry_manager() const
  {
    return _geom_manager_ != 0;
  }

  void electromagnetic_field_manager::set_geometry_manager(const geomtools::manager & geomgr_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error,
                "Cannot set the geometry manager ! EM field manager is locked !");
    _geom_manager_ = &geomgr_;
    return;
  }

  const geomtools::manager & electromagnetic_field_manager::get_geometry_manager() const
  {
    DT_THROW_IF(! has_geometry_manager(), std::logic_error, "No geometry manager is available !");
    return *_geom_manager_;
  }

  bool electromagnetic_field_manager::has_geom_map() const
  {
    return _geom_map_.get() != 0; //is_initialized();
  }

  bool electromagnetic_field_manager::has_service_manager() const
  {
    return _service_manager_ != 0;
  }

  void electromagnetic_field_manager::set_service_manager(datatools::service_manager & sm_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error,
                "Cannot change the service manager ! EM field manager is locked !");
    _service_manager_ = &sm_;
    return;
  }

  void electromagnetic_field_manager::reset_service_manager()
  {
    DT_THROW_IF(is_initialized(), std::logic_error,
                "Cannot reset the service manager ! EM field manager is locked !");
    _service_manager_ = 0;
    return;
  }

  void electromagnetic_field_manager::load(const std::string & fields_definitions_filename_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error,
                "Cannot load field definitions file '"
                << fields_definitions_filename_ << "' ! EM field manager is locked !");
    std::string filename = fields_definitions_filename_;
    datatools::fetch_path_with_env(filename);
    datatools::multi_properties::read_config(filename, _field_definitions_);
    return;
  }

  void electromagnetic_field_manager::_construct_geomap_(const datatools::properties & setup_)
  {
    // Extract geomap setup
    _geom_map_.reset(new emfield::geom_map);
    _geom_map_.get()->set_fields_manager(const_cast<electromagnetic_field_manager&>(*this));
    if (has_geometry_manager()) {
      DT_LOG_DEBUG(get_logging_priority(), "Setup the geometry manager in the geometry/field map...");
      _geom_map_.get()->set_geometry_manager(*_geom_manager_);
    }
    DT_LOG_DEBUG(get_logging_priority(), "Geometry map setup:");
    if (get_logging_priority() >= datatools::logger::PRIO_DEBUG) {
      setup_.tree_dump(std::clog, "", "[debug]: ");
    }
    DT_LOG_DEBUG(get_logging_priority(), "Initialize geometry/field map...");
    _geom_map_.get()->initialize(setup_);
    return;
  }

  void electromagnetic_field_manager::_construct_()
  {
    // Create a EM field factory:
    uint32_t factory_flags = 0;
    if (_factory_verbose_) {
      factory_flags |= base_electromagnetic_field::factory_register_type::verbose;
    }
    _factory_register_.reset(new emfield_factory_type("emfield::base_electromagnetic_field/manager",
                                                      factory_flags));
    if (_factory_preload_) {
      DT_LOG_DEBUG(_logging_priority_, "Importing EM field factories from the system register...");
      _factory_register_->import(DATATOOLS_FACTORY_GET_SYSTEM_REGISTER(::emfield::base_electromagnetic_field));
    }

    // Create EM fields from parameters:
    for (datatools::multi_properties::entries_ordered_col_type::const_iterator i
           = _field_definitions_.ordered_entries ().begin ();
         i != _field_definitions_.ordered_entries ().end ();
         i++) {
        const datatools::multi_properties::entry * ptr_entry = *i;
        const datatools::multi_properties::entry & e = *ptr_entry;
        std::string field_name = e.get_key();
        DT_THROW_IF(_fields_.find(field_name) != _fields_.end(), std::logic_error,
                     "A field named '" << field_name << "' already exists !");

        const std::string field_type = e.get_meta();
        DT_THROW_IF(! _factory_register_->has(field_type), std::logic_error,
                     "No registered EM field class with ID '"
                     << field_type << "' for field named '" << field_name << " !");
        const base_electromagnetic_field::factory_register_type::factory_type & the_factory
          = _factory_register_->get(field_type);
        DT_LOG_TRACE(get_logging_priority(),
                      "About to create a new field of type \"" << field_type
                      << "\" with name \"" << field_name << "\"...");

        base_electromagnetic_field * ptr_field = the_factory();
        ptr_field->set_name(field_name);
        if (has_service_manager()) {
          ptr_field->initialize(e.get_properties(), *_service_manager_, _fields_);
        } else {
          ptr_field->initialize_with_dictionary_only(e.get_properties(), _fields_);
        }
        base_electromagnetic_field::handle_type new_handle(ptr_field);
        _fields_[field_name] = new_handle;
      }
    _field_definitions_.reset();
    return;
  }

  void electromagnetic_field_manager::_set_defaults()
  {
    _build_geom_map_ = false;
    _factory_preload_ = true;
    _factory_verbose_ = false;
    _logging_priority_ = datatools::logger::PRIO_FATAL;
    return;
  }

  void electromagnetic_field_manager::set_factory_preload(bool fp_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Manager is locked!");
    _factory_preload_ = fp_;
    return;
  }

  void electromagnetic_field_manager::set_factory_verbose(bool fv_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Manager is locked!");
    _factory_verbose_ = fv_;
    return;
  }

  electromagnetic_field_manager::electromagnetic_field_manager(uint32_t flags_)
  {
    _initialized_ = false;
    _set_defaults();
    _service_manager_ = 0;
    _geom_manager_    = 0;
    if (flags_ & INIT_DEBUG) {
      set_logging_priority(datatools::logger::PRIO_DEBUG);
    }
    if (flags_ & INIT_NO_PRELOAD) {
      set_factory_preload(false);
    }
    _service_manager_ = 0;
    _geom_manager_    = 0;
    return;
  }

  electromagnetic_field_manager::~electromagnetic_field_manager()
  {
    if (is_initialized()) {
      reset();
    }
    return;
  }

  void electromagnetic_field_manager::initialize(const datatools::properties & setup_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error,
                "The EM field manager is already initialized !");

    // Parse configuration parameters :
    // Logging priority:
    datatools::logger::priority lp
      = datatools::logger::extract_logging_configuration(setup_, datatools::logger::PRIO_UNDEFINED, true);
    if (lp != datatools::logger::PRIO_UNDEFINED) {
      set_logging_priority(lp);
    }

    bool needs_service_manager = false;
    if(setup_.has_key("needs_service_manager")) {
      needs_service_manager = setup_.fetch_boolean("needs_service_manager");
    }

    bool needs_geometry_manager = true;
    if (setup_.has_key("needs_geometry_manager")) {
      needs_geometry_manager = setup_.fetch_boolean("needs_geometry_manager");
    }

    DT_LOG_DEBUG(_logging_priority_,
                 "needs_service_manager = " << needs_service_manager);

    DT_LOG_DEBUG(_logging_priority_,
                 "needs_geometry_manager = " << needs_geometry_manager);

    // Checks :
    DT_THROW_IF(needs_service_manager && ! has_service_manager(), std::logic_error,
                "Cannot find mandatory service manager !");

    // Search for a handle to a geometry manager from the service manager :
    if (needs_geometry_manager && ! has_geometry_manager()) {
      DT_THROW_IF(! has_service_manager(), std::logic_error, "No service manager is available !");

      const std::string geo_service_name = setup_.fetch_string("services.geometry");
      DT_THROW_IF(! _service_manager_->has(geo_service_name) ||
                  ! _service_manager_->is_a<geomtools::geometry_service>(geo_service_name),
                  std::logic_error,
                  "Cannot find a geometry service named '" << geo_service_name << "' "
                  << "from the service manager !");

      const geomtools::geometry_service & Geo
        = _service_manager_->get<geomtools::geometry_service>(geo_service_name);
      const geomtools::manager & geomgr = Geo.get_geom_manager();
      set_geometry_manager(geomgr);
      DT_LOG_DEBUG(get_logging_priority(),
                   "Found geometry manager '" << _geom_manager_->get_setup_label() << "-"
                   << _geom_manager_->get_setup_version() << "'");
    }

    std::vector<std::string> field_definitions_filenames;
    if (setup_.has_key("field_definitions_filenames")) {
      setup_.fetch("field_definitions_filenames", field_definitions_filenames);
    } /* else if (setup_.has_key("field_definitions")) {
      setup_.fetch("field_definitions", field_definitions_filenames);
      }*/
    for (size_t i = 0; i < field_definitions_filenames.size(); i++) {
      load(field_definitions_filenames[i]);
    }

    // Initialization :
    _construct_();

    if (setup_.has_flag("build_geom_map")) {
      datatools::properties geomap_config;
      if (setup_.has_key("geom_map_config")) {
        std::string geomap_config_file = setup_.fetch_string("geom_map_config");
        datatools::fetch_path_with_env(geomap_config_file);
        datatools::properties::read_config(geomap_config_file, geomap_config);
      } else {
        setup_.export_and_rename_starting_with(geomap_config, "geom_map.", "");
      }
      if (get_logging_priority() >= datatools::logger::PRIO_DEBUG) {
        geomap_config.tree_dump(std::clog, "EM fields/Geometry map config: ", "[debug]: ");
      }
      _construct_geomap_(geomap_config);
    }

    _set_initialized(true);
    return;
  }

  void electromagnetic_field_manager::reset()
  {
    DT_THROW_IF(! is_initialized(), std::logic_error,
                "Cannot reset the electromagnetic field manager ! EM field manager is not initialized !");
    _set_initialized(false);
    _fields_.clear();
    _field_definitions_.reset();
    _geom_manager_ = 0;
    if (_geom_map_.get() != 0) {
      if (_geom_map_.get()->is_initialized()) {
        _geom_map_.get()->reset();
      }
      _geom_map_.reset(0);
    }
    reset_service_manager();
    _factory_register_.reset(0);
    _set_defaults();
    return;
  }

  const base_electromagnetic_field::field_dict_type & electromagnetic_field_manager::get_fields() const
  {
    return _fields_;
  }

  bool electromagnetic_field_manager::has_field(const std::string & field_name_) const
  {
    return _fields_.find(field_name_) != _fields_.end();
  }

  base_electromagnetic_field & electromagnetic_field_manager::grab_field(const std::string & field_name_)
  {
    base_electromagnetic_field::field_dict_type::iterator found
      = _fields_.find(field_name_);
    DT_THROW_IF(found == _fields_.end(), std::logic_error, "Cannot find EM field named '" << field_name_ << "' !");
    return found->second.grab();
  }

  const base_electromagnetic_field & electromagnetic_field_manager::get_field(const std::string & field_name_) const
  {
    base_electromagnetic_field::field_dict_type::const_iterator found
      = _fields_.find(field_name_);
    DT_THROW_IF(found == _fields_.end(), std::logic_error, "Cannot find EM field named '" << field_name_ << "' !");
    return found->second.get();
  }

  const geom_map & electromagnetic_field_manager::get_geom_map() const
  {
    DT_THROW_IF(! has_geom_map(), std::logic_error, "No geom map is available !");
    return *_geom_map_.get();
  }

  void electromagnetic_field_manager::tree_dump(std::ostream & out_,
                                                const std::string & title_,
                                                const std::string & indent_,
                                                bool inherit_) const
  {
    std::string indent;
    if (! indent_.empty()) indent = indent_;
    if (! title_.empty()) {
      out_ << indent << title_ << std::endl;
    }

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Initialized  : " << (_initialized_? "Yes": "No") << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Logging priority : '" << datatools::logger::get_priority_label(get_logging_priority())
         << "'" << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Factory verbose : " << _factory_verbose_ << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Factory preload : " << _factory_preload_ << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Build geometry/EM-field map : " << _build_geom_map_ << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Factory register : ";
    if (!_factory_register_) {
      out_ << "<none>";
    }
    out_ << std::endl;
    if (_factory_register_) {
      std::ostringstream indent_oss;
      indent_oss << indent_ << datatools::i_tree_dumpable::skip_tag;
      _factory_register_->tree_dump(out_, "", indent_oss.str());
    }

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Service manager : ";
    if (!_service_manager_) {
      out_ << "<none>";
    } else {
      out_ << "[@" << _service_manager_ << "]";
    }
    out_ << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Geometry manager : ";
    if (!_geom_manager_) {
      out_ << "<none>";
    } else {
      out_ << "[@" << _geom_manager_ << "]";
    }
    out_ << std::endl;

    if (! _initialized_)  {
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Field definitions : ";
      if( _field_definitions_.entries().size() == 0) {
        out_ << "<empty>";
      }
      out_ << std::endl;
      {
        std::ostringstream indent_oss;
        indent_oss << indent;
        indent_oss << datatools::i_tree_dumpable::skip_tag;
        _field_definitions_.tree_dump(out_, "", indent_oss.str());
      }
    }

    {
      out_ << indent << datatools::i_tree_dumpable::tag
           << "EM fields : ";
      if ( _fields_.size() == 0) {
        out_ << "<empty>";
      } else {
        out_ << "[" << _fields_.size() << "]";
      }
      out_ << std::endl;
      for (base_electromagnetic_field::field_dict_type::const_iterator i = _fields_.begin();
           i != _fields_.end(); ++i) {
        const std::string & field_name = i->first;
        const base_electromagnetic_field & a_field = i->second.get();
        std::ostringstream indent_oss;
        out_ << indent << datatools::i_tree_dumpable::skip_tag;
        indent_oss << indent << datatools::i_tree_dumpable::skip_tag;
        base_electromagnetic_field::field_dict_type::const_iterator j = i;
        j++;
        if (j == _fields_.end()) {
          out_ << datatools::i_tree_dumpable::last_tag;
          indent_oss << datatools::i_tree_dumpable::last_skip_tag;
        } else {
          out_ << datatools::i_tree_dumpable::tag;
          indent_oss << datatools::i_tree_dumpable::skip_tag;
        }
        out_ << "EM field : " << '"' << field_name << '"' << std::endl;
        a_field.tree_dump(out_, "", indent_oss.str());
      }
    }

    {
      out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_)
           << "Geometry field map : " << has_geom_map() << std::endl;
      if (has_geom_map()) {
        std::ostringstream indent_oss;
        indent_oss << indent << datatools::i_tree_dumpable::inherit_skip_tag(inherit_);
        _geom_map_.get()->tree_dump(out_, "", indent_oss.str(), inherit_);
      }
    }
    return;
  }

} // end of namespace emfield
