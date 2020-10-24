// multi_zone_field.cc

// Ourselves:
#include <emfield/multi_zone_field.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/units.h>
#include <datatools/utils.h>
#include <datatools/service_manager.h>
#include <datatools/exception.h>
// - Bayeux/geomtools:
#include <geomtools/utils.h>
#include <geomtools/geometry_service.h>
#include <geomtools/manager.h>

namespace emfield {

  // Registration instantiation macro :
  EMFIELD_REGISTRATION_IMPLEMENT(multi_zone_field, "emfield::multi_zone_field")

  // static
  std::string multi_zone_field::zone_priority_to_label(zone_priority_type p_)
  {
    switch (p_) {
    case ZONE_PRIORITY_VERY_LOW:  return std::string("very_low");
    case ZONE_PRIORITY_LOW:       return std::string("low");
    case ZONE_PRIORITY_HIGH:      return std::string("high");
    case ZONE_PRIORITY_VERY_HIGH: return std::string("very_high");
    default: return std::string("");
    }
  }

  // static
  multi_zone_field::zone_priority_type multi_zone_field::label_to_zone_priority(const std::string & label_)
  {
    if (label_ == zone_priority_to_label(ZONE_PRIORITY_VERY_LOW)) return ZONE_PRIORITY_VERY_LOW;
    if (label_ == zone_priority_to_label(ZONE_PRIORITY_LOW)) return ZONE_PRIORITY_LOW;
    if (label_ == zone_priority_to_label(ZONE_PRIORITY_HIGH)) return ZONE_PRIORITY_HIGH;
    if (label_ == zone_priority_to_label(ZONE_PRIORITY_VERY_HIGH)) return ZONE_PRIORITY_VERY_HIGH;
    if (label_ == "min") return ZONE_PRIORITY_MIN;
    if (label_ == "max") return ZONE_PRIORITY_MAX;
    if (label_ == "default") return ZONE_PRIORITY_DEFAULT;
    return ZONE_PRIORITY_INVALID;
  }

  // static
  bool multi_zone_field::zone_field_entry::_higher_zone_priority(const zone_field_entry * zfe1_,
                                                                 const zone_field_entry * zfe2_)
  {
    return zfe1_->_priority_ > zfe2_->_priority_;
  }

  void multi_zone_field::zone_field_entry::_set_defaults()
  {
    _zone_positioning_.reset();
    _zone_shape_ = 0;
    _zone_tolerance_ = 0.0;
    _zone_field_ = 0;
    _absolute_positioning_ = true;
    _priority_ = ZONE_PRIORITY_DEFAULT;
    return;
  }

  multi_zone_field::zone_field_entry::zone_field_entry()
  {
    _set_defaults();
    return;
  }

  void multi_zone_field::zone_field_entry::reset()
  {
    _set_defaults();
    return;
  }

  void multi_zone_field::_set_defaults()
  {
    // _set_electric_field(true);
    // _set_electric_field_can_be_combined(true);
    // _set_magnetic_field(true);
    // _set_magnetic_field_can_be_combined(true);
    return;
  }

  // Constructor :
  multi_zone_field::multi_zone_field(uint32_t flags_)
    : ::emfield::base_electromagnetic_field(flags_)
  {
    _own_shape_factory_ = false;
    _shape_factory_ = 0;
    _set_defaults();
    return;
  }

  // Destructor :
  multi_zone_field::~multi_zone_field()
  {
    if (is_initialized()) {
      reset();
    }
    return;
  }

  const multi_zone_field::zone_field_entry &
  multi_zone_field::get_zone_field(const std::string & zone_label_) const
  {
    zone_field_dict_type::const_iterator found = _zone_fields_.find(zone_label_);
    DT_THROW_IF(found == _zone_fields_.end(), std::logic_error,
                "No zone field with label '" << zone_label_ << "'!");
    return found->second;
  }

  void multi_zone_field::fine_zones(const geomtools::vector_3d & position_,
                                   std::vector<const zone_field_entry *> & zones_) const
  {
    datatools::logger::priority local_priority = datatools::logger::PRIO_WARNING;
    // local_priority = datatools::logger::PRIO_TRACE;
    DT_LOG_TRACE_ENTERING(local_priority);
    zones_.clear();
    for (zone_field_dict_type::const_iterator i = _zone_fields_.begin();
         i != _zone_fields_.end();
         i++) {
      DT_LOG_TRACE(local_priority, "Checking inside zone '" << i->first << "'...");
      const zone_field_entry & zfe = i->second;
      DT_LOG_TRACE(local_priority, "zone tolerance = " << zfe._zone_tolerance_ / CLHEP::mm << " mm");
      geomtools::vector_3d local_pos;
      zfe._zone_positioning_.mother_to_child(position_, local_pos);
      if (zfe._zone_shape_->check_inside(local_pos, zfe._zone_tolerance_)) {
        DT_LOG_TRACE(local_priority, "inside zone '" << i->first << "'...");
        zones_.push_back(&zfe);
      }
    }
    if (zones_.size() > 1) {
      std::sort(zones_.begin(), zones_.end(), zone_field_entry::_higher_zone_priority);
      zone_priority_type top_priority = zones_[0]->_priority_;
      std::vector<const zone_field_entry *>::iterator cut_iter = zones_.end();
      for (std::vector<const zone_field_entry *>::iterator zone_iter = zones_.begin();
           zone_iter != zones_.end();
           zone_iter++) {
        const zone_field_entry & zfe_ptr = **zone_iter;
        if (zfe_ptr._priority_ < top_priority) {
          cut_iter = zone_iter;
          break;
        }
      }
      if (cut_iter != zones_.end()) {
        zones_.erase(cut_iter, zones_.end());
      }
    }
    DT_LOG_TRACE_EXITING(local_priority);
    return;
  }

  void multi_zone_field::add_zone_field(const std::string & zone_label_,
                                        const geomtools::placement & zone_positioning_,
                                        const geomtools::i_shape_3d & zone_shape_,
                                        double zone_tolerance_,
                                        const base_electromagnetic_field & zone_field_,
                                        bool absolute_positioning_,
                                        zone_priority_type priority_)
  {
    DT_THROW_IF (is_initialized (), std::logic_error, "Multi zone field is locked !");
    zone_field_dict_type::const_iterator found = _zone_fields_.find(zone_label_);
    DT_THROW_IF(found != _zone_fields_.end(), std::logic_error,
                "A zone field with label '" << zone_label_ << "' already exists!");
    {
      zone_field_entry dummy;
      _zone_fields_[zone_label_] = dummy;
    }
    zone_field_entry & zfe = _zone_fields_.find(zone_label_)->second;
    zfe._label_ = zone_label_;
    zfe._zone_positioning_ = zone_positioning_;
    DT_THROW_IF(!zone_shape_.is_locked(),
                std::logic_error,
                "Zone '" << zone_label_ << "' shape is not locked!");
    zfe._zone_shape_           = &zone_shape_;
    zfe._zone_tolerance_       =  zone_tolerance_;
    zfe._zone_field_           = &zone_field_;
    zfe._absolute_positioning_ =  absolute_positioning_;
    zfe._priority_             =  priority_;
    return;
  }

  int multi_zone_field::compute_electric_field(const ::geomtools::vector_3d & position_,
                                               double time_,
                                               ::geomtools::vector_3d & electric_field_) const
  {
    datatools::logger::priority local_priority = datatools::logger::PRIO_WARNING;
    DT_LOG_TRACE_ENTERING(local_priority);
    geomtools::invalidate(electric_field_);
    if (! is_electric_field()) {
      return STATUS_ERROR;
    }
    std::vector<const zone_field_entry *> zones;
    fine_zones(position_, zones);
    DT_LOG_TRACE(local_priority, "zones = [" << zones.size() << ']');
    electric_field_.set(0.0, 0.0, 0.0);
    if (zones.size() > 0) {
      std::vector<geomtools::vector_3d> fields;
      fields.reserve(zones.size());

      for (size_t izone = 0; izone < zones.size(); izone++) {
        const zone_field_entry & zfe = *zones[izone];
        geomtools::vector_3d zone_pos;
        geomtools::vector_3d zone_field_value;
        if (zfe._absolute_positioning_) {
          zone_pos = position_;
        } else {
          zfe._zone_positioning_.mother_to_child(position_, zone_pos);
        }
        int status = zfe._zone_field_->compute_electric_field(zone_pos,
                                                              time_,
                                                              zone_field_value);
        if (status != STATUS_SUCCESS) {
          return status;
        }
        if (zfe._absolute_positioning_) {
          fields.push_back(zone_field_value);
        } else {
          fields.push_back(zone_field_value);
          zfe._zone_positioning_.child_to_mother(zone_field_value, fields.back());
        }
      }

      for (int ifield = 0; ifield < (int) fields.size(); ifield++) {
        electric_field_ += fields[ifield];
      }
      electric_field_ /= fields.size();
    }

    DT_LOG_TRACE_EXITING(local_priority);
    return STATUS_SUCCESS;
  }

  int multi_zone_field::compute_magnetic_field(const ::geomtools::vector_3d & position_,
                                               double time_,
                                               ::geomtools::vector_3d & magnetic_field_) const
  {
    datatools::logger::priority local_priority = datatools::logger::PRIO_WARNING;
    DT_LOG_TRACE_ENTERING(local_priority);
    geomtools::invalidate(magnetic_field_);
    if (! is_magnetic_field()) {
      return STATUS_ERROR;
    }
    std::vector<const zone_field_entry*> zones;
    fine_zones(position_, zones);
    magnetic_field_.set(0.0, 0.0, 0.0);
    if (zones.size() > 0) {

      if (local_priority >= datatools::logger::PRIO_TRACE) {
        DT_LOG_TRACE(local_priority, "Found zones: [#" << zones.size() << ']');
        for (size_t izone = 0; izone < zones.size(); izone++) {
          DT_LOG_TRACE(local_priority, "in zone: " << zones[izone]->_label_);
        }
      }

      std::vector<geomtools::vector_3d> fields;
      fields.reserve(zones.size());

      for (int izone = 0; izone < (int) zones.size(); izone++) {
        const zone_field_entry & zfe = *zones[izone];
        geomtools::vector_3d zone_pos;
        geomtools::vector_3d zone_field_value;
        if (zfe._absolute_positioning_) {
          zone_pos = position_;
        } else {
          zfe._zone_positioning_.mother_to_child(position_, zone_pos);
        }
        int status = zfe._zone_field_->compute_magnetic_field(zone_pos,
                                                              time_,
                                                              zone_field_value);
        if (status != STATUS_SUCCESS) {
          return status;
        }
        if (zfe._absolute_positioning_) {
          fields.push_back(zone_field_value);
        } else {
          fields.push_back(zone_field_value);
          zfe._zone_positioning_.child_to_mother(zone_field_value, fields.back());
        }
      }

      // Strategy: average field from overlapping zones
      for (int ifield = 0; ifield < (int) fields.size(); ifield++) {
        magnetic_field_ += fields[ifield];
      }
      magnetic_field_ /= fields.size();
    }
    DT_LOG_TRACE_EXITING(local_priority);
    return STATUS_SUCCESS;
  }

  void multi_zone_field::reset()
  {
    DT_THROW_IF(! is_initialized(), std::logic_error, "Cannot reset the multi zone field !");
    _set_initialized(false);
    reset_shape_factory();
    _zone_fields_.clear();
    _set_defaults();
    this->base_electromagnetic_field::_set_defaults();
    return;
  }

  void multi_zone_field::set_shape_factory(const geomtools::shape_factory & shfact_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Field is already initialized !");
    _own_shape_factory_ = false;
    _shape_factory_ = const_cast<geomtools::shape_factory *>(&shfact_);
    return;
  }

  void multi_zone_field::reset_shape_factory()
  {
    if (_own_shape_factory_ && _shape_factory_ != 0) {
      if (_shape_factory_->is_initialized()) {
        _shape_factory_->reset();
      }
      delete _shape_factory_;
    }
    _shape_factory_ = 0;
    _own_shape_factory_ = false;
    return;
  }

  bool multi_zone_field::owns_shape_factory() const
  {
    return has_shape_factory() && _own_shape_factory_;
  }

  bool multi_zone_field::has_shape_factory() const
  {
    return _shape_factory_ != 0;
  }

  void multi_zone_field::create_shape_factory(const ::datatools::properties & setup_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Field is already initialized !");
    DT_THROW_IF(has_shape_factory(), std::logic_error, "Field already has a shape factory !");
    _own_shape_factory_ = true;
    _shape_factory_ = new geomtools::shape_factory;

    datatools::properties shape_factory_config;
    if (setup_.has_key("shape_factory_config")) {
      // Specify the configuration file for the embedded shape factory:
      std::string shape_factory_config_filename = setup_.fetch_string("shape_factory_config");
      datatools::fetch_path_with_env(shape_factory_config_filename);
      datatools::read_config(shape_factory_config_filename, shape_factory_config);
    } else {
      // Or extract shape factory parameters :
      setup_.export_and_rename_starting_with(shape_factory_config, "shape_factory.", "");
    }
    _shape_factory_->initialize(shape_factory_config);
    return;
  }

  void multi_zone_field::initialize(const ::datatools::properties & setup_,
                                    ::datatools::service_manager & service_manager_,
                                    base_electromagnetic_field::field_dict_type & fields_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Field is already initialized !");

    base_electromagnetic_field::_parse_basic_parameters(setup_, service_manager_, fields_);

    if (! has_shape_factory()) {
      std::string geometry_service_name;
      if (setup_.has_key("geometry_service")) {
        geometry_service_name = setup_.fetch_string("geometry_service");
      }
      if (! geometry_service_name.empty()) {
        // If a geometry service is requested, try to extract the shape factory from it:
        if (service_manager_.has(geometry_service_name)
            && service_manager_.is_a<geomtools::geometry_service>(geometry_service_name)) {
          // Fetch the geometry service:
          const geomtools::geometry_service & geo
            = service_manager_.get<geomtools::geometry_service>(geometry_service_name);
          // Fetch the geometry manager:
          const geomtools::manager & geom_mgr = geo.get_geom_manager();
          // Fetch the shape factory:
          set_shape_factory(geom_mgr.get_shape_factory());
        }
      } else {
        // Create our own shape factory:
        create_shape_factory(setup_);
      }
    }

    DT_THROW_IF(! has_shape_factory(), std::logic_error, "No shape factory is available!");

    if (_zone_fields_.size() == 0) {
      DT_THROW_IF(!setup_.has_key("zones"), std::logic_error,
                  "Missing 'zones' property!");
      std::vector<std::string> zone_labels;
      setup_.fetch("zones", zone_labels);

      for (int izone = 0; izone < (int) zone_labels.size(); izone++) {
        const std::string & zone_label = zone_labels[izone];
        geomtools::placement zone_pos;
        zone_pos.reset();
        double zone_tol = 0.0;
        const geomtools::i_object_3d * zone_shape = 0;
        bool zone_abspos = true;
        const base_electromagnetic_field * zone_field = 0;
        zone_priority_type zone_priority = ZONE_PRIORITY_DEFAULT;

        {
          std::ostringstream zone_priority_oss;
          zone_priority_oss << "zones." << zone_label << ".priority";
          if (setup_.has_key(zone_priority_oss.str())) {
            const std::string & zone_priority_str = setup_.fetch_string(zone_priority_oss.str());
            zone_priority = label_to_zone_priority(zone_priority_str);
            DT_THROW_IF(zone_priority == ZONE_PRIORITY_INVALID, std::range_error,
                        "Invalid zone priority '" << zone_priority_str << "'!");
          }
        }

        {
          std::ostringstream zone_pos_oss;
          zone_pos_oss << "zones." << zone_label << ".positioning";
          if (setup_.has_key(zone_pos_oss.str())) {
            const std::string & zone_pos_str = setup_.fetch_string(zone_pos_oss.str());
            DT_THROW_IF(!geomtools::placement::from_string(zone_pos_str, zone_pos),
                        std::logic_error,
                        "Zone positioning parsing error!");
          }
        }

        {
          std::ostringstream zone_tol_oss;
          zone_tol_oss << "zones." << zone_label << ".tolerance";
          if (setup_.has_key(zone_tol_oss.str())) {
            zone_tol = setup_.fetch_real(zone_tol_oss.str());
            DT_THROW_IF(zone_tol < 0.0,
                        std::logic_error,
                        "Zone invalid tolerance!");
          }
        }

        {
          std::ostringstream zone_abspos_oss;
          zone_abspos_oss << "zones." << zone_label << ".absolute";
          if (setup_.has_key(zone_abspos_oss.str())) {
            zone_abspos = setup_.fetch_boolean(zone_abspos_oss.str());
          }
        }

        {
          std::ostringstream zone_shape_ref_oss;
          zone_shape_ref_oss << "zones." << zone_label << ".shape_ref";
          if (setup_.has_key(zone_shape_ref_oss.str())) {
            const std::string & zone_shape_ref = setup_.fetch_string(zone_shape_ref_oss.str());
            if (_shape_factory_->has(zone_shape_ref)) {
              zone_shape = &_shape_factory_->get(zone_shape_ref);
            }
          }
        }

        {
          std::ostringstream zone_field_name_oss;
          zone_field_name_oss << "zones." << zone_label << ".field";
          std::string zone_field_name;
          if (setup_.has_key(zone_field_name_oss.str())) {
            zone_field_name = setup_.fetch_string(zone_field_name_oss.str());
          }
          DT_THROW_IF(zone_field_name.empty(), std::logic_error, "Missing field name!");
          base_electromagnetic_field::field_dict_type::iterator zone_field_found
            = fields_.find(zone_field_name);
          DT_THROW_IF (zone_field_found == fields_.end (),
                       std::logic_error,
                       "Cannot find a EM field with name '" << zone_field_name << "' !");
          zone_field = &zone_field_found->second.get();
        }

        add_zone_field(zone_label,
                       zone_pos,
                       dynamic_cast<const geomtools::i_shape_3d &>(*zone_shape),
                       zone_tol,
                       *zone_field,
                       zone_abspos,
                       zone_priority);

      } // for (int izone
    } // (_zone_fields_.size() == 0)

    // Checks:
    {
      size_t efield_count = 0;
      size_t mfield_count = 0;

      for (zone_field_dict_type::const_iterator izone = _zone_fields_.begin();
           izone != _zone_fields_.end();
           izone++) {
        const zone_field_entry & zfe = izone->second;
        const base_electromagnetic_field & zone_field = *zfe._zone_field_;
        if (zone_field.is_electric_field()) {
          efield_count++;
        }
        if (zone_field.is_magnetic_field()) {
          mfield_count++;
        }
      }

      if (efield_count == 0) {
        _set_electric_field(false);
      } else if (efield_count == _zone_fields_.size()) {
        _set_electric_field(true);
      } else {
        DT_THROW(std::logic_error, "Not all zone fields are electric fields!");
      }

      if (mfield_count == 0) {
        _set_magnetic_field(false);
      } else if (mfield_count == _zone_fields_.size()) {
        _set_magnetic_field(true);
      } else {
        DT_THROW(std::logic_error, "Not all zone fields are magnetic fields!");
      }
    }

    _set_initialized(true);
    return;
  }

  void multi_zone_field::tree_dump(std::ostream & out_,
                                   const std::string & title_,
                                   const std::string & indent_,
                                   bool inherit_) const
  {
    this->base_electromagnetic_field::tree_dump(out_, title_, indent_, true);

    out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
         << "Shape factory: " << has_shape_factory()
         << " [@" << _shape_factory_ << "] " << ( owns_shape_factory() ? "(embedded)" : "(external)")
         << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
         << "Zone fields: ";
    if (_zone_fields_.size()) {
      out_ << '[' << _zone_fields_.size() << ']';
    } else {
      out_ << "<none>";
    }
    out_ << std::endl;

    for (zone_field_dict_type::const_iterator izone = _zone_fields_.begin();
         izone != _zone_fields_.end();
         izone++) {
      out_ << indent_ << datatools::i_tree_dumpable::inherit_skip_tag(inherit_);
      zone_field_dict_type::const_iterator jzone = izone;
      jzone++;
      if (jzone == _zone_fields_.end()) {
        out_ << datatools::i_tree_dumpable::last_tag;
      } else {
        out_ << datatools::i_tree_dumpable::tag;
      }
      out_ << "Zone field label '" << izone->first << "' :"
           << " EM field '" << izone->second._zone_field_->get_name() << "' "
           << "(class ID='" << izone->second._zone_field_->get_class_id() << "' "
           << "[@" << izone->second._zone_field_ << "]) with placement='"
           <<  izone->second._zone_positioning_ << "' in shape of type '"
           <<  izone->second._zone_shape_->get_shape_name() << "'"
           << " priority='" << zone_priority_to_label(izone->second._priority_) << "'" << std::endl;
      out_ << std::endl;
    }

    return;
  }

} // end of namespace emfield
