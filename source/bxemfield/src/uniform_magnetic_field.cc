// uniform_magnetic_field.cc

// Ourselves:
#include <emfield/uniform_magnetic_field.h>

// Third party:
#include <datatools/properties.h>
#include <datatools/units.h>
#include <datatools/utils.h>
#include <datatools/service_manager.h>

namespace emfield {

  // Registration instantiation macro :
  EMFIELD_REGISTRATION_IMPLEMENT(uniform_magnetic_field, "emfield::uniform_magnetic_field");

  void uniform_magnetic_field::_set_defaults()
  {
    _set_electric_field(false);
    _set_electric_field_can_be_combined(false);
    _set_electric_field_is_time_dependent(false);
    _set_magnetic_field(true);
    _set_magnetic_field_can_be_combined(false);
    _set_magnetic_field_is_time_dependent(false);
    geomtools::invalidate(_uniform_magnetic_field_);
    return;
  }

  // Constructor :
  uniform_magnetic_field::uniform_magnetic_field(uint32_t flags_)
    : ::emfield::base_electromagnetic_field(flags_)
  {
    _set_defaults();
    return;
  }

  // Destructor :
  uniform_magnetic_field::~uniform_magnetic_field()
  {
    if (is_initialized()) {
      reset();
    }
    return;
  }

  // Getter :
  const geomtools::vector_3d & uniform_magnetic_field::get_uniform_magnetic_field() const
  {
    return _uniform_magnetic_field_;
  }

  // Setter :
  void uniform_magnetic_field::set_uniform_magnetic_field(const geomtools::vector_3d & b_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Cannot change the magnetic field value !");
    _uniform_magnetic_field_ = b_;
    return;
  }

  int uniform_magnetic_field::compute_electric_field(const geomtools::vector_3d & /* position_ */,
                                                      double /* time_ */,
                                                      geomtools::vector_3d & electric_field_) const
  {
    geomtools::invalidate(electric_field_);
    return STATUS_ERROR;
  }

  int uniform_magnetic_field::compute_magnetic_field(const ::geomtools::vector_3d & /* position_ */,
                                                     double  /* time_ */,
                                                     ::geomtools::vector_3d & magnetic_field_) const
  {
    // DT_LOG_DEBUG(get_logging_priority(), "*** COMPUTE UNIFORM MAGNETIC FIELD ***");
    magnetic_field_ = _uniform_magnetic_field_;
    DT_LOG_DEBUG(get_logging_priority(),
                 "Magnetic field values = "
                 << magnetic_field_ / CLHEP::gauss << " G");
    return STATUS_SUCCESS;
  }

  void uniform_magnetic_field::reset()
  {
    DT_THROW_IF(! is_initialized(), std::logic_error, "Cannot reset the uniform magnetic field !");
    _set_initialized(false);
    _set_defaults();
    this->base_electromagnetic_field::_set_defaults();
    return;
  }

  void uniform_magnetic_field::initialize(const ::datatools::properties & setup_,
                                          ::datatools::service_manager & service_manager_,
                                          base_electromagnetic_field::field_dict_type & fields_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Field is already initialized !");

    base_electromagnetic_field::_parse_basic_parameters(setup_, service_manager_, fields_);
    _set_electric_field(false);
    _set_magnetic_field(true);
    _set_magnetic_field_is_time_dependent(false);

    double b_unit = CLHEP::gauss;
    if (setup_.has_key("magnetic_field.unit")) {
      const std::string b_unit_str = setup_.fetch_string("magnetic_field.unit");
      b_unit = datatools::units::get_magnetic_field_unit_from(b_unit_str);
    }

    if (setup_.has_flag("magnetic_field.can_be_combined")) {
      _set_magnetic_field_can_be_combined(true);
    }

    if (! geomtools::is_valid(_uniform_magnetic_field_)) {
      if (setup_.has_key("magnetic_field.coordinates")) {
        std::vector<double> magfield_coord;
        setup_.fetch("magnetic_field.coordinates", magfield_coord);
        DT_THROW_IF(magfield_coord.size() != 3, std::logic_error, "Invalid magnetic field vector 'magnetic_field' !");
        _uniform_magnetic_field_.set(magfield_coord[0],
                                     magfield_coord[1],
                                     magfield_coord[2]);
        if (! setup_.has_explicit_unit("magnetic_field.coordinates")) {
          _uniform_magnetic_field_ *= b_unit;
        }
      }
    }

    if (! geomtools::is_valid(_uniform_magnetic_field_)) {
      double magfield_amp;
      datatools::invalidate(magfield_amp);
      if (setup_.has_key("magnetic_field.magnitude")) {
        magfield_amp = setup_.fetch_real("magnetic_field.magnitude");
        if (! setup_.has_explicit_unit("magnetic_field.magnitude")) {
          magfield_amp *= b_unit;
        }
      }

      int magfield_axis = geomtools::ROTATION_AXIS_INVALID;
      if (setup_.has_key("magnetic_field.axis")) {
        const std::string axis_str = setup_.fetch_string("magnetic_field.axis");
        magfield_axis = geomtools::get_rotation_axis_from_label(axis_str);
        DT_THROW_IF(magfield_axis == geomtools::ROTATION_AXIS_INVALID, std::logic_error, "Invalid 'magnetic_field.axis' !");
      }

      geomtools::vector_3d magfield_direction(0., 0., 0.);
      if (magfield_axis == geomtools::ROTATION_AXIS_X) {
        magfield_direction.setX(1.0);
      } else if (magfield_axis == geomtools::ROTATION_AXIS_Y) {
        magfield_direction.setY(1.0);
      } else if (magfield_axis == geomtools::ROTATION_AXIS_Z) {
        magfield_direction.setZ(1.0);
      }

      _uniform_magnetic_field_ = magfield_amp * magfield_direction;
    }

    _set_initialized(true);
    return;
  }

  void uniform_magnetic_field::tree_dump(std::ostream & out_,
                                         const std::string & title_,
                                         const std::string & indent_,
                                         bool inherit_) const
  {
    this->base_electromagnetic_field::tree_dump(out_, title_, indent_, true);

    out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
         << "Uniform magnetic field  : " << _uniform_magnetic_field_ / (CLHEP::tesla) << " T" << std::endl;

    return;
  }

} // end of namespace emfield
