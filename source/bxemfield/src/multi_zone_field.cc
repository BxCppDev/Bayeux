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

namespace emfield {

  // Registration instantiation macro :
  EMFIELD_REGISTRATION_IMPLEMENT(multi_zone_field, "emfield::multi_zone_field");

  multi_zone_field::zone_field_entry::zone_field_entry()
  {
    _zone_positioning_.reset();
    _zone_shape_ = 0;
    _zone_tolerance_ = 0.0;
    _zone_field_ = 0;
    _absolute_positioning_ = true;
    return;
  }

  void multi_zone_field::zone_field_entry::reset()
  {
    _label_.clear();
    _zone_positioning_.reset();
    _zone_shape_ = 0;
    _zone_tolerance_ = 0.0;
    _zone_field_ = 0;
    _absolute_positioning_ = true;
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

  void multi_zone_field::find_zone(const geomtools::vector_3d & position_,
                                   std::vector<const zone_field_entry *> & zones_) const
  {
    std::cerr << "DEVEL: multi_zone_field::find_zone: "
              << "Entering..."
              << std::endl;
    zones_.clear();
    for (zone_field_dict_type::const_iterator i = _zone_fields_.begin();
         i != _zone_fields_.end();
         i++) {
      std::cerr << "DEVEL: multi_zone_field::find_zone: "
                << "Checking inside zone '" << i->first << "'..."
                << std::endl;
      const zone_field_entry & zfe = i->second;
      std::cerr << "DEVEL: multi_zone_field::find_zone: "
                << "  zone tolerance = " << zfe._zone_tolerance_ / CLHEP::mm << " mm"
                << std::endl;
      geomtools::vector_3d local_pos;
      zfe._zone_positioning_.mother_to_child(position_, local_pos);
      if (zfe._zone_shape_->check_inside(local_pos, zfe._zone_tolerance_)) {
        std::cerr << "DEVEL: multi_zone_field::find_zone: "
                  << "  inside zone '" << i->first << "'..."
                  << std::endl;
        zones_.push_back(&zfe);
      }
    }
    std::cerr << "DEVEL: multi_zone_field::find_zone: "
              << "Exiting."
              << std::endl;
    return;
  }

  void multi_zone_field::add_zone_field(const std::string & zone_label_,
                                        const geomtools::placement & zone_positioning_,
                                        const geomtools::i_shape_3d & zone_shape_,
                                        double zone_tolerance_,
                                        const base_electromagnetic_field & zone_field_,
                                        bool absolute_positioning_)
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
    return;
  }

  int multi_zone_field::compute_electric_field(const ::geomtools::vector_3d & position_,
                                               double time_,
                                               ::geomtools::vector_3d & electric_field_) const
  {
    std::cerr << "DEVEL: multi_zone_field::compute_electric_field: "
              << "Entering..."
              << std::endl;
    geomtools::invalidate(electric_field_);
    if (! is_electric_field()) {
      return STATUS_ERROR;
    }
    std::vector<const zone_field_entry *> zones;
    find_zone(position_, zones);
    std::cerr << "DEVEL: multi_zone_field::compute_electric_field: "
              << "zones = [" << zones.size() << ']'
              << std::endl;
    electric_field_.set(0.0, 0.0, 0.0);
    if (zones.size() > 0) {

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

    return STATUS_SUCCESS;
  }

  int multi_zone_field::compute_magnetic_field(const ::geomtools::vector_3d & position_,
                                               double time_,
                                               ::geomtools::vector_3d & magnetic_field_) const
  {
    geomtools::invalidate(magnetic_field_);
    if (! is_magnetic_field()) {
      return STATUS_ERROR;
    }
    std::vector<const zone_field_entry*> zones;
    find_zone(position_, zones);
    magnetic_field_.set(0.0, 0.0, 0.0);
    if (zones.size() > 0) {

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

    return STATUS_SUCCESS;
  }

  void multi_zone_field::reset()
  {
    DT_THROW_IF(! is_initialized(), std::logic_error, "Cannot reset the multi zone field !");
    _set_initialized(false);
    _zone_fields_.clear();
    _set_defaults();
    this->base_electromagnetic_field::_set_defaults();
    return;
  }

  void multi_zone_field::initialize(const ::datatools::properties & setup_,
                                    ::datatools::service_manager & service_manager_,
                                    base_electromagnetic_field::field_dict_type & fields_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Field is already initialized !");

    base_electromagnetic_field::_parse_basic_parameters(setup_, service_manager_, fields_);

    double length_unit = CLHEP::millimeter;
    double angle_unit  = CLHEP::degree;

    if (setup_.has_key ("length.unit")) {
      const std::string length_unit_str = setup_.fetch_string ("length.unit");
      length_unit = datatools::units::get_length_unit_from (length_unit_str);
    }

    if (setup_.has_key ("angle.unit")) {
      const std::string angle_unit_str = setup_.fetch_string ("angle.unit");
      angle_unit = datatools::units::get_angle_unit_from (angle_unit_str);
    }

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
        const geomtools::i_shape_3d * zone_shape = 0;
        bool zone_abspos = true;
        const base_electromagnetic_field * zone_field = 0;

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
                       *zone_shape,
                       zone_tol,
                       *zone_field,
                       zone_abspos);

      } // for (int izone
    } // (_zone_fields_.size() == 0)

    // Checks:
    {
      int efield_count = 0;
      int mfield_count = 0;

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
      out_ << izone->first << " : ";
      out_ << std::endl;

    }

    return;
  }

} // end of namespace emfield
