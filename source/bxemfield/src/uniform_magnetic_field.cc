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

  // Constructor :
  EMFIELD_CONSTRUCTOR_IMPLEMENT_HEAD(uniform_magnetic_field,flags_)
  {
    _set_electric_field(false);
    _set_electric_field_can_be_combined(false);
    _set_magnetic_field(true);
    _set_magnetic_field_can_be_combined(false);
    geomtools::invalidate(_uniform_magnetic_field_);
    return;
  }

  // Destructor :
  EMFIELD_DEFAULT_DESTRUCTOR_IMPLEMENT(uniform_magnetic_field);

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

  // EMFIELD_COMPUTE_EFIELD_IMPLEMENT_HEAD(uniform_magnetic_field,
  //                                       /*position_*/,
  //                                       /*time_*/,
  //                                       electric_field_)
  int uniform_magnetic_field::compute_electric_field(const geomtools::vector_3d & /* position_ */,
                                                      double /* time_ */,
                                                      geomtools::vector_3d & electric_field_) const
  {
    geomtools::invalidate(electric_field_);
    return STATUS_ERROR;
  }

  EMFIELD_COMPUTE_BFIELD_IMPLEMENT_HEAD(uniform_magnetic_field,
                                        position_,
                                        /*time_*/,
                                        magnetic_field_)
  {
    DT_LOG_DEBUG(get_logging_priority(), "*** COMPUTE UNIFORM MAGNETIC FIELD ***");
    magnetic_field_ = _uniform_magnetic_field_;
    DT_LOG_DEBUG(get_logging_priority(),
                 "Magnetic field values @ " << position_/CLHEP::mm << " mm = "
                 << magnetic_field_/CLHEP::gauss << " G");
    return STATUS_SUCCESS;
  }

  EMFIELD_RESET_IMPLEMENT_HEAD(uniform_magnetic_field)
  {
    DT_THROW_IF(! is_initialized(), std::logic_error, "Cannot reset the magnetic field !");

    _set_initialized(false);

     geomtools::invalidate(_uniform_magnetic_field_);
    return;
  }

  EMFIELD_INITIALIZE_IMPLEMENT_HEAD(uniform_magnetic_field,setup_,service_manager_,fields_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Field is already initialized !");

    base_electromagnetic_field::_parse_basic_parameters(setup_, service_manager_, fields_);

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

} // end of namespace emfield
